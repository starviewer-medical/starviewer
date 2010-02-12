#include "qexperimental3dextension.h"

#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageMathematics.h>

#include "experimental3dvolume.h"
#include "informationtheory.h"
#include "logging.h"
#include "mathtools.h"
#include "obscurancemainthread.h"
#include "transferfunctionio.h"
#include "vector3.h"
#include "viewpointgenerator.h"
#include "viewpointinformationchannel.h"
#include "viewpointintensityinformationchannel.h"
#include "volumereslicer.h"
#include "experimental3dsettings.h"

#include <QButtonGroup>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QStringListModel>
#include <QTextStream>
#include <QTime>

#ifdef CUDA_AVAILABLE
#include "cudafiltering.h"
#endif


namespace udg {


QExperimental3DExtension::QExperimental3DExtension( QWidget *parent )
 : QWidget( parent ), m_volume( 0 ),
   m_computingObscurance( false ), m_obscuranceMainThread( 0 ), m_obscurance( 0 ), m_interactive( true )
{
    setupUi( this );
    Experimental3DSettings().init();

    createConnections();

    QButtonGroup *bestViewpointsRadioButtons = new QButtonGroup( this );
    bestViewpointsRadioButtons->addButton( m_computeBestViewsNRadioButton );
    bestViewpointsRadioButtons->addButton( m_computeBestViewsThresholdRadioButton );

    m_colorVomiPalette << Vector3Float( 1.0f, 1.0f, 1.0f );

    m_recentTransferFunctionsModel = new QStringListModel( this );
    m_recentTransferFunctionsListView->setModel( m_recentTransferFunctionsModel );

#ifndef CUDA_AVAILABLE
    m_vmiDisplayCheckBox->setChecked( true );
    m_vmiDisplayCheckBox->setEnabled( false );
#endif // CUDA_AVAILABLE
}


QExperimental3DExtension::~QExperimental3DExtension()
{
    delete m_volume;

    if ( m_computingObscurance )
    {
        m_obscuranceMainThread->stop();
        m_obscuranceMainThread->wait();
    }

    delete m_obscuranceMainThread;
    delete m_obscurance;
}


void QExperimental3DExtension::setInput( Volume *input )
{
    m_volume = new Experimental3DVolume( input );

    m_viewer->setInput( input );
    m_viewer->setVolume( m_volume );

    unsigned short max = m_volume->getRangeMax();
    m_transferFunctionEditor->setRange( 0, max );
    TransferFunction defaultTransferFunction;
    defaultTransferFunction.addPoint( 0, QColor( 0, 0, 0, 0 ) );
    defaultTransferFunction.addPoint( max, QColor( 255, 255, 255, 255 ) );
    m_transferFunctionEditor->setTransferFunction( defaultTransferFunction );

    setTransferFunction( false );
    render();
}


void QExperimental3DExtension::setNewVolume( Volume *volume )
{
    m_viewer->removeCurrentVolume();
    delete m_volume;
    m_volume = new Experimental3DVolume( volume );
    m_viewer->setVolume( m_volume );
    render();
}


void QExperimental3DExtension::saveViewedVolume( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToSave( Experimental3DSettings::ViewedVolumeDir, tr("Save viewed volume"), tr("Text files (*.txt);;All files (*)"), "txt" );
        if ( fileName.isNull() ) return;
    }

    if ( !saveFloatDataAsText( m_viewedVolume, fileName, QString( "volume(v%1) = %2" ), 1 ) && m_interactive )
        QMessageBox::warning( this, tr("Can't save viewed volume"), QString( tr("Can't save viewed volume to file ") ) + fileName );
}


void QExperimental3DExtension::loadViewpointEntropy( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::ViewpointEntropyDir, tr("Load viewpoint entropy"), tr("Data files (*.dat);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    if ( loadData( fileName, m_viewpointEntropy ) ) m_saveViewpointEntropyPushButton->setEnabled( true );
    else if ( m_interactive ) QMessageBox::warning( this, tr("Can't load viewpoint entropy"), QString( tr("Can't load viewpoint entropy from file ") ) + fileName );
}


void QExperimental3DExtension::saveViewpointEntropy( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToSave( Experimental3DSettings::ViewpointEntropyDir, tr("Save viewpoint entropy"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
        if ( fileName.isNull() ) return;
    }

    bool error;

    if ( fileName.endsWith( ".txt" ) ) error = !saveFloatDataAsText( m_viewpointEntropy, fileName, QString( "H(Z|v%1) = %2" ), 1 );
    else error = !saveData( m_viewpointEntropy, fileName );

    if ( error && m_interactive ) QMessageBox::warning( this, tr("Can't save viewpoint entropy"), QString( tr("Can't save viewpoint entropy to file ") ) + fileName );
}


void QExperimental3DExtension::loadEntropy( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::EntropyDir, tr("Load entropy"), tr("Data files (*.dat);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    if ( loadFloatData( fileName, m_entropy ) ) m_saveEntropyPushButton->setEnabled( true );
    else if ( m_interactive ) QMessageBox::warning( this, tr("Can't load entropy"), QString( tr("Can't load entropy from file ") ) + fileName );
}


void QExperimental3DExtension::saveEntropy( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName =getFileNameToSave( Experimental3DSettings::EntropyDir, tr("Save entropy"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
        if ( fileName.isNull() ) return;
    }

    bool error;

    if ( fileName.endsWith( ".txt" ) ) error = !saveFloatDataAsText( m_entropy, fileName, QString( "H(Z) = %1" ) );
    else error = !saveFloatData( m_entropy, fileName );

    if ( error && m_interactive ) QMessageBox::warning( this, tr("Can't save entropy"), QString( tr("Can't save entropy to file ") ) + fileName );
}


void QExperimental3DExtension::loadVmi( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::ViewpointMutualInformationDir, tr("Load VMI"), tr("Data files (*.dat);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    if ( loadData( fileName, m_vmi ) ) m_saveVmiPushButton->setEnabled( true );
    else if ( m_interactive ) QMessageBox::warning( this, tr("Can't load VMI"), QString( tr("Can't load VMI from file ") ) + fileName );
}


void QExperimental3DExtension::saveVmi( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToSave( Experimental3DSettings::ViewpointMutualInformationDir, tr("Save VMI"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
        if ( fileName.isNull() ) return;
    }

    bool error;

    if ( fileName.endsWith( ".txt" ) ) error = !saveFloatDataAsText( m_vmi, fileName, QString( "VMI(v%1) = %2" ), 1 );
    else error = !saveData( m_vmi, fileName );

    if ( error && m_interactive ) QMessageBox::warning( this, tr("Can't save VMI"), QString( tr("Can't save VMI to file ") ) + fileName );
}


void QExperimental3DExtension::loadMi( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::MutualInformationDir, tr("Load MI"), tr("Data files (*.dat);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    if ( loadFloatData( fileName, m_mi ) ) m_saveMiPushButton->setEnabled( true );
    else if ( m_interactive ) QMessageBox::warning( this, tr("Can't load MI"), QString( tr("Can't load MI from file ") ) + fileName );
}


void QExperimental3DExtension::saveMi( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToSave( Experimental3DSettings::MutualInformationDir, tr("Save MI"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
        if ( fileName.isNull() ) return;
    }

    bool error;

    if ( fileName.endsWith( ".txt" ) ) error = !saveFloatDataAsText( m_mi, fileName, QString( "I(V;Z) = %1" ) );
    else error = !saveFloatData( m_mi, fileName );

    if ( error && m_interactive ) QMessageBox::warning( this, tr("Can't save MI"), QString( tr("Can't save MI to file ") ) + fileName );
}


void QExperimental3DExtension::loadViewpointUnstabilities( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::ViewpointUnstabilitiesDir, tr("Load viewpoint unstabilities"), tr("Data files (*.dat);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    if ( loadData( fileName, m_viewpointUnstabilities ) ) m_saveViewpointUnstabilitiesPushButton->setEnabled( true );
    else if ( m_interactive ) QMessageBox::warning( this, tr("Can't load viewpoint unstabilities"), QString( tr("Can't load viewpoint unstabilities from file ") ) + fileName );
}


void QExperimental3DExtension::saveViewpointUnstabilities( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToSave( Experimental3DSettings::ViewpointUnstabilitiesDir, tr("Save viewpoint unstabilities"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
        if ( fileName.isNull() ) return;
    }

    bool error;

    if ( fileName.endsWith( ".txt" ) ) error = !saveFloatDataAsText( m_viewpointUnstabilities, fileName, QString( "U(v%1) = %2" ), 1 );
    else error = !saveData( m_viewpointUnstabilities, fileName );

    if ( error && m_interactive ) QMessageBox::warning( this, tr("Can't save viewpoint unstabilities"), QString( tr("Can't save viewpoint unstabilities to file ") ) + fileName );
}


void QExperimental3DExtension::loadVomi( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::VoxelMutualInformationDir, tr("Load VoMI"), tr("Data files (*.dat);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    if ( loadData( fileName, m_vomi ) )
    {
        int nVoxels = m_vomi.size();
        m_maximumVomi = 0.0f;

        for ( int j = 0; j < nVoxels; j++ ) if ( m_vomi.at( j ) > m_maximumVomi ) m_maximumVomi = m_vomi.at( j );

        m_baseVomiRadioButton->setEnabled( true );
        m_baseVomiCoolWarmRadioButton->setEnabled( true );
        m_vomiCheckBox->setEnabled( true );
        m_vomiCoolWarmCheckBox->setEnabled( true );
        m_opacityLabel->setEnabled( true );
        m_opacityVomiCheckBox->setEnabled( true );
        m_saveVomiPushButton->setEnabled( true );
        m_vomiGradientPushButton->setEnabled( true );
    }
    else if ( m_interactive ) QMessageBox::warning( this, tr("Can't load VoMI"), QString( tr("Can't load VoMI from file ") ) + fileName );
}


void QExperimental3DExtension::saveVomi( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToSave( Experimental3DSettings::VoxelMutualInformationDir, tr("Save VoMI"), tr("Data files (*.dat);;All files (*)"), "dat" );
        if ( fileName.isNull() ) return;
    }

    if ( !saveData( m_vomi, fileName ) && m_interactive ) QMessageBox::warning( this, tr("Can't save VoMI"), QString( tr("Can't save VoMI to file ") ) + fileName );
}


void QExperimental3DExtension::loadViewpointVomi( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::ViewpointVoxelMutualInformationDir, tr("Load viewpoint VoMI"), tr("Data files (*.dat);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    if ( loadData( fileName, m_viewpointVomi ) ) m_saveViewpointVomiPushButton->setEnabled( true );
    else if ( m_interactive ) QMessageBox::warning( this, tr("Can't load viewpoint VoMI"), QString( tr("Can't load viewpoint VoMI from file ") ) + fileName );
}


void QExperimental3DExtension::saveViewpointVomi( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToSave( Experimental3DSettings::ViewpointVoxelMutualInformationDir, tr("Save viewpoint VoMI"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
        if ( fileName.isNull() ) return;
    }

    bool error;

    if ( fileName.endsWith( ".txt" ) ) error = !saveFloatDataAsText( m_viewpointVomi, fileName, QString( "VVoMI(v%1) = %2" ), 1 );
    else error = !saveData( m_viewpointVomi, fileName );

    if ( error && m_interactive ) QMessageBox::warning( this, tr("Can't save viewpoint VoMI"), QString( tr("Can't save viewpoint VoMI to file ") ) + fileName );
}


void QExperimental3DExtension::loadColorVomiPalette( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::ColorVoxelMutualInformationPaletteDir, tr("Load color VoMI palette"), tr("Text files (*.txt);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    QFile colorVomiPaletteFile( fileName );

    if ( !colorVomiPaletteFile.open( QFile::ReadOnly | QFile::Text ) )
    {
        DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't load color VoMI palette"), QString( tr("Can't load color VoMI palette from file ") ) + fileName );
        return;
    }

    m_colorVomiPalette.clear();

    QTextStream in( &colorVomiPaletteFile );

    while ( !in.atEnd() )
    {
        QString line = in.readLine();
        QStringList numbers = line.split( ' ', QString::SkipEmptyParts );

        if ( numbers.size() < 3 ) continue;

        Vector3Float color;

        if ( numbers.at( 0 ).contains( '.' ) )  // reals [0,1]
        {
            color.x = numbers.at( 0 ).toFloat();
            color.y = numbers.at( 1 ).toFloat();
            color.z = numbers.at( 2 ).toFloat();
        }
        else    // enters [0,255]
        {
            color.x = static_cast<unsigned char>( numbers.at( 0 ).toUShort() ) / 255.0f;
            color.y = static_cast<unsigned char>( numbers.at( 1 ).toUShort() ) / 255.0f;
            color.z = static_cast<unsigned char>( numbers.at( 2 ).toUShort() ) / 255.0f;
        }

        m_colorVomiPalette << color;
    }

    if ( m_colorVomiPalette.isEmpty() ) m_colorVomiPalette << Vector3Float( 1.0f, 1.0f, 1.0f );

    colorVomiPaletteFile.close();
}


void QExperimental3DExtension::loadColorVomi( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::ColorVoxelMutualInformationDir, tr("Load color VoMI"), tr("Data files (*.dat);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    if ( loadData( fileName, m_colorVomi ) )
    {
        int nVoxels = m_colorVomi.size();
        m_maximumColorVomi = 0.0f;

        for ( int j = 0; j < nVoxels; j++ )
        {
            const Vector3Float &colorVomi = m_colorVomi.at( j );
            if ( colorVomi.x > m_maximumColorVomi ) m_maximumColorVomi = colorVomi.x;
            if ( colorVomi.y > m_maximumColorVomi ) m_maximumColorVomi = colorVomi.y;
            if ( colorVomi.z > m_maximumColorVomi ) m_maximumColorVomi = colorVomi.z;
        }

        m_baseColorVomiRadioButton->setEnabled( true );
        m_colorVomiCheckBox->setEnabled( true );
        m_saveColorVomiPushButton->setEnabled( true );
    }
    else if ( m_interactive ) QMessageBox::warning( this, tr("Can't load color VoMI"), QString( tr("Can't load color VoMI from file ") ) + fileName );
}


void QExperimental3DExtension::saveColorVomi( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToSave( Experimental3DSettings::ColorVoxelMutualInformationDir, tr("Save color VoMI"), tr("Data files (*.dat);;All files (*)"), "dat" );
        if ( fileName.isNull() ) return;
    }

    if ( !saveData( m_colorVomi, fileName ) && m_interactive ) QMessageBox::warning( this, tr("Can't save color VoMI"), QString( tr("Can't save color VoMI to file ") ) + fileName );
}


void QExperimental3DExtension::loadEvmiOpacityOtherTransferFunction( QString fileName )
{
    if ( !m_computeEvmiOpacityUseOtherPushButton->isChecked() ) return;

    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::TransferFunctionDir, tr("Load transfer function"), tr("XML files (*.xml);;Transfer function files (*.tf);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    TransferFunction *transferFunction;

    if ( fileName.endsWith( ".xml" ) ) transferFunction = TransferFunctionIO::fromXmlFile( fileName );
    else transferFunction = TransferFunctionIO::fromFile( fileName );

    m_evmiOpacityTransferFunction = *transferFunction;

    delete transferFunction;
}


void QExperimental3DExtension::loadEvmiOpacity( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::EnhancedViewpointMutualInformationOpacityDir, tr("Load EVMI with opacity"), tr("Data files (*.dat);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    if ( loadData( fileName, m_evmiOpacity ) ) m_saveEvmiOpacityPushButton->setEnabled( true );
    else if ( m_interactive ) QMessageBox::warning( this, tr("Can't load EVMI with opacity"), QString( tr("Can't load EVMI with opacity from file ") ) + fileName );
}


void QExperimental3DExtension::saveEvmiOpacity( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToSave( Experimental3DSettings::EnhancedViewpointMutualInformationOpacityDir, tr("Save EVMI with opacity"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
        if ( fileName.isNull() ) return;
    }

    bool error;

    if ( fileName.endsWith( ".txt" ) ) error = !saveFloatDataAsText( m_evmiOpacity, fileName, QString( "EVMI(v%1) = %2" ), 1 );
    else error = !saveData( m_evmiOpacity, fileName );

    if ( error && m_interactive ) QMessageBox::warning( this, tr("Can't save EVMI with opacity"), QString( tr("Can't save EVMI with opacity to file ") ) + fileName );
}


void QExperimental3DExtension::loadEvmiVomi( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::EnhancedViewpointMutualInformationVoxelMutualInformationDir, tr("Load EVMI with VoMI"), tr("Data files (*.dat);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    if ( loadData( fileName, m_evmiVomi ) ) m_saveEvmiVomiPushButton->setEnabled( true );
    else if ( m_interactive ) QMessageBox::warning( this, tr("Can't load EVMI with VoMI"), QString( tr("Can't load EVMI with VoMI from file ") ) + fileName );
}


void QExperimental3DExtension::saveEvmiVomi( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToSave( Experimental3DSettings::EnhancedViewpointMutualInformationVoxelMutualInformationDir, tr("Save EVMI with VoMI"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
        if ( fileName.isNull() ) return;
    }

    bool error;

    if ( fileName.endsWith( ".txt" ) ) error = !saveFloatDataAsText( m_evmiVomi, fileName, QString( "EVMI(v%1) = %2" ), 1 );
    else error = !saveData( m_evmiVomi, fileName );

    if ( error && m_interactive ) QMessageBox::warning( this, tr("Can't save EVMI with VoMI"), QString( tr("Can't save EVMI with VoMI to file ") ) + fileName );
}


void QExperimental3DExtension::loadBestViews( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::BestViewsDir, tr("Load best views"), tr("Data files (*.dat);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    if ( loadData( fileName, m_bestViews ) )
    {
        m_saveBestViewsPushButton->setEnabled( true );
        m_tourBestViewsPushButton->setEnabled( true );
    }
    else if ( m_interactive ) QMessageBox::warning( this, tr("Can't load best views"), QString( tr("Can't load best views from file ") ) + fileName );
}


void QExperimental3DExtension::saveBestViews( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToSave( Experimental3DSettings::BestViewsDir, tr("Save best views"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
        if ( fileName.isNull() ) return;
    }

    bool error;

    if ( fileName.endsWith( ".txt" ) ) error = !saveDataAsText( m_bestViews, fileName, QString( "%1: v%2 %3" ), 0, 1 );
    else error = !saveData( m_bestViews, fileName );

    if ( error && m_interactive ) QMessageBox::warning( this, tr("Can't save best views"), QString( tr("Can't save best views to file ") ) + fileName );
}


void QExperimental3DExtension::loadGuidedTour( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::GuidedTourDir, tr("Load guided tour"), tr("Data files (*.dat);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    if ( loadData( fileName, m_guidedTour ) )
    {
        m_saveGuidedTourPushButton->setEnabled( true );
        m_guidedTourPushButton->setEnabled( true );
    }
    else if ( m_interactive ) QMessageBox::warning( this, tr("Can't load guided tour"), QString( tr("Can't load guided tour from file ") ) + fileName );
}


void QExperimental3DExtension::saveGuidedTour( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToSave( Experimental3DSettings::GuidedTourDir, tr("Save guided tour"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
        if ( fileName.isNull() ) return;
    }

    bool error;

    if ( fileName.endsWith( ".txt" ) ) error = !saveDataAsText( m_guidedTour, fileName, QString( "%1: v%2 %3" ), 0, 1 );
    else error = !saveData( m_guidedTour, fileName );

    if ( error && m_interactive )QMessageBox::warning( this, tr("Can't save guided tour"), QString( tr("Can't save guided tour to file ") ) + fileName );
}


void QExperimental3DExtension::loadExploratoryTour( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::ExploratoryTourDir, tr("Load exploratory tour"), tr("Data files (*.dat);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    if ( loadData( fileName, m_exploratoryTour ) )
    {
        m_saveExploratoryTourPushButton->setEnabled( true );
        m_exploratoryTourPushButton->setEnabled( true );
    }
    else if ( m_interactive ) QMessageBox::warning( this, tr("Can't load exploratory tour"), QString( tr("Can't load exploratory tour from file ") ) + fileName );
}


void QExperimental3DExtension::saveExploratoryTour( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToSave( Experimental3DSettings::ExploratoryTourDir, tr("Save exploratory tour"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
        if ( fileName.isNull() ) return;
    }

    bool error;

    if ( fileName.endsWith( ".txt" ) ) error = !saveDataAsText( m_exploratoryTour, fileName, QString( "%1: v%2 %3" ), 0, 1 );
    else error = !saveData( m_exploratoryTour, fileName );

    if ( error && m_interactive )QMessageBox::warning( this, tr("Can't save exploratory tour"), QString( tr("Can't save exploratory tour to file ") ) + fileName );
}


void QExperimental3DExtension::saveViewedVolumeI( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToSave( Experimental3DSettings::ViewedVolumeIntensityDir, tr("Save viewed volume"), tr("Text files (*.txt);;All files (*)"), "txt" );
        if ( fileName.isNull() ) return;
    }

    if ( !saveFloatDataAsText( m_viewedVolumeI, fileName, QString( "volume(v%1) = %2" ), 1 ) && m_interactive )
        QMessageBox::warning( this, tr("Can't save viewed volume"), QString( tr("Can't save viewed volume to file ") ) + fileName );
}


void QExperimental3DExtension::loadViewpointEntropyI( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::ViewpointEntropyIntensityDir, tr("Load viewpoint entropy"), tr("Data files (*.dat);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    if ( loadData( fileName, m_viewpointEntropyI ) ) m_saveViewpointEntropyIPushButton->setEnabled( true );
    else if ( m_interactive ) QMessageBox::warning( this, tr("Can't load viewpoint entropy"), QString( tr("Can't load viewpoint entropy from file ") ) + fileName );
}


void QExperimental3DExtension::saveViewpointEntropyI( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToSave( Experimental3DSettings::ViewpointEntropyIntensityDir, tr("Save viewpoint entropy"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
        if ( fileName.isNull() ) return;
    }

    bool error;

    if ( fileName.endsWith( ".txt" ) ) error = !saveFloatDataAsText( m_viewpointEntropyI, fileName, QString( "H(I|v%1) = %2" ), 1 );
    else error = !saveData( m_viewpointEntropyI, fileName );

    if ( error && m_interactive ) QMessageBox::warning( this, tr("Can't save viewpoint entropy"), QString( tr("Can't save viewpoint entropy to file ") ) + fileName );
}


void QExperimental3DExtension::loadEntropyI( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::EntropyIntensityDir, tr("Load entropy"), tr("Data files (*.dat);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    if ( loadFloatData( fileName, m_entropyI ) ) m_saveEntropyIPushButton->setEnabled( true );
    else if ( m_interactive ) QMessageBox::warning( this, tr("Can't load entropy"), QString( tr("Can't load entropy from file ") ) + fileName );
}


void QExperimental3DExtension::saveEntropyI( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName =getFileNameToSave( Experimental3DSettings::EntropyIntensityDir, tr("Save entropy"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
        if ( fileName.isNull() ) return;
    }

    bool error;

    if ( fileName.endsWith( ".txt" ) ) error = !saveFloatDataAsText( m_entropyI, fileName, QString( "H(I) = %1" ) );
    else error = !saveFloatData( m_entropyI, fileName );

    if ( error && m_interactive ) QMessageBox::warning( this, tr("Can't save entropy"), QString( tr("Can't save entropy to file ") ) + fileName );
}


void QExperimental3DExtension::loadVmii( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::ViewpointMutualInformationIntensityDir, tr("Load VMIi"), tr("Data files (*.dat);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    if ( loadData( fileName, m_vmii ) ) m_saveVmiiPushButton->setEnabled( true );
    else if ( m_interactive ) QMessageBox::warning( this, tr("Can't load VMIi"), QString( tr("Can't load VMIi from file ") ) + fileName );
}


void QExperimental3DExtension::saveVmii( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToSave( Experimental3DSettings::ViewpointMutualInformationIntensityDir, tr("Save VMIi"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
        if ( fileName.isNull() ) return;
    }

    bool error;

    if ( fileName.endsWith( ".txt" ) ) error = !saveFloatDataAsText( m_vmii, fileName, QString( "VMIi(v%1) = %2" ), 1 );
    else error = !saveData( m_vmii, fileName );

    if ( error && m_interactive ) QMessageBox::warning( this, tr("Can't save VMIi"), QString( tr("Can't save VMIi to file ") ) + fileName );
}


void QExperimental3DExtension::loadMii( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::MutualInformationIntensityDir, tr("Load MIi"), tr("Data files (*.dat);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    if ( loadFloatData( fileName, m_mii ) ) m_saveMiiPushButton->setEnabled( true );
    else if ( m_interactive ) QMessageBox::warning( this, tr("Can't load MIi"), QString( tr("Can't load MIi from file ") ) + fileName );
}


void QExperimental3DExtension::saveMii( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToSave( Experimental3DSettings::MutualInformationIntensityDir, tr("Save MIi"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
        if ( fileName.isNull() ) return;
    }

    bool error;

    if ( fileName.endsWith( ".txt" ) ) error = !saveFloatDataAsText( m_mii, fileName, QString( "I(V;I) = %1" ) );
    else error = !saveFloatData( m_mii, fileName );

    if ( error && m_interactive ) QMessageBox::warning( this, tr("Can't save MIi"), QString( tr("Can't save MIi to file ") ) + fileName );
}


void QExperimental3DExtension::loadViewpointUnstabilitiesI( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::ViewpointUnstabilitiesIntensityDir, tr("Load viewpoint unstabilities"), tr("Data files (*.dat);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    if ( loadData( fileName, m_viewpointUnstabilitiesI ) ) m_saveViewpointUnstabilitiesIPushButton->setEnabled( true );
    else if ( m_interactive ) QMessageBox::warning( this, tr("Can't load viewpoint unstabilities"), QString( tr("Can't load viewpoint unstabilities from file ") ) + fileName );
}


void QExperimental3DExtension::saveViewpointUnstabilitiesI( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToSave( Experimental3DSettings::ViewpointUnstabilitiesIntensityDir, tr("Save viewpoint unstabilities"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
        if ( fileName.isNull() ) return;
    }

    bool error;

    if ( fileName.endsWith( ".txt" ) ) error = !saveFloatDataAsText( m_viewpointUnstabilitiesI, fileName, QString( "U(v%1) = %2" ), 1 );
    else error = !saveData( m_viewpointUnstabilitiesI, fileName );

    if ( error && m_interactive ) QMessageBox::warning( this, tr("Can't save viewpoint unstabilities"), QString( tr("Can't save viewpoint unstabilities to file ") ) + fileName );
}


void QExperimental3DExtension::loadImi( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToLoad( Experimental3DSettings::IntensityMutualInformationDir, tr("Load IMI"), tr("Data files (*.dat);;All files (*)") );
        if ( fileName.isNull() ) return;
    }

    if ( loadData( fileName, m_imi ) )
    {
        int nIntensities = m_imi.size();
        m_maximumImi = 0.0f;

        for ( int j = 0; j < nIntensities; j++ ) if ( m_imi.at( j ) > m_maximumImi ) m_maximumImi = m_imi.at( j );

        m_baseImiRadioButton->setEnabled( true );
//        m_baseImiCoolWarmRadioButton->setEnabled( true );
//        m_imiCheckBox->setEnabled( true );
//        m_imiCoolWarmCheckBox->setEnabled( true );
//        m_opacityLabel->setEnabled( true );
//        m_opacityImiCheckBox->setEnabled( true );
        m_saveImiPushButton->setEnabled( true );
//        m_imiGradientPushButton->setEnabled( true );
    }
    else if ( m_interactive ) QMessageBox::warning( this, tr("Can't load IMI"), QString( tr("Can't load IMI from file ") ) + fileName );
}


void QExperimental3DExtension::saveImi( QString fileName )
{
    if ( fileName.isEmpty() )
    {
        fileName = getFileNameToSave( Experimental3DSettings::IntensityMutualInformationDir, tr("Save IMI"), tr("Data files (*.dat);;All files (*)"), "dat" );
        if ( fileName.isNull() ) return;
    }

    if ( !saveData( m_imi, fileName ) && m_interactive ) QMessageBox::warning( this, tr("Can't save IMI"), QString( tr("Can't save IMI to file ") ) + fileName );
}


bool QExperimental3DExtension::loadFloatData( const QString &fileName, float &data )
{
    QFile file( fileName );

    if ( !file.open( QIODevice::ReadOnly ) )
    {
        DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + fileName );
        return false;
    }

    QDataStream in( &file );

    if ( !in.atEnd() ) in >> data;

    file.close();

    return true;
}


template <class T>
bool QExperimental3DExtension::loadData( const QString &fileName, QList<T> &list )
{
    QFile file( fileName );

    if ( !file.open( QIODevice::ReadOnly ) )
    {
        DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + fileName );
        return false;
    }

    list.clear();

    QDataStream in( &file );

    while ( !in.atEnd() )
    {
        T data;
        in >> data;
        list << data;
    }

    file.close();

    return true;
}


template <class T>
bool QExperimental3DExtension::loadData( const QString &fileName, QVector<T> &vector )
{
    QFile file( fileName );

    if ( !file.open( QIODevice::ReadOnly ) )
    {
        DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + fileName );
        return false;
    }

    vector.clear();

    QDataStream in( &file );

    while ( !in.atEnd() )
    {
        T data;
        in >> data;
        vector << data;
    }

    file.close();

    return true;
}


bool QExperimental3DExtension::saveFloatData( float data, const QString &fileName )
{
    QFile file( fileName );

    if ( !file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
    {
        DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + fileName );
        return false;
    }

    QDataStream out( &file );

    out << data;

    file.close();

    return true;
}


template <class T>
bool QExperimental3DExtension::saveData( const QList<T> &list, const QString &fileName )
{
    QFile file( fileName );

    if ( !file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
    {
        DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + fileName );
        return false;
    }

    QDataStream out( &file );
    int n = list.size();

    for ( int i = 0; i < n; i++ ) out << list.at( i );

    file.close();

    return true;
}


template <class T>
bool QExperimental3DExtension::saveData( const QVector<T> &vector, const QString &fileName )
{
    QFile file( fileName );

    if ( !file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
    {
        DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + fileName );
        return false;
    }

    QDataStream out( &file );
    int n = vector.size();

    for ( int i = 0; i < n; i++ ) out << vector.at( i );

    file.close();

    return true;
}


bool QExperimental3DExtension::saveFloatDataAsText( float data, const QString &fileName, const QString &format )
{
    QFile file( fileName );

    if ( !file.open( QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text ) )
    {
        DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + fileName );
        return false;
    }

    QTextStream out( &file );

    out << format.arg( data ) << "\n";

    file.close();

    return true;
}


bool QExperimental3DExtension::saveFloatDataAsText( const QVector<float> &vector, const QString &fileName, const QString &format, int base )
{
    QFile file( fileName );

    if ( !file.open( QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text ) )
    {
        DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + fileName );
        return false;
    }

    QTextStream out( &file );
    int n = vector.size();

    for ( int i = 0; i < n; i++ ) out << format.arg( i + base ).arg( vector.at( i ) ) << "\n";

    file.close();

    return true;
}


bool QExperimental3DExtension::saveDataAsText( const QList< QPair<int, Vector3> > &list, const QString &fileName, const QString &format, int base1, int base2 )
{
    QFile file( fileName );

    if ( !file.open( QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text ) )
    {
        DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + fileName );
        return false;
    }

    QTextStream out( &file );
    int n = list.size();

    for ( int i = 0; i < n; i++ ) out << format.arg( i + base1 ).arg( list.at( i ).first + base2 ).arg( list.at( i ).second.toString() ) << "\n";

    file.close();

    return true;
}


void QExperimental3DExtension::createConnections()
{
    connect( m_viewer, SIGNAL( volumeChanged(Volume*) ), SLOT( setNewVolume(Volume*) ) );
    
    // visualització
    connect( m_backgroundColorPushButton, SIGNAL( clicked() ), SLOT( chooseBackgroundColor() ) );
    connect( m_baseDiffuseLightingRadioButton, SIGNAL( toggled(bool) ), SLOT( enableSpecularLighting(bool) ) );
    connect( m_baseSpecularLightingCheckBox, SIGNAL( toggled(bool) ), m_baseSpecularLightingPowerLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseSpecularLightingCheckBox, SIGNAL( toggled(bool) ), m_baseSpecularLightingPowerDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_baseCoolWarmRadioButton, SIGNAL( toggled(bool) ), m_baseCoolWarmBLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseCoolWarmRadioButton, SIGNAL( toggled(bool) ), m_baseCoolWarmBDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_baseCoolWarmRadioButton, SIGNAL( toggled(bool) ), m_baseCoolWarmYLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseCoolWarmRadioButton, SIGNAL( toggled(bool) ), m_baseCoolWarmYDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_baseCoolWarmRadioButton, SIGNAL( toggled(bool) ), m_baseCoolWarmAlphaLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseCoolWarmRadioButton, SIGNAL( toggled(bool) ), m_baseCoolWarmAlphaDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_baseCoolWarmRadioButton, SIGNAL( toggled(bool) ), m_baseCoolWarmBetaLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseCoolWarmRadioButton, SIGNAL( toggled(bool) ), m_baseCoolWarmBetaDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_baseObscuranceRadioButton, SIGNAL( toggled(bool) ), m_baseObscuranceFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseObscuranceRadioButton, SIGNAL( toggled(bool) ), m_baseObscuranceFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_baseObscuranceRadioButton, SIGNAL( toggled(bool) ), m_baseObscuranceFiltersLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseObscuranceRadioButton, SIGNAL( toggled(bool) ), m_baseObscuranceLowFilterLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseObscuranceRadioButton, SIGNAL( toggled(bool) ), m_baseObscuranceLowFilterDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_baseObscuranceRadioButton, SIGNAL( toggled(bool) ), m_baseObscuranceHighFilterLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseObscuranceRadioButton, SIGNAL( toggled(bool) ), m_baseObscuranceHighFilterDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_baseVomiRadioButton, SIGNAL( toggled(bool) ), m_baseVomiFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseVomiRadioButton, SIGNAL( toggled(bool) ), m_baseVomiFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_baseVomiCoolWarmRadioButton, SIGNAL( toggled(bool) ), m_baseVomiCoolWarmYLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseVomiCoolWarmRadioButton, SIGNAL( toggled(bool) ), m_baseVomiCoolWarmYDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_baseVomiCoolWarmRadioButton, SIGNAL( toggled(bool) ), m_baseVomiCoolWarmBLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseVomiCoolWarmRadioButton, SIGNAL( toggled(bool) ), m_baseVomiCoolWarmBDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_baseVomiCoolWarmRadioButton, SIGNAL( toggled(bool) ), m_baseVomiCoolWarmFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseVomiCoolWarmRadioButton, SIGNAL( toggled(bool) ), m_baseVomiCoolWarmFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_baseColorVomiRadioButton, SIGNAL( toggled(bool) ), m_baseColorVomiFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseColorVomiRadioButton, SIGNAL( toggled(bool) ), m_baseColorVomiFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_baseImiRadioButton, SIGNAL( toggled(bool) ), m_baseImiFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseImiRadioButton, SIGNAL( toggled(bool) ), m_baseImiFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_baseVoxelSalienciesRadioButton, SIGNAL( toggled(bool) ), m_baseVoxelSalienciesFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseVoxelSalienciesRadioButton, SIGNAL( toggled(bool) ), m_baseVoxelSalienciesFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_baseFilteringAmbientOcclusionRadioButton, SIGNAL( toggled(bool) ), m_baseFilteringAmbientOcclusionTypeComboBox, SLOT( setEnabled(bool) ) );
    connect( m_baseFilteringAmbientOcclusionRadioButton, SIGNAL( toggled(bool) ), m_baseFilteringAmbientOcclusionFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseFilteringAmbientOcclusionRadioButton, SIGNAL( toggled(bool) ), m_baseFilteringAmbientOcclusionFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_baseProbabilisticAmbientOcclusionRadioButton, SIGNAL( toggled(bool) ), m_baseProbabilisticAmbientOcclusionGammaLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseProbabilisticAmbientOcclusionRadioButton, SIGNAL( toggled(bool) ), m_baseProbabilisticAmbientOcclusionGammaDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_baseProbabilisticAmbientOcclusionRadioButton, SIGNAL( toggled(bool) ), m_baseProbabilisticAmbientOcclusionFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseProbabilisticAmbientOcclusionRadioButton, SIGNAL( toggled(bool) ), m_baseProbabilisticAmbientOcclusionFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_additiveObscuranceVomiCheckBox, SIGNAL( toggled(bool) ), m_additiveObscuranceVomiWeightLabel, SLOT( setEnabled(bool) ) );
    connect( m_additiveObscuranceVomiCheckBox, SIGNAL( toggled(bool) ), m_additiveObscuranceVomiWeightDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_contourCheckBox, SIGNAL( toggled(bool) ), m_contourDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_renderingOkPushButton, SIGNAL( clicked() ), SLOT( render() ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceFiltersLabel, SLOT( setEnabled(bool) ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceLowFilterLabel, SLOT( setEnabled(bool) ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceLowFilterDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceHighFilterLabel, SLOT( setEnabled(bool) ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceHighFilterDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_vomiCheckBox, SIGNAL( toggled(bool) ), m_vomiFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_vomiCheckBox, SIGNAL( toggled(bool) ), m_vomiFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_vomiCoolWarmCheckBox, SIGNAL( toggled(bool) ), m_vomiCoolWarmYLabel, SLOT( setEnabled(bool) ) );
    connect( m_vomiCoolWarmCheckBox, SIGNAL( toggled(bool) ), m_vomiCoolWarmYDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_vomiCoolWarmCheckBox, SIGNAL( toggled(bool) ), m_vomiCoolWarmBLabel, SLOT( setEnabled(bool) ) );
    connect( m_vomiCoolWarmCheckBox, SIGNAL( toggled(bool) ), m_vomiCoolWarmBDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_vomiCoolWarmCheckBox, SIGNAL( toggled(bool) ), m_vomiCoolWarmFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_vomiCoolWarmCheckBox, SIGNAL( toggled(bool) ), m_vomiCoolWarmFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_colorVomiCheckBox, SIGNAL( toggled(bool) ), m_colorVomiFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_colorVomiCheckBox, SIGNAL( toggled(bool) ), m_colorVomiFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_opacityVomiCheckBox, SIGNAL( toggled(bool) ), SLOT( opacityVomiChecked(bool) ) );
    connect( m_opacitySaliencyCheckBox, SIGNAL( toggled(bool) ), SLOT( opacitySaliencyChecked(bool) ) );
    connect( m_opacityFilteringCheckBox, SIGNAL( toggled(bool) ), SLOT( opacityFilteringChecked(bool) ) );
    connect( m_opacityProbabilisticAmbientOcclusionCheckBox, SIGNAL( toggled(bool) ), SLOT( opacityProbabilisticAmbientOcclusionChecked(bool) ) );
    connect( m_filteringAmbientOcclusionCheckBox, SIGNAL( toggled(bool) ), m_filteringAmbientOcclusionLambdaLabel, SLOT( setEnabled(bool) ) );
    connect( m_filteringAmbientOcclusionCheckBox, SIGNAL( toggled(bool) ), m_filteringAmbientOcclusionLambdaDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_probabilisticAmbientOcclusionCheckBox, SIGNAL( toggled(bool) ), m_probabilisticAmbientOcclusionGammaLabel, SLOT( setEnabled(bool) ) );
    connect( m_probabilisticAmbientOcclusionCheckBox, SIGNAL( toggled(bool) ), m_probabilisticAmbientOcclusionGammaDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_probabilisticAmbientOcclusionCheckBox, SIGNAL( toggled(bool) ), m_probabilisticAmbientOcclusionFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_probabilisticAmbientOcclusionCheckBox, SIGNAL( toggled(bool) ), m_probabilisticAmbientOcclusionFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_opacityVarianceCheckBox, SIGNAL( toggled(bool) ), m_opacityVarianceMaxLabel, SLOT( setEnabled(bool) ) );
    connect( m_opacityVarianceCheckBox, SIGNAL( toggled(bool) ), m_opacityVarianceMaxDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_celShadingCheckBox, SIGNAL( toggled(bool) ), m_celShadingQuantumsLabel, SLOT( setEnabled(bool) ) );
    connect( m_celShadingCheckBox, SIGNAL( toggled(bool) ), m_celShadingQuantumsSpinBox, SLOT( setEnabled(bool) ) );

    // funcions de transferència
    connect( m_loadTransferFunctionPushButton, SIGNAL( clicked() ), SLOT( loadTransferFunction() ) );
    connect( m_saveTransferFunctionPushButton, SIGNAL( clicked() ), SLOT( saveTransferFunction() ) );
    connect( m_addRecentTransferFunctionPushButton, SIGNAL( clicked() ), SLOT( addRecentTransferFunction() ) );
    connect( m_recentTransferFunctionsListView, SIGNAL( doubleClicked(const QModelIndex&) ), SLOT( setRecentTransferFunction(const QModelIndex&) ) );
    connect( m_transferFunctionOkPushButton, SIGNAL( clicked() ), SLOT( setTransferFunction() ) );

    // càmera
    connect( m_cameraGetPushButton, SIGNAL( clicked() ), SLOT( getCamera() ) );
    connect( m_cameraSetPushButton, SIGNAL( clicked() ), SLOT( setCamera() ) );
    connect( m_cameraLoadPushButton, SIGNAL( clicked() ), SLOT( loadCamera() ) );
    connect( m_cameraSavePushButton, SIGNAL( clicked() ), SLOT( saveCamera() ) );
    connect( m_cameraViewpointDistributionWidget, SIGNAL( numberOfViewpointsChanged(int) ), SLOT( setNumberOfViewpoints(int) ) );
    connect( m_viewpointPushButton, SIGNAL( clicked() ), SLOT( setViewpoint() ) );
    connect( m_tourPushButton, SIGNAL( clicked() ), SLOT( tour() ) );
    connect( m_saveNextTourCheckBox, SIGNAL( toggled(bool) ), m_saveNextTourLineEdit, SLOT( setEnabled(bool) ) );
    connect( m_saveNextTourCheckBox, SIGNAL( toggled(bool) ), m_saveNextTourPushButton, SLOT( setEnabled(bool) ) );
    connect( m_saveNextTourPushButton, SIGNAL( clicked() ), SLOT( getFileNameToSaveTour() ) );

    // obscurances
    connect( m_obscurancePushButton, SIGNAL( clicked() ), SLOT( computeCancelObscurance() ) );
    connect( m_obscuranceLoadPushButton, SIGNAL( clicked() ), SLOT( loadObscurance() ) );
    connect( m_obscuranceSavePushButton, SIGNAL( clicked() ), SLOT( saveObscurance() ) );

    // SMI
    connect( m_smiViewpointDistributionWidget, SIGNAL( numberOfViewpointsChanged(int) ), SLOT( setNumberOfSmiViewpoints(int) ) );
    connect( m_smiDefaultAxisCheckBox, SIGNAL( toggled(bool) ), m_smiViewpointDistributionWidget, SLOT( setDisabled(bool) ) );
    connect( m_smiDefaultAxisCheckBox, SIGNAL( toggled(bool) ), m_smiViewpointLabel, SLOT( setDisabled(bool) ) );
    connect( m_smiDefaultAxisCheckBox, SIGNAL( toggled(bool) ), m_smiViewpointSpinBox, SLOT( setDisabled(bool) ) );
    connect( m_smiPushButton, SIGNAL( clicked() ), SLOT( computeSmi() ) );
    connect( m_sliceUnstabilitiesPushButton, SIGNAL( clicked() ), SLOT( computeSliceUnstabilities() ) );
    connect( m_pmiPushButton, SIGNAL( clicked() ), SLOT( computePmi() ) );
    connect( m_propertySalienciesPushButton, SIGNAL( clicked() ), SLOT( computePropertySaliencies() ) );

    // VMI
    connect( m_vmiViewpointDistributionWidget, SIGNAL( numberOfViewpointsChanged(int) ), SLOT( setVmiOneViewpointMaximum(int) ) );
    connect( m_vmiOneViewpointCheckBox, SIGNAL( toggled(bool) ), m_vmiOneViewpointSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_saveViewedVolumePushButton, SIGNAL( clicked() ), SLOT( saveViewedVolume() ) );
    connect( m_loadViewpointEntropyPushButton, SIGNAL( clicked() ), SLOT( loadViewpointEntropy() ) );
    connect( m_saveViewpointEntropyPushButton, SIGNAL( clicked() ), SLOT( saveViewpointEntropy() ) );
    connect( m_loadEntropyPushButton, SIGNAL( clicked() ), SLOT( loadEntropy() ) );
    connect( m_saveEntropyPushButton, SIGNAL( clicked() ), SLOT( saveEntropy() ) );
    connect( m_loadVmiPushButton, SIGNAL( clicked() ), SLOT( loadVmi() ) );
    connect( m_saveVmiPushButton, SIGNAL( clicked() ), SLOT( saveVmi() ) );
    connect( m_loadMiPushButton, SIGNAL( clicked() ), SLOT( loadMi() ) );
    connect( m_saveMiPushButton, SIGNAL( clicked() ), SLOT( saveMi() ) );
    connect( m_loadViewpointUnstabilitiesPushButton, SIGNAL( clicked() ), SLOT( loadViewpointUnstabilities() ) );
    connect( m_saveViewpointUnstabilitiesPushButton, SIGNAL( clicked() ), SLOT( saveViewpointUnstabilities() ) );
    connect( m_loadVomiPushButton, SIGNAL( clicked() ), SLOT( loadVomi() ) );
    connect( m_saveVomiPushButton, SIGNAL( clicked() ), SLOT( saveVomi() ) );
    connect( m_loadViewpointVomiPushButton, SIGNAL( clicked() ), SLOT( loadViewpointVomi() ) );
    connect( m_saveViewpointVomiPushButton, SIGNAL( clicked() ), SLOT( saveViewpointVomi() ) );
    connect( m_loadColorVomiPalettePushButton, SIGNAL( clicked() ), SLOT( loadColorVomiPalette() ) );
    connect( m_loadColorVomiPushButton, SIGNAL( clicked() ), SLOT( loadColorVomi() ) );
    connect( m_saveColorVomiPushButton, SIGNAL( clicked() ), SLOT( saveColorVomi() ) );
    connect( m_computeEvmiOpacityUseOtherPushButton, SIGNAL( clicked() ), SLOT( loadEvmiOpacityOtherTransferFunction() ) );
    connect( m_loadEvmiOpacityPushButton, SIGNAL( clicked() ), SLOT( loadEvmiOpacity() ) );
    connect( m_saveEvmiOpacityPushButton, SIGNAL( clicked() ), SLOT( saveEvmiOpacity() ) );
    connect( m_loadEvmiVomiPushButton, SIGNAL( clicked() ), SLOT( loadEvmiVomi() ) );
    connect( m_saveEvmiVomiPushButton, SIGNAL( clicked() ), SLOT( saveEvmiVomi() ) );
    connect( m_computeBestViewsCheckBox, SIGNAL( toggled(bool) ), m_computeBestViewsNRadioButton, SLOT( setEnabled(bool) ) );
    connect( m_computeBestViewsCheckBox, SIGNAL( toggled(bool) ), m_computeBestViewsNSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_computeBestViewsCheckBox, SIGNAL( toggled(bool) ), m_computeBestViewsThresholdRadioButton, SLOT( setEnabled(bool) ) );
    connect( m_computeBestViewsCheckBox, SIGNAL( toggled(bool) ), m_computeBestViewsThresholdDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_loadBestViewsPushButton, SIGNAL( clicked() ), SLOT( loadBestViews() ) );
    connect( m_saveBestViewsPushButton, SIGNAL( clicked() ), SLOT( saveBestViews() ) );
    connect( m_loadGuidedTourPushButton, SIGNAL( clicked() ), SLOT( loadGuidedTour() ) );
    connect( m_saveGuidedTourPushButton, SIGNAL( clicked() ), SLOT( saveGuidedTour() ) );
    connect( m_computeExploratoryTourCheckBox, SIGNAL( toggled(bool) ), m_computeExploratoryTourThresholdLabel, SLOT( setEnabled(bool) ) );
    connect( m_computeExploratoryTourCheckBox, SIGNAL( toggled(bool) ), m_computeExploratoryTourThresholdDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_loadExploratoryTourPushButton, SIGNAL( clicked() ), SLOT( loadExploratoryTour() ) );
    connect( m_saveExploratoryTourPushButton, SIGNAL( clicked() ), SLOT( saveExploratoryTour() ) );
    connect( m_computeVmiPushButton, SIGNAL( clicked() ), SLOT( computeSelectedVmi() ) );
    connect( m_tourBestViewsPushButton, SIGNAL( clicked() ), SLOT( tourBestViews() ) );
    connect( m_guidedTourPushButton, SIGNAL( clicked() ), SLOT( guidedTour() ) );
    connect( m_exploratoryTourPushButton, SIGNAL( clicked() ), SLOT( exploratoryTour() ) );
    connect( m_vomiGradientPushButton, SIGNAL( clicked() ), SLOT( computeVomiGradient() ) );

    // VMIi
    connect( m_vmiiViewpointDistributionWidget, SIGNAL( numberOfViewpointsChanged(int) ), SLOT( setVmiiOneViewpointMaximum(int) ) );
    connect( m_vmiiOneViewpointCheckBox, SIGNAL( toggled(bool) ), m_vmiiOneViewpointSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_saveViewedVolumeIPushButton, SIGNAL( clicked() ), SLOT( saveViewedVolumeI() ) );
    connect( m_loadViewpointEntropyIPushButton, SIGNAL( clicked() ), SLOT( loadViewpointEntropyI() ) );
    connect( m_saveViewpointEntropyIPushButton, SIGNAL( clicked() ), SLOT( saveViewpointEntropyI() ) );
    connect( m_loadEntropyIPushButton, SIGNAL( clicked() ), SLOT( loadEntropyI() ) );
    connect( m_saveEntropyIPushButton, SIGNAL( clicked() ), SLOT( saveEntropyI() ) );
    connect( m_loadVmiiPushButton, SIGNAL( clicked() ), SLOT( loadVmii() ) );
    connect( m_saveVmiiPushButton, SIGNAL( clicked() ), SLOT( saveVmii() ) );
    connect( m_loadMiiPushButton, SIGNAL( clicked() ), SLOT( loadMii() ) );
    connect( m_saveMiiPushButton, SIGNAL( clicked() ), SLOT( saveMii() ) );
    connect( m_loadViewpointUnstabilitiesIPushButton, SIGNAL( clicked() ), SLOT( loadViewpointUnstabilitiesI() ) );
    connect( m_saveViewpointUnstabilitiesIPushButton, SIGNAL( clicked() ), SLOT( saveViewpointUnstabilitiesI() ) );
    connect( m_loadImiPushButton, SIGNAL( clicked() ), SLOT( loadImi() ) );
    connect( m_saveImiPushButton, SIGNAL( clicked() ), SLOT( saveImi() ) );
    connect( m_computeVmiiPushButton, SIGNAL( clicked() ), SLOT( computeSelectedVmii() ) );

    // Program
    connect( m_loadAndRunProgramPushButton, SIGNAL( clicked() ), SLOT( loadAndRunProgram() ) );

    // Filtering
    connect( m_filteringGaussianPushButton, SIGNAL( clicked() ), SLOT( gaussianFilter() ) );
    connect( m_filteringBoxMeanPushButton, SIGNAL( clicked() ), SLOT( boxMeanFilter() ) );
    connect( m_probabilisticAmbientOcclusionGaussianChebychevPushButton, SIGNAL( clicked() ), SLOT( probabilisticAmbientOcclusionGaussianChebychev() ) );
    connect( m_probabilisticAmbientOcclusionBoxMeanChebychevPushButton, SIGNAL( clicked() ), SLOT( probabilisticAmbientOcclusionBoxMeanChebychev() ) );
    connect( m_probabilisticAmbientOcclusionGaussianPushButton, SIGNAL( clicked() ), SLOT( probabilisticAmbientOcclusionGaussian() ) );
    connect( m_probabilisticAmbientOcclusionCubePushButton, SIGNAL( clicked() ), SLOT( probabilisticAmbientOcclusionCube() ) );
    connect( m_probabilisticAmbientOcclusionSpherePushButton, SIGNAL( clicked() ), SLOT( probabilisticAmbientOcclusionSphere() ) );
    connect( m_probabilisticAmbientOcclusionTangentSphereVariancePushButton, SIGNAL( clicked() ), SLOT( probabilisticAmbientOcclusionTangentSphereVariance() ) );
    connect( m_probabilisticAmbientOcclusionTangentSphereCdfPushButton, SIGNAL( clicked() ), SLOT( probabilisticAmbientOcclusionTangentSphereCdf() ) );
    connect( m_probabilisticAmbientOcclusionTangentSphereGaussianPushButton, SIGNAL( clicked() ), SLOT( probabilisticAmbientOcclusionTangentSphereGaussian() ) );
    connect( m_probabilisticAmbientOcclusionGradientPushButton, SIGNAL( clicked() ), SLOT( probabilisticAmbientOcclusionGradient() ) );
    connect( m_volumeVariancePushButton, SIGNAL( clicked() ), SLOT( volumeVariance() ) );
}


QString QExperimental3DExtension::getFileNameToLoad( const QString &settingsDirKey, const QString &caption, const QString &filter )
{
    Settings settings;

    QString dir = settings.getValue( settingsDirKey ).toString();
    QString fileName = QFileDialog::getOpenFileName( this, caption, dir, filter );

    if ( !fileName.isNull() )
    {
        QFileInfo fileInfo( fileName );
        settings.setValue( settingsDirKey, fileInfo.absolutePath() );
    }

    return fileName;
}


QString QExperimental3DExtension::getFileNameToSave( const QString &settingsDirKey, const QString &caption, const QString &filter, const QString &defaultSuffix )
{
    QString fileName;
    Settings settings;

    QString dir = settings.getValue( settingsDirKey ).toString();
    QFileDialog saveDialog( this, caption, dir, filter );
    saveDialog.setAcceptMode( QFileDialog::AcceptSave );
    saveDialog.setDefaultSuffix( defaultSuffix );

    if ( saveDialog.exec() == QDialog::Accepted )
    {
        fileName = saveDialog.selectedFiles().first();
        QFileInfo fileInfo( fileName );
        settings.setValue( settingsDirKey, fileInfo.absolutePath() );
    }

    return fileName;
}


void QExperimental3DExtension::loadTransferFunction()
{
    QString transferFunctionFileName = getFileNameToLoad( Experimental3DSettings::TransferFunctionDir, tr("Load transfer function"), tr("XML files (*.xml);;Transfer function files (*.tf);;All files (*)") );
    if ( !transferFunctionFileName.isNull() ) loadTransferFunction( transferFunctionFileName );
}


void QExperimental3DExtension::loadTransferFunction( const QString &fileName )
{
    TransferFunction *transferFunction;

    if ( fileName.endsWith( ".xml" ) ) transferFunction = TransferFunctionIO::fromXmlFile( fileName );
    else transferFunction = TransferFunctionIO::fromFile( fileName );

    m_transferFunctionEditor->setTransferFunction( *transferFunction );

    m_recentTransferFunctions << *transferFunction;
    int row = m_recentTransferFunctionsModel->rowCount();
    m_recentTransferFunctionsModel->insertRow( row );
    QString name = transferFunction->name();
    if ( name.isEmpty() ) name = tr("<unnamed>");
    m_recentTransferFunctionsModel->setData( m_recentTransferFunctionsModel->index( row, 0 ), name );

    delete transferFunction;

    setTransferFunction();
}


void QExperimental3DExtension::saveTransferFunction()
{
    QString transferFunctionFileName = getFileNameToSave( Experimental3DSettings::TransferFunctionDir, tr("Save transfer function"), tr("XML files (*.xml);;Transfer function files (*.tf);;All files (*)"), "xml" );

    if ( !transferFunctionFileName.isNull() ) saveTransferFunction( transferFunctionFileName );
}


void QExperimental3DExtension::saveTransferFunction( const QString &fileName )
{
    if ( fileName.endsWith( ".xml" ) ) TransferFunctionIO::toXmlFile( fileName, m_transferFunctionEditor->transferFunction() );
    else TransferFunctionIO::toFile( fileName, m_transferFunctionEditor->transferFunction() );

    m_recentTransferFunctions << m_transferFunctionEditor->transferFunction();
    int row = m_recentTransferFunctionsModel->rowCount();
    m_recentTransferFunctionsModel->insertRow( row );
    QString name = m_transferFunctionEditor->transferFunction().name();
    if ( name.isEmpty() ) name = tr("<unnamed>");
    m_recentTransferFunctionsModel->setData( m_recentTransferFunctionsModel->index( row, 0 ), name );
}


void QExperimental3DExtension::addRecentTransferFunction()
{
    m_recentTransferFunctions << m_transferFunctionEditor->transferFunction();
    int row = m_recentTransferFunctionsModel->rowCount();
    m_recentTransferFunctionsModel->insertRow( row );
    QString name = m_transferFunctionEditor->transferFunction().name();
    if ( name.isEmpty() ) name = tr("<unnamed>");
    m_recentTransferFunctionsModel->setData( m_recentTransferFunctionsModel->index( row, 0 ), name );
}


void QExperimental3DExtension::setRecentTransferFunction( const QModelIndex &index )
{
    m_transferFunctionEditor->setTransferFunction( m_recentTransferFunctions.at( index.row() ) );
    setTransferFunction();
}


void QExperimental3DExtension::setTransferFunction( bool render )
{
    m_volume->setTransferFunction( m_transferFunctionEditor->transferFunction() );
    if ( render ) m_viewer->render();
}


void QExperimental3DExtension::tour( const QList<Vector3> &viewpoints, double speedFactor )
{
    if ( viewpoints.isEmpty() ) return;

    uint frameCounter = 0;
    QString tourFileName = m_saveNextTourLineEdit->text();
    bool saveTour = m_saveNextTourCheckBox->isChecked();

    const double ALMOST_1 = 0.9;

    int *dimensions = m_volume->getImage()->GetDimensions();
    int maxDimension = qMax( qMax( dimensions[0], dimensions[1] ), dimensions[2] );
    double maxDistance = speedFactor * maxDimension / 4.0;

    DEBUG_LOG( "Tour:" );

    Vector3 previousPoint = viewpoints.at( 0 );
    DEBUG_LOG( previousPoint.toString() );

    Vector3 currentPoint = previousPoint;
    setViewpoint( currentPoint );
    if ( saveTour ) m_viewer->screenshot( tourFileName.arg( frameCounter++, 8, 10, QChar( '0' ) ) );

    for ( int i = 1; i < viewpoints.size(); i++ )
    {
        Vector3 nextPoint = viewpoints.at( i );
        double nextRadius = nextPoint.length();

        // Mirem si s'ha d'afegir un punt intermig
        {
            Vector3 c = currentPoint;
            c.normalize();
            Vector3 n = nextPoint;
            n.normalize();

            if ( c * n < -ALMOST_1 ) // la línia entre els punts passa pel centre del volum
            {
                DEBUG_LOG( QString( "punt intermig: c = %1, n = %2, c * n = %3" ).arg( c.toString() ).arg( n.toString() ).arg( c * n ) );
                // afegim un punt intermig (0,0,radi) o (radi,0,0)
                double radius = ( currentPoint.length() + nextRadius ) / 2.0;

                if ( qAbs( c * Vector3( 0.0, 0.0, 1.0 ) ) > ALMOST_1 ) nextPoint.set( radius, 0.0, 0.0 );
                else nextPoint.set( 0.0, 0.0, radius );

                i--;
            }
        }

        while ( currentPoint != nextPoint )
        {
            Vector3 direction = nextPoint - currentPoint;

            if ( direction.length() < maxDistance ) currentPoint = nextPoint;
            else
            {
                double currentRadius = currentPoint.length();
                Vector3 nextCurrentPoint;

                do
                {
                    direction.normalize() *= maxDistance;  // posem la direcció a la llargada desitjada
                    nextCurrentPoint = currentPoint + direction;

                    double currentToNextCurrent = ( nextCurrentPoint - currentPoint ).length();
                    double nextCurrentToNext = ( nextPoint - nextCurrentPoint ).length();
                    double a = currentToNextCurrent / ( currentToNextCurrent + nextCurrentToNext ); // valor per interpolar el radi
                    double nextCurrentRadius = a * currentRadius + ( 1.0 - a ) * nextRadius;

                    nextCurrentPoint.normalize() *= nextCurrentRadius;  // posem el nou punt a la distància correcta
                    direction = nextCurrentPoint - currentPoint;
                } while ( direction.length() <= maxDistance - 1.0 || direction.length() >= maxDistance + 1.0 );

                currentPoint = nextCurrentPoint;
            }

            setViewpoint( currentPoint );
            if ( saveTour ) m_viewer->screenshot( tourFileName.arg( frameCounter++, 8, 10, QChar( '0' ) ) );
        }

        DEBUG_LOG( nextPoint.toString() );
    }
}


void QExperimental3DExtension::chooseBackgroundColor()
{
    QColor color = QColorDialog::getColor( m_viewer->getBackgroundColor(), this );
    if ( color.isValid() ) m_viewer->setBackgroundColor( color );
}


void QExperimental3DExtension::enableSpecularLighting( bool on )
{
    m_baseSpecularLightingCheckBox->setEnabled( on );

    if ( on )
    {
        m_baseSpecularLightingPowerLabel->setEnabled( m_baseSpecularLightingCheckBox->isChecked() );
        m_baseSpecularLightingPowerDoubleSpinBox->setEnabled( m_baseSpecularLightingCheckBox->isChecked() );
    }
    else
    {
        m_baseSpecularLightingPowerLabel->setEnabled( false );
        m_baseSpecularLightingPowerDoubleSpinBox->setEnabled( false );
    }
}


float passIfNegative( float f )
{
    return f < 0.0f ? f : 0.0f;
}


void QExperimental3DExtension::render()
{
    m_volume->setInterpolation( static_cast<Experimental3DVolume::Interpolation>( m_interpolationComboBox->currentIndex() ) );
    m_volume->setGradientEstimator( static_cast<Experimental3DVolume::GradientEstimator>( m_gradientEstimatorComboBox->currentIndex() ) );

    m_volume->resetShadingOptions();

    if ( m_baseAmbientLightingRadioButton->isChecked() ) m_volume->addLighting();
    else if ( m_baseDiffuseLightingRadioButton->isChecked() )
    {
        m_viewer->updateShadingTable();
        m_volume->addLighting( true, m_baseSpecularLightingCheckBox->isChecked(), m_baseSpecularLightingPowerDoubleSpinBox->value() );
    }
    else if ( m_baseCoolWarmRadioButton->isChecked() ) m_volume->addCoolWarm( m_baseCoolWarmBDoubleSpinBox->value(), m_baseCoolWarmYDoubleSpinBox->value(), m_baseCoolWarmAlphaDoubleSpinBox->value(),
                                                                              m_baseCoolWarmBetaDoubleSpinBox->value() );
    else if ( m_baseObscuranceRadioButton->isChecked() ) m_volume->addObscurance( m_obscurance, m_baseObscuranceFactorDoubleSpinBox->value(), m_baseObscuranceLowFilterDoubleSpinBox->value(),
                                                                                  m_baseObscuranceHighFilterDoubleSpinBox->value() );
    else if ( m_baseVomiRadioButton->isChecked() ) m_volume->addVomi( m_vomi, m_maximumVomi, m_baseVomiFactorDoubleSpinBox->value() );
    //else if ( m_baseVomiRadioButton->isChecked() ) m_volume->addVoxelSaliencies( m_vomi, m_maximumVomi, m_baseVomiFactorDoubleSpinBox->value() );
    else if ( m_baseVomiCoolWarmRadioButton->isChecked() ) m_volume->addVomiCoolWarm( m_vomi, m_maximumVomi, m_baseVomiCoolWarmFactorDoubleSpinBox->value(),
                                                                                      m_baseVomiCoolWarmYDoubleSpinBox->value(), m_baseVomiCoolWarmBDoubleSpinBox->value() );
    else if ( m_baseColorVomiRadioButton->isChecked() ) m_volume->addColorVomi( m_colorVomi, m_maximumColorVomi, m_baseColorVomiFactorDoubleSpinBox->value() );
    else if ( m_baseImiRadioButton->isChecked() ) m_volume->addImi( m_imi, m_maximumImi, m_baseImiFactorDoubleSpinBox->value() );
    else if ( m_baseVoxelSalienciesRadioButton->isChecked() ) m_volume->addVoxelSaliencies( m_voxelSaliencies, m_maximumSaliency, m_baseVoxelSalienciesFactorDoubleSpinBox->value() );
    //else if ( m_baseVoxelSalienciesRadioButton->isChecked() ) m_volume->addVomi( m_voxelSaliencies, m_maximumSaliency, m_baseVoxelSalienciesFactorDoubleSpinBox->value() );
    else if ( m_baseFilteringAmbientOcclusionRadioButton->isChecked() )
    {
        switch ( m_baseFilteringAmbientOcclusionTypeComboBox->currentIndex() )
        {
            case 0: // direct
                m_volume->addFilteringAmbientOcclusionMap( m_spatialImportanceFunction, m_maximumSpatialImportanceFunction, m_baseFilteringAmbientOcclusionFactorDoubleSpinBox->value() );
                break;
            case 1: // absolute
                {
                    QVector<float> absFiltering = QtConcurrent::blockingMapped( m_spatialImportanceFunction, qAbs<float> );
                    m_volume->addFilteringAmbientOcclusionMap( absFiltering, m_maximumSpatialImportanceFunction, m_baseFilteringAmbientOcclusionFactorDoubleSpinBox->value() );
                }
                break;
            case 2: // negatives
                {
                    QVector<float> negativeFiltering = QtConcurrent::blockingMapped( m_spatialImportanceFunction, passIfNegative );
                    m_volume->addFilteringAmbientOcclusionMap( negativeFiltering, m_maximumSpatialImportanceFunction, m_baseFilteringAmbientOcclusionFactorDoubleSpinBox->value() );
                }
                break;
        }
    }
    else if ( m_baseProbabilisticAmbientOcclusionRadioButton->isChecked() )
        m_volume->addVomiGamma( m_probabilisticAmbientOcclusion, 1.0f, m_baseProbabilisticAmbientOcclusionFactorDoubleSpinBox->value(), m_baseProbabilisticAmbientOcclusionGammaDoubleSpinBox->value() );

    if ( m_contourCheckBox->isChecked() ) m_volume->addContour( m_contourDoubleSpinBox->value() );
    if ( m_obscuranceCheckBox->isChecked() ) m_volume->addObscurance( m_obscurance, m_obscuranceFactorDoubleSpinBox->value(), m_obscuranceLowFilterDoubleSpinBox->value(), m_obscuranceHighFilterDoubleSpinBox->value(),
                                                                      m_additiveObscuranceVomiCheckBox->isChecked(), m_additiveObscuranceVomiWeightDoubleSpinBox->value() );
    if ( m_vomiCheckBox->isChecked() ) m_volume->addVomi( m_vomi, m_maximumVomi, m_vomiFactorDoubleSpinBox->value(), m_additiveObscuranceVomiCheckBox->isChecked(),
                                                          m_additiveObscuranceVomiWeightDoubleSpinBox->value() );
    if ( m_vomiCoolWarmCheckBox->isChecked() ) m_volume->addVomiCoolWarm( m_vomi, m_maximumVomi, m_vomiCoolWarmFactorDoubleSpinBox->value(),
                                                                          m_vomiCoolWarmYDoubleSpinBox->value(), m_vomiCoolWarmBDoubleSpinBox->value() );
    if ( m_colorVomiCheckBox->isChecked() ) m_volume->addColorVomi( m_colorVomi, m_maximumColorVomi, m_colorVomiFactorDoubleSpinBox->value() );
    if ( m_opacityVomiCheckBox->isChecked() ) m_volume->addOpacity( m_vomi, m_maximumVomi, m_opacityLowThresholdDoubleSpinBox->value(), m_opacityLowFactorDoubleSpinBox->value(),
                                                                                           m_opacityHighThresholdDoubleSpinBox->value(), m_opacityHighFactorDoubleSpinBox->value() );
    if ( m_opacitySaliencyCheckBox->isChecked() ) m_volume->addOpacity( m_voxelSaliencies, m_maximumSaliency, m_opacityLowThresholdDoubleSpinBox->value(), m_opacityLowFactorDoubleSpinBox->value(),
                                                                                                              m_opacityHighThresholdDoubleSpinBox->value(), m_opacityHighFactorDoubleSpinBox->value() );
    if ( m_opacityFilteringCheckBox->isChecked() )
    {
        QVector<float> absFiltering = QtConcurrent::blockingMapped( m_spatialImportanceFunction, qAbs<float> );
        m_volume->addOpacity( absFiltering, m_maximumSpatialImportanceFunction, m_opacityLowThresholdDoubleSpinBox->value(), m_opacityLowFactorDoubleSpinBox->value(),
                                                                                m_opacityHighThresholdDoubleSpinBox->value(), m_opacityHighFactorDoubleSpinBox->value() );
    }
    if ( m_opacityProbabilisticAmbientOcclusionCheckBox->isChecked() )
        m_volume->addOpacity( m_probabilisticAmbientOcclusion, 1.0f, m_opacityLowThresholdDoubleSpinBox->value(), m_opacityLowFactorDoubleSpinBox->value(),
                                                                     m_opacityHighThresholdDoubleSpinBox->value(), m_opacityHighFactorDoubleSpinBox->value() );
    if ( m_filteringAmbientOcclusionCheckBox->isChecked() )
    {
        switch ( m_filteringAmbientOcclusionTypeComboBox->currentIndex() )
        {
            case 0: // direct
                m_volume->addFilteringAmbientOcclusion( m_spatialImportanceFunction, m_maximumSpatialImportanceFunction, m_filteringAmbientOcclusionLambdaDoubleSpinBox->value() );
                break;
            case 1: // absolute
                {
                    QVector<float> absFiltering = QtConcurrent::blockingMapped( m_spatialImportanceFunction, qAbs<float> );
                    m_volume->addFilteringAmbientOcclusion( absFiltering, m_maximumSpatialImportanceFunction, m_filteringAmbientOcclusionLambdaDoubleSpinBox->value() );
                }
                break;
            case 2: // negatives
                {
                    QVector<float> negativeFiltering = QtConcurrent::blockingMapped( m_spatialImportanceFunction, passIfNegative );
                    m_volume->addFilteringAmbientOcclusion( negativeFiltering, m_maximumSpatialImportanceFunction, m_filteringAmbientOcclusionLambdaDoubleSpinBox->value() );
                }
                break;
        }
    }
    if ( m_probabilisticAmbientOcclusionCheckBox->isChecked() )
        m_volume->addVomiGamma( m_probabilisticAmbientOcclusion, 1.0f, m_probabilisticAmbientOcclusionFactorDoubleSpinBox->value(), m_probabilisticAmbientOcclusionGammaDoubleSpinBox->value(),
                                m_additiveObscuranceVomiCheckBox->isChecked(), m_additiveObscuranceVomiWeightDoubleSpinBox->value() );
    if ( m_opacityVarianceCheckBox->isChecked() ) m_volume->addOpacity( m_volumeVariance, m_opacityVarianceMaxDoubleSpinBox->value() );
    if ( m_celShadingCheckBox->isChecked() ) m_volume->addCelShading( m_celShadingQuantumsSpinBox->value() );

    m_viewer->render();
}


void QExperimental3DExtension::getCamera()
{
    Vector3 position, focus, up;

    m_viewer->getCamera( position, focus, up );

    m_cameraPositionXDoubleSpinBox->setValue( position.x );
    m_cameraPositionYDoubleSpinBox->setValue( position.y );
    m_cameraPositionZDoubleSpinBox->setValue( position.z );

    m_cameraFocusXDoubleSpinBox->setValue( focus.x );
    m_cameraFocusYDoubleSpinBox->setValue( focus.y );
    m_cameraFocusZDoubleSpinBox->setValue( focus.z );

    m_cameraUpXDoubleSpinBox->setValue( up.x );
    m_cameraUpYDoubleSpinBox->setValue( up.y );
    m_cameraUpZDoubleSpinBox->setValue( up.z );
}


void QExperimental3DExtension::setCamera()
{
    Vector3 position, focus, up;

    position.x = m_cameraPositionXDoubleSpinBox->value();
    position.y = m_cameraPositionYDoubleSpinBox->value();
    position.z = m_cameraPositionZDoubleSpinBox->value();

    focus.x = m_cameraFocusXDoubleSpinBox->value();
    focus.y = m_cameraFocusYDoubleSpinBox->value();
    focus.z = m_cameraFocusZDoubleSpinBox->value();

    up.x = m_cameraUpXDoubleSpinBox->value();
    up.y = m_cameraUpYDoubleSpinBox->value();
    up.z = m_cameraUpZDoubleSpinBox->value();

    m_viewer->setCamera( position, focus, up );
}


void QExperimental3DExtension::loadCamera()
{
    QString cameraFileName = getFileNameToLoad( Experimental3DSettings::CameraDir, tr("Load camera parameters"), tr("Camera files (*.cam);;All files (*)") );
    if ( !cameraFileName.isNull() ) loadCamera( cameraFileName );
}


void QExperimental3DExtension::loadCamera( const QString &fileName )
{
    QFile cameraFile( fileName );

    if ( !cameraFile.open( QFile::ReadOnly | QFile::Text ) )
    {
        DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + fileName );
        ERROR_LOG( QString( "No es pot llegir el fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't load"), QString( tr("Can't load from file ") ) + fileName );
        return;
    }

    QTextStream in( &cameraFile );

    Vector3 position, focus, up;

    if ( !in.atEnd() ) in >> position.x;
    if ( !in.atEnd() ) in >> position.y;
    if ( !in.atEnd() ) in >> position.z;

    if ( !in.atEnd() ) in >> focus.x;
    if ( !in.atEnd() ) in >> focus.y;
    if ( !in.atEnd() ) in >> focus.z;

    if ( !in.atEnd() ) in >> up.x;
    if ( !in.atEnd() ) in >> up.y;
    if ( !in.atEnd() ) in >> up.z;

    m_cameraPositionXDoubleSpinBox->setValue( position.x );
    m_cameraPositionYDoubleSpinBox->setValue( position.y );
    m_cameraPositionZDoubleSpinBox->setValue( position.z );

    m_cameraFocusXDoubleSpinBox->setValue( focus.x );
    m_cameraFocusYDoubleSpinBox->setValue( focus.y );
    m_cameraFocusZDoubleSpinBox->setValue( focus.z );

    m_cameraUpXDoubleSpinBox->setValue( up.x );
    m_cameraUpYDoubleSpinBox->setValue( up.y );
    m_cameraUpZDoubleSpinBox->setValue( up.z );

    cameraFile.close();

    setCamera();
}


void QExperimental3DExtension::saveCamera()
{
    QString cameraFileName = getFileNameToSave( Experimental3DSettings::CameraDir, tr("Save camera parameters"), tr("Camera files (*.cam);;All files (*)"), "cam" );

    if ( !cameraFileName.isNull() )
    {
        QFile cameraFile( cameraFileName );

        if ( !cameraFile.open( QFile::WriteOnly | QFile::Truncate | QFile::Text ) )
        {
            ERROR_LOG( QString( "No es pot escriure al fitxer " ) + cameraFileName );
            if ( m_interactive ) QMessageBox::warning( this, tr("Can't save"), QString( tr("Can't save to file ") ) + cameraFileName );
            return;
        }

        QTextStream out( &cameraFile );

        out << m_cameraPositionXDoubleSpinBox->value() << "\n";
        out << m_cameraPositionYDoubleSpinBox->value() << "\n";
        out << m_cameraPositionZDoubleSpinBox->value() << "\n";

        out << m_cameraFocusXDoubleSpinBox->value() << "\n";
        out << m_cameraFocusYDoubleSpinBox->value() << "\n";
        out << m_cameraFocusZDoubleSpinBox->value() << "\n";

        out << m_cameraUpXDoubleSpinBox->value() << "\n";
        out << m_cameraUpYDoubleSpinBox->value() << "\n";
        out << m_cameraUpZDoubleSpinBox->value() << "\n";

        out.flush();
        cameraFile.close();
    }
}


void QExperimental3DExtension::setNumberOfViewpoints( int numberOfViewpoints )
{
    m_viewpointSpinBox->setMaximum( numberOfViewpoints );
}


void QExperimental3DExtension::setViewpoint()
{
    Vector3 position, focus, up;
    m_viewer->getCamera( position, focus, up );

    float distance = ( position - focus ).length();

    ViewpointGenerator viewpointGenerator;

    if ( m_cameraViewpointDistributionWidget->isUniform() )
    {
        switch ( m_cameraViewpointDistributionWidget->numberOfViewpoints() )
        {
            case 4: viewpointGenerator.setToUniform4( distance ); break;
            case 6: viewpointGenerator.setToUniform6( distance ); break;
            case 8: viewpointGenerator.setToUniform8( distance ); break;
            case 12: viewpointGenerator.setToUniform12( distance ); break;
            case 20: viewpointGenerator.setToUniform20( distance ); break;
            default: Q_ASSERT_X( false, "setViewpoint", qPrintable( QString( "Nombre de punts de vista uniformes incorrecte: %1" ).arg( m_cameraViewpointDistributionWidget->numberOfViewpoints() ) ) );
        }
    }
    else viewpointGenerator.setToQuasiUniform( m_cameraViewpointDistributionWidget->recursionLevel(), distance );

    setViewpoint( viewpointGenerator.viewpoint( m_viewpointSpinBox->value() - 1 ) );
}


void QExperimental3DExtension::setViewpoint( const Vector3 &viewpoint )
{
    m_viewer->setCamera( viewpoint, Vector3(), ViewpointGenerator::up( viewpoint ) );
}


void QExperimental3DExtension::tour()
{
    Vector3 position, focus, up;
    m_viewer->getCamera( position, focus, up );
    float distance = ( position - focus ).length();
    ViewpointGenerator viewpointGenerator = m_cameraViewpointDistributionWidget->viewpointGenerator( distance );
    QVector<Vector3> viewpoints = viewpointGenerator.viewpoints();

    QStringList indices = m_tourLineEdit->text().split( ',' );
    QList<Vector3> tourViewpoints;

    for ( int i = 0; i < indices.size(); i++ )
    {
        int index = indices.at( i ).toInt() - 1;
        if ( index >= 0 && index < viewpoints.size() ) tourViewpoints << viewpoints.at( index );
    }

    tour( tourViewpoints, m_tourSpeedDoubleSpinBox->value() );
}


void QExperimental3DExtension::computeCancelObscurance()
{
    if ( !m_computingObscurance )
    {
        m_computingObscurance = true;

        bool render = false;

        if ( m_baseObscuranceRadioButton->isChecked() )
        {
            m_baseAmbientLightingRadioButton->setChecked( true );
            render = true;
        }

        if ( m_obscuranceCheckBox->isChecked() )
        {
            m_obscuranceCheckBox->setChecked( false );
            render = true;
        }

        if ( render ) this->render();

        m_baseObscuranceRadioButton->setEnabled( false );
        m_obscuranceCheckBox->setEnabled( false );

        delete m_obscuranceMainThread;          // esborrem el thread d'abans
        delete m_obscurance; m_obscurance = 0;  // esborrem l'obscurança d'abans

        int numberOfDirections;
        if ( m_obscuranceViewpointDistributionWidget->isUniform() )
            numberOfDirections = -1 * m_obscuranceViewpointDistributionWidget->numberOfViewpoints();
        else
            numberOfDirections = m_obscuranceViewpointDistributionWidget->recursionLevel();

        m_obscuranceMainThread = new ObscuranceMainThread( numberOfDirections,
                                                           m_obscuranceMaximumDistanceDoubleSpinBox->value(),
                                                           static_cast<ObscuranceMainThread::Function>( m_obscuranceFunctionComboBox->currentIndex() ),
                                                           static_cast<ObscuranceMainThread::Variant>( m_obscuranceVariantComboBox->currentIndex() ),
                                                           m_obscuranceDoublePrecisionRadioButton->isChecked(),
                                                           this );
        m_obscuranceMainThread->setVolume( m_volume->getVolume() );
        m_obscuranceMainThread->setTransferFunction( m_transferFunctionEditor->transferFunction() );

        m_obscurancePushButton->setText( tr("Cancel obscurance") );
        m_obscuranceProgressBar->setValue( 0 );
        connect( m_obscuranceMainThread, SIGNAL( progress(int) ), m_obscuranceProgressBar, SLOT( setValue(int) ) );
        connect( m_obscuranceMainThread, SIGNAL( computed() ), SLOT( endComputeObscurance() ) );
        m_obscuranceLoadPushButton->setEnabled( false );
        m_obscuranceSavePushButton->setEnabled( false );

        m_obscuranceMainThread->start();
    }
    else
    {
        m_obscuranceMainThread->stop();
        connect( m_obscuranceMainThread, SIGNAL( finished() ), SLOT( endCancelObscurance() ) );

        m_obscurancePushButton->setText( tr("Cancelling obscurance...") );
        m_obscurancePushButton->setEnabled( false );
    }
}


void QExperimental3DExtension::endComputeObscurance()
{
    m_computingObscurance = false;

    m_obscurance = m_obscuranceMainThread->getObscurance();
    m_obscurancePushButton->setText( tr("Compute obscurance") );
    m_obscuranceLoadPushButton->setEnabled( true );
    m_obscuranceSavePushButton->setEnabled( true );
    m_baseObscuranceRadioButton->setEnabled( true );
    m_obscuranceCheckBox->setEnabled( true );
}


void QExperimental3DExtension::endCancelObscurance()
{
    m_computingObscurance = false;

    m_obscurancePushButton->setText( tr("Compute obscurance") );
    m_obscurancePushButton->setEnabled( true );
    m_obscuranceLoadPushButton->setEnabled( true );
}


void QExperimental3DExtension::loadObscurance()
{
    QString obscuranceFileName = getFileNameToLoad( Experimental3DSettings::ObscuranceDir, tr("Load obscurance"), tr("Data files (*.dat);;All files (*)") );

    if ( !obscuranceFileName.isNull() )
    {
        bool render = false;

        if ( m_baseObscuranceRadioButton->isChecked() )
        {
            m_baseAmbientLightingRadioButton->setChecked( true );
            render = true;
        }

        if ( m_obscuranceCheckBox->isChecked() )
        {
            m_obscuranceCheckBox->setChecked( false );
            render = true;
        }

        if ( render ) this->render();

        m_baseObscuranceRadioButton->setEnabled( false );
        m_obscuranceCheckBox->setEnabled( false );

        delete m_obscurance;

        m_obscurance = new Obscurance( m_volume->getSize(), ObscuranceMainThread::hasColor( static_cast<ObscuranceMainThread::Variant>( m_obscuranceVariantComboBox->currentIndex() ) ),
                                       m_obscuranceDoublePrecisionRadioButton->isChecked() );
        bool ok = m_obscurance->load( obscuranceFileName );

        if ( ok )
        {
            m_obscuranceSavePushButton->setEnabled( true );
            m_obscuranceCheckBox->setEnabled( true );
            m_baseObscuranceRadioButton->setEnabled( true );
        }
        else
        {
            m_obscuranceSavePushButton->setEnabled( false );
            if ( m_interactive ) QMessageBox::warning( this, tr("Can't load obscurance"), QString( tr("Can't load obscurance from file ") ) + obscuranceFileName );
        }
    }
}


void QExperimental3DExtension::saveObscurance()
{
    QString obscuranceFileName = getFileNameToSave( Experimental3DSettings::ObscuranceDir, tr("Save obscurance"), tr("Data files (*.dat);;All files (*)"), "dat" );

    if ( !obscuranceFileName.isNull() )
    {
        if ( !m_obscurance->save( obscuranceFileName ) )
        {
            if ( m_interactive ) QMessageBox::warning( this, tr("Can't save obscurance"), QString( tr("Can't save obscurance to file ") ) + obscuranceFileName );
        }
    }
}


void QExperimental3DExtension::setNumberOfSmiViewpoints( int numberOfViewpoints )
{
    m_smiViewpointSpinBox->setMaximum( numberOfViewpoints );
}


void QExperimental3DExtension::computeSmi()
{
    if ( m_smiDefaultAxisCheckBox->isChecked() )
    {
        VolumeReslicer volumeReslicer;
        volumeReslicer.setInput( m_volume->getImage() );
        volumeReslicer.noReslice();
        volumeReslicer.computeSmi();
    }
    else
    {
        QVector<Vector3> viewpoints = m_smiViewpointDistributionWidget->viewpoints();
        int i0, i1;

        if ( m_smiViewpointSpinBox->value() == 0 )  // tots
        {
            i0 = 0; i1 = viewpoints.size();
        }
        else    // un en concret
        {
            i0 = m_smiViewpointSpinBox->value() - 1; i1 = i0 + 1;
        }

        for ( int i = i0; i < i1; i++ )
        {
            const Vector3 &viewpoint = viewpoints.at( i );

            VolumeReslicer volumeReslicer( i + 1 );
            volumeReslicer.setInput( m_volume->getImage() );

            Vector3 position( viewpoint );
            Vector3 up( 0.0, 1.0, 0.0 );
            if ( qAbs( position.normalize() * up ) > 0.9 ) up = Vector3( 0.0, 0.0, 1.0 );
            volumeReslicer.setViewpoint( viewpoint, up );

            volumeReslicer.setSpacing( 1.0, 1.0, 1.0 );
            volumeReslicer.reslice();

            volumeReslicer.computeSmi();
        }
    }
}


void QExperimental3DExtension::computeSliceUnstabilities()
{
    if ( m_smiDefaultAxisCheckBox->isChecked() )
    {
        VolumeReslicer volumeReslicer;
        volumeReslicer.setInput( m_volume->getImage() );
        volumeReslicer.noReslice();
        volumeReslicer.computeSliceUnstabilities();
    }
    else
    {
        QVector<Vector3> viewpoints = m_smiViewpointDistributionWidget->viewpoints();
        int i0, i1;

        if ( m_smiViewpointSpinBox->value() == 0 )  // tots
        {
            i0 = 0; i1 = viewpoints.size();
        }
        else    // un en concret
        {
            i0 = m_smiViewpointSpinBox->value() - 1; i1 = i0 + 1;
        }

        for ( int i = i0; i < i1; i++ )
        {
            const Vector3 &viewpoint = viewpoints.at( i );

            VolumeReslicer volumeReslicer( i + 1 );
            volumeReslicer.setInput( m_volume->getImage() );

            Vector3 position( viewpoint );
            Vector3 up( 0.0, 1.0, 0.0 );
            if ( qAbs( position.normalize() * up ) > 0.9 ) up = Vector3( 0.0, 0.0, 1.0 );
            volumeReslicer.setViewpoint( viewpoint, up );

            volumeReslicer.setSpacing( 1.0, 1.0, 1.0 );
            volumeReslicer.reslice();

            volumeReslicer.computeSliceUnstabilities();
        }
    }
}


void QExperimental3DExtension::computePmi()
{
    if ( m_smiDefaultAxisCheckBox->isChecked() )
    {
        VolumeReslicer volumeReslicer;
        volumeReslicer.setInput( m_volume->getImage() );
        volumeReslicer.noReslice();
        volumeReslicer.computePmi();
    }
    else
    {
        QVector<Vector3> viewpoints = m_smiViewpointDistributionWidget->viewpoints();
        int i0, i1;

        if ( m_smiViewpointSpinBox->value() == 0 )  // tots
        {
            i0 = 0; i1 = viewpoints.size();
        }
        else    // un en concret
        {
            i0 = m_smiViewpointSpinBox->value() - 1; i1 = i0 + 1;
        }

        for ( int i = i0; i < i1; i++ )
        {
            const Vector3 &viewpoint = viewpoints.at( i );

            VolumeReslicer volumeReslicer( i + 1 );
            volumeReslicer.setInput( m_volume->getImage() );

            Vector3 position( viewpoint );
            Vector3 up( 0.0, 1.0, 0.0 );
            if ( qAbs( position.normalize() * up ) > 0.9 ) up = Vector3( 0.0, 0.0, 1.0 );
            volumeReslicer.setViewpoint( viewpoint, up );

            volumeReslicer.setSpacing( 1.0, 1.0, 1.0 );
            volumeReslicer.reslice();

            volumeReslicer.computePmi();
        }
    }
}


void QExperimental3DExtension::computePropertySaliencies()
{
    if ( m_smiDefaultAxisCheckBox->isChecked() )
    {
        VolumeReslicer volumeReslicer;
        volumeReslicer.setInput( m_volume->getImage() );
        volumeReslicer.noReslice();
        volumeReslicer.computePropertySaliencies();
    }
    else
    {
        QVector<Vector3> viewpoints = m_smiViewpointDistributionWidget->viewpoints();
        int i0, i1;

        if ( m_smiViewpointSpinBox->value() == 0 )  // tots
        {
            i0 = 0; i1 = viewpoints.size();
        }
        else    // un en concret
        {
            i0 = m_smiViewpointSpinBox->value() - 1; i1 = i0 + 1;
        }

        for ( int i = i0; i < i1; i++ )
        {
            const Vector3 &viewpoint = viewpoints.at( i );

            VolumeReslicer volumeReslicer( i + 1 );
            volumeReslicer.setInput( m_volume->getImage() );

            Vector3 position( viewpoint );
            Vector3 up( 0.0, 1.0, 0.0 );
            if ( qAbs( position.normalize() * up ) > 0.9 ) up = Vector3( 0.0, 0.0, 1.0 );
            volumeReslicer.setViewpoint( viewpoint, up );

            volumeReslicer.setSpacing( 1.0, 1.0, 1.0 );
            volumeReslicer.reslice();

            volumeReslicer.computePropertySaliencies();
        }
    }
}


void QExperimental3DExtension::computeSelectedVmi()
{
    // Què ha demanat l'usuari
    bool computeViewpointEntropy = m_computeViewpointEntropyCheckBox->isChecked();
    bool computeEntropy = m_computeEntropyCheckBox->isChecked();
    bool computeVmi = m_computeVmiCheckBox->isChecked();
    bool computeMi = m_computeMiCheckBox->isChecked();
    bool computeViewpointUnstabilities = m_computeViewpointUnstabilitiesCheckBox->isChecked();
    bool computeVomi = m_computeVomiCheckBox->isChecked();
    bool computeViewpointVomi = m_computeViewpointVomiCheckBox->isChecked();
    bool computeColorVomi = m_computeColorVomiCheckBox->isChecked();
    bool computeEvmiOpacity = m_computeEvmiOpacityCheckBox->isChecked();
    bool computeEvmiVomi = m_computeEvmiVomiCheckBox->isChecked();
    bool computeBestViews = m_computeBestViewsCheckBox->isChecked();
    bool computeGuidedTour = m_computeGuidedTourCheckBox->isChecked();
    bool computeExploratoryTour = m_computeExploratoryTourCheckBox->isChecked();

    // Si no hi ha res a calcular marxem
    if ( !computeViewpointEntropy && !computeEntropy && !computeVmi && !computeMi && !computeViewpointUnstabilities && !computeVomi && !computeViewpointVomi && !computeColorVomi && !computeEvmiOpacity
         && !computeEvmiVomi && !computeBestViews && !computeGuidedTour && !computeExploratoryTour ) return;

    setCursor( QCursor( Qt::WaitCursor ) );

    // Obtenir direccions
    Vector3 position, focus, up;
    m_viewer->getCamera( position, focus, up );
    float distance = ( position - focus ).length();
    ViewpointGenerator viewpointGenerator = m_vmiViewpointDistributionWidget->viewpointGenerator( distance );

    // Viewpoint Information Channel
    ViewpointInformationChannel viewpointInformationChannel( viewpointGenerator, m_volume, m_viewer, m_transferFunctionEditor->transferFunction() );

    // Paleta de colors per la color VoMI
    if ( computeColorVomi ) viewpointInformationChannel.setColorVomiPalette( m_colorVomiPalette );

    // Funció de transferència per l'EVMI amb opacitat
    if ( computeEvmiOpacity )
    {
        if ( m_computeEvmiOpacityUseOtherPushButton->isChecked() ) viewpointInformationChannel.setEvmiOpacityTransferFunction( m_evmiOpacityTransferFunction );
        else viewpointInformationChannel.setEvmiOpacityTransferFunction( m_transferFunctionEditor->transferFunction() );
    }

    // Paràmetres extres per calcular les millors vistes (els passem sempre perquè tinguin algun valor, per si s'ha de calcular el guided tour per exemple)
    viewpointInformationChannel.setBestViewsParameters( m_computeBestViewsNRadioButton->isChecked(), m_computeBestViewsNSpinBox->value(), m_computeBestViewsThresholdDoubleSpinBox->value() );

    // Llindar per calcular l'exploratory tour
    viewpointInformationChannel.setExploratoryTourThreshold( m_computeExploratoryTourThresholdDoubleSpinBox->value() );

    // Filtratge de punts de vista
    if ( m_vmiOneViewpointCheckBox->isChecked() )
    {
        int nViewpoints = m_vmiViewpointDistributionWidget->numberOfViewpoints();
        int selectedViewpoint = m_vmiOneViewpointSpinBox->value() - 1;

        QVector<bool> filter( nViewpoints );

        filter[selectedViewpoint] = true;

        QVector<int> neighbours = viewpointGenerator.neighbours( selectedViewpoint );
        for ( int i = 0; i < neighbours.size(); i++ ) filter[neighbours.at( i )] = true;

        viewpointInformationChannel.filterViewpoints( filter );
    }

    connect( &viewpointInformationChannel, SIGNAL( totalProgressMaximum(int) ), m_vmiTotalProgressBar, SLOT( setMaximum(int) ) );
    connect( &viewpointInformationChannel, SIGNAL( totalProgressMaximum(int) ), m_vmiTotalProgressBar, SLOT( repaint() ) ); // no sé per què però cal això perquè s'actualitzi quan toca
    connect( &viewpointInformationChannel, SIGNAL( totalProgress(int) ), m_vmiTotalProgressBar, SLOT( setValue(int) ) );
    connect( &viewpointInformationChannel, SIGNAL( partialProgress(int) ), m_vmiProgressBar, SLOT( setValue(int) ) );

    QTime time;
    time.start();
    viewpointInformationChannel.compute( computeViewpointEntropy, computeEntropy, computeVmi, computeMi, computeViewpointUnstabilities, computeVomi, computeViewpointVomi, computeColorVomi, computeEvmiOpacity,
                                         computeEvmiVomi, computeBestViews, computeGuidedTour, computeExploratoryTour, m_vmiDisplayCheckBox->isChecked() );
    int elapsed = time.elapsed();
    DEBUG_LOG( QString( "Temps total de VOMI i altres: %1 s" ).arg( elapsed / 1000.0f ) );
    INFO_LOG( QString( "Temps total de VOMI i altres: %1 s" ).arg( elapsed / 1000.0f ) );

    if ( viewpointInformationChannel.hasViewedVolume() )
    {
        m_viewedVolume = viewpointInformationChannel.viewedVolume();
        m_saveViewedVolumePushButton->setEnabled( true );
    }

    if ( computeViewpointEntropy )
    {
        m_viewpointEntropy = viewpointInformationChannel.viewpointEntropy();
        m_saveViewpointEntropyPushButton->setEnabled( true );
    }

    if ( computeEntropy )
    {
        m_entropy = viewpointInformationChannel.entropy();
        m_saveEntropyPushButton->setEnabled( true );
    }

    if ( computeVmi )
    {
        m_vmi = viewpointInformationChannel.vmi();
        m_saveVmiPushButton->setEnabled( true );
    }

    if ( computeMi )
    {
        m_mi = viewpointInformationChannel.mi();
        m_saveMiPushButton->setEnabled( true );
    }

    if ( computeViewpointUnstabilities )
    {
        m_viewpointUnstabilities = viewpointInformationChannel.viewpointUnstabilities();
        m_saveViewpointUnstabilitiesPushButton->setEnabled( true );
    }

    if ( computeVomi )
    {
        m_vomi = viewpointInformationChannel.vomi();
        m_maximumVomi = viewpointInformationChannel.maximumVomi();
        m_baseVomiRadioButton->setEnabled( true );
        m_baseVomiCoolWarmRadioButton->setEnabled( true );
        m_vomiCheckBox->setEnabled( true );
        m_vomiCoolWarmCheckBox->setEnabled( true );
        m_opacityLabel->setEnabled( true );
        m_opacityVomiCheckBox->setEnabled( true );
        m_saveVomiPushButton->setEnabled( true );
        m_vomiGradientPushButton->setEnabled( true );
    }

    if ( computeViewpointVomi )
    {
        m_viewpointVomi = viewpointInformationChannel.viewpointVomi();
        m_saveViewpointVomiPushButton->setEnabled( true );
    }

    if ( computeColorVomi )
    {
        m_colorVomi = viewpointInformationChannel.colorVomi();
        m_maximumColorVomi = viewpointInformationChannel.maximumColorVomi();
        m_baseColorVomiRadioButton->setEnabled( true );
        m_colorVomiCheckBox->setEnabled( true );
        m_saveColorVomiPushButton->setEnabled( true );
    }

    if ( computeEvmiOpacity )
    {
        m_evmiOpacity = viewpointInformationChannel.evmiOpacity();
        m_saveEvmiOpacityPushButton->setEnabled( true );
    }

    if ( computeEvmiVomi )
    {
        m_evmiVomi = viewpointInformationChannel.evmiVomi();
        m_saveEvmiVomiPushButton->setEnabled( true );
    }

    if ( computeBestViews )
    {
        m_bestViews = viewpointInformationChannel.bestViews();
        m_saveBestViewsPushButton->setEnabled( true );
        m_tourBestViewsPushButton->setEnabled( true );
    }

    if ( computeGuidedTour )
    {
        m_guidedTour = viewpointInformationChannel.guidedTour();
        m_saveGuidedTourPushButton->setEnabled( true );
        m_guidedTourPushButton->setEnabled( true );
    }

    if ( computeExploratoryTour )
    {
        m_exploratoryTour = viewpointInformationChannel.exploratoryTour();
        m_saveExploratoryTourPushButton->setEnabled( true );
        m_exploratoryTourPushButton->setEnabled( true );
    }

    // Restaurem els paràmetres normals (en realitat només cal si es fa amb CPU)
    render();
    m_viewer->setCamera( position, focus, up );

    setCursor( QCursor( Qt::ArrowCursor ) );
}


void QExperimental3DExtension::computeSelectedVmii()
{
#ifndef CUDA_AVAILABLE
    QMessageBox::information( this, tr("Operation only available with CUDA"), "VMIi computations are only implemented in CUDA. Compile with CUDA support to use them." );
#else // CUDA_AVAILABLE
    // Què ha demanat l'usuari
    bool computeViewpointEntropy = m_computeViewpointEntropyICheckBox->isChecked();
    bool computeEntropy = m_computeEntropyICheckBox->isChecked();
    bool computeVmii = m_computeVmiiCheckBox->isChecked();
    bool computeMii = m_computeMiiCheckBox->isChecked();
    bool computeViewpointUnstabilities = m_computeViewpointUnstabilitiesICheckBox->isChecked();
    bool computeImi = m_computeImiCheckBox->isChecked();
//    bool computeViewpointVomi = m_computeViewpointVomiCheckBox->isChecked();
//    bool computeColorVomi = m_computeColorVomiCheckBox->isChecked();
//    bool computeEvmiOpacity = m_computeEvmiOpacityCheckBox->isChecked();
//    bool computeEvmiVomi = m_computeEvmiVomiCheckBox->isChecked();
//    bool computeBestViews = m_computeBestViewsCheckBox->isChecked();
//    bool computeGuidedTour = m_computeGuidedTourCheckBox->isChecked();
//    bool computeExploratoryTour = m_computeExploratoryTourCheckBox->isChecked();

    // Si no hi ha res a calcular marxem
    if ( !computeViewpointEntropy && !computeEntropy && !computeVmii && !computeMii && !computeViewpointUnstabilities && !computeImi /*&& !computeViewpointVomi && !computeColorVomi && !computeEvmiOpacity
         && !computeEvmiVomi && !computeBestViews && !computeGuidedTour && !computeExploratoryTour*/ ) return;

    setCursor( QCursor( Qt::WaitCursor ) );

    // Obtenir direccions
    Vector3 position, focus, up;
    m_viewer->getCamera( position, focus, up );
    float distance = ( position - focus ).length();
    ViewpointGenerator viewpointGenerator = m_vmiiViewpointDistributionWidget->viewpointGenerator( distance );

    // Viewpoint Intensity Information Channel
    ViewpointIntensityInformationChannel viewpointIntensityInformationChannel( viewpointGenerator, m_volume, m_viewer, m_transferFunctionEditor->transferFunction() );

    // Paleta de colors per la color VoMI
//    if ( computeColorVomi ) viewpointInformationChannel.setColorVomiPalette( m_colorVomiPalette );

    // Funció de transferència per l'EVMI amb opacitat
//    if ( computeEvmiOpacity )
//    {
//        if ( m_computeEvmiOpacityUseOtherPushButton->isChecked() ) viewpointInformationChannel.setEvmiOpacityTransferFunction( m_evmiOpacityTransferFunction );
//        else viewpointInformationChannel.setEvmiOpacityTransferFunction( m_transferFunctionEditor->transferFunction() );
//    }

    // Paràmetres extres per calcular les millors vistes (els passem sempre perquè tinguin algun valor, per si s'ha de calcular el guided tour per exemple)
//    viewpointInformationChannel.setBestViewsParameters( m_computeBestViewsNRadioButton->isChecked(), m_computeBestViewsNSpinBox->value(), m_computeBestViewsThresholdDoubleSpinBox->value() );

    // Llindar per calcular l'exploratory tour
//    viewpointInformationChannel.setExploratoryTourThreshold( m_computeExploratoryTourThresholdDoubleSpinBox->value() );

    // Filtratge de punts de vista
    if ( m_vmiiOneViewpointCheckBox->isChecked() )
    {
        int nViewpoints = m_vmiiViewpointDistributionWidget->numberOfViewpoints();
        int selectedViewpoint = m_vmiiOneViewpointSpinBox->value() - 1;

        QVector<bool> filter( nViewpoints );

        filter[selectedViewpoint] = true;

        QVector<int> neighbours = viewpointGenerator.neighbours( selectedViewpoint );
        for ( int i = 0; i < neighbours.size(); i++ ) filter[neighbours.at( i )] = true;

        viewpointIntensityInformationChannel.filterViewpoints( filter );
    }

    connect( &viewpointIntensityInformationChannel, SIGNAL( totalProgressMaximum(int) ), m_vmiiTotalProgressBar, SLOT( setMaximum(int) ) );
    connect( &viewpointIntensityInformationChannel, SIGNAL( totalProgressMaximum(int) ), m_vmiiTotalProgressBar, SLOT( repaint() ) ); // no sé per què però cal això perquè s'actualitzi quan toca
    connect( &viewpointIntensityInformationChannel, SIGNAL( totalProgress(int) ), m_vmiiTotalProgressBar, SLOT( setValue(int) ) );
    connect( &viewpointIntensityInformationChannel, SIGNAL( partialProgress(int) ), m_vmiiProgressBar, SLOT( setValue(int) ) );

    QTime time;
    time.start();
    viewpointIntensityInformationChannel.compute( computeViewpointEntropy, computeEntropy, computeVmii, computeMii, computeViewpointUnstabilities, computeImi, /*computeViewpointVomi, computeColorVomi, computeEvmiOpacity,
                                         computeEvmiVomi, computeBestViews, computeGuidedTour, computeExploratoryTour,*/ m_vmiiDisplayCheckBox->isChecked() );
    int elapsed = time.elapsed();
    DEBUG_LOG( QString( "Temps total de VMIi i altres: %1 s" ).arg( elapsed / 1000.0f ) );
    INFO_LOG( QString( "Temps total de VMIi i altres: %1 s" ).arg( elapsed / 1000.0f ) );

    if ( viewpointIntensityInformationChannel.hasViewedVolume() )
    {
        m_viewedVolumeI = viewpointIntensityInformationChannel.viewedVolume();
        m_saveViewedVolumeIPushButton->setEnabled( true );
    }

    if ( computeViewpointEntropy )
    {
        m_viewpointEntropyI = viewpointIntensityInformationChannel.viewpointEntropy();
        m_saveViewpointEntropyIPushButton->setEnabled( true );
    }

    if ( computeEntropy )
    {
        m_entropyI = viewpointIntensityInformationChannel.entropy();
        m_saveEntropyIPushButton->setEnabled( true );
    }

    if ( computeVmii )
    {
        m_vmii = viewpointIntensityInformationChannel.vmii();
        m_saveVmiiPushButton->setEnabled( true );
    }

    if ( computeMii )
    {
        m_mii = viewpointIntensityInformationChannel.mii();
        m_saveMiiPushButton->setEnabled( true );
    }

    if ( computeViewpointUnstabilities )
    {
        m_viewpointUnstabilitiesI = viewpointIntensityInformationChannel.viewpointUnstabilities();
        m_saveViewpointUnstabilitiesIPushButton->setEnabled( true );
    }

    if ( computeImi )
    {
        m_imi = viewpointIntensityInformationChannel.imi();
        m_maximumImi = viewpointIntensityInformationChannel.maximumImi();
        m_baseImiRadioButton->setEnabled( true );
//        m_baseImiCoolWarmRadioButton->setEnabled( true );
//        m_imiCheckBox->setEnabled( true );
//        m_imiCoolWarmCheckBox->setEnabled( true );
//        m_opacityLabel->setEnabled( true );
//        m_opacityImiCheckBox->setEnabled( true );
        m_saveImiPushButton->setEnabled( true );
//        m_imiGradientPushButton->setEnabled( true );
    }

//    if ( computeViewpointVomi )
//    {
//        m_viewpointVomi = viewpointInformationChannel.viewpointVomi();
//        m_saveViewpointVomiPushButton->setEnabled( true );
//    }
//
//    if ( computeColorVomi )
//    {
//        m_colorVomi = viewpointInformationChannel.colorVomi();
//        m_maximumColorVomi = viewpointInformationChannel.maximumColorVomi();
//        m_baseColorVomiRadioButton->setEnabled( true );
//        m_colorVomiCheckBox->setEnabled( true );
//        m_saveColorVomiPushButton->setEnabled( true );
//    }
//
//    if ( computeEvmiOpacity )
//    {
//        m_evmiOpacity = viewpointInformationChannel.evmiOpacity();
//        m_saveEvmiOpacityPushButton->setEnabled( true );
//    }
//
//    if ( computeEvmiVomi )
//    {
//        m_evmiVomi = viewpointInformationChannel.evmiVomi();
//        m_saveEvmiVomiPushButton->setEnabled( true );
//    }
//
//    if ( computeBestViews )
//    {
//        m_bestViews = viewpointInformationChannel.bestViews();
//        m_saveBestViewsPushButton->setEnabled( true );
//        m_tourBestViewsPushButton->setEnabled( true );
//    }
//
//    if ( computeGuidedTour )
//    {
//        m_guidedTour = viewpointInformationChannel.guidedTour();
//        m_saveGuidedTourPushButton->setEnabled( true );
//        m_guidedTourPushButton->setEnabled( true );
//    }
//
//    if ( computeExploratoryTour )
//    {
//        m_exploratoryTour = viewpointInformationChannel.exploratoryTour();
//        m_saveExploratoryTourPushButton->setEnabled( true );
//        m_exploratoryTourPushButton->setEnabled( true );
//    }

    // Restaurem els paràmetres normals (en realitat només cal si es fa amb CPU)
//    render();
//    m_viewer->setCamera( position, focus, up );

    setCursor( QCursor( Qt::ArrowCursor ) );
#endif // CUDA_AVAILABLE
}


void QExperimental3DExtension::tourBestViews()
{
    QList<Vector3> viewpoints;
    for ( int i = 0; i < m_bestViews.size(); i++ ) viewpoints << m_bestViews.at( i ).second;
    tour( viewpoints, m_tourSpeedDoubleSpinBox->value() );
}


void QExperimental3DExtension::guidedTour()
{
    QList<Vector3> viewpoints;
    for ( int i = 0; i < m_guidedTour.size(); i++ ) viewpoints << m_guidedTour.at( i ).second;
    tour( viewpoints, m_tourSpeedDoubleSpinBox->value() );
}


void QExperimental3DExtension::exploratoryTour()
{
    QList<Vector3> viewpoints;
    for ( int i = 0; i < m_exploratoryTour.size(); i++ ) viewpoints << m_exploratoryTour.at( i ).second;
    tour( viewpoints, m_tourSpeedDoubleSpinBox->value() );
}


void QExperimental3DExtension::computeVomiGradient()
{
    m_voxelSaliencies = m_volume->computeVomiGradient( m_vomi );
    m_maximumSaliency = 1.0f;
    m_baseVoxelSalienciesRadioButton->setEnabled( true );
    m_opacityLabel->setEnabled( true );
    m_opacitySaliencyCheckBox->setEnabled( true );
}


void QExperimental3DExtension::loadAndRunProgram()
{
    QString programFileName = getFileNameToLoad( Experimental3DSettings::ProgramDir, tr("Load program"), tr("Text files (*.txt);;All files (*)") );

    if ( programFileName.isNull() ) return;

    QFile programFile( programFileName );

    if ( !programFile.open( QFile::ReadOnly | QFile::Text ) )
    {
        DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + programFileName );
        QMessageBox::warning( this, tr("Can't load program"), QString( tr("Can't load program from file ") ) + programFileName );
        return;
    }

    m_interactive = false;

    QTextStream in( &programFile );
    bool run = false;
    bool errors = false;

    for ( int i = 0; i < 2; i++ )
    {
        int lineNumber = 0;

        while ( !in.atEnd() && !( errors && run ) )
        {
            lineNumber++;

            QString line = in.readLine();
            QStringList words = line.split( ' ', QString::SkipEmptyParts );

            if ( words.isEmpty() ) continue;

            QString command = words.at( 0 );

            if ( command == "//" ) continue;
            else if ( command == "tab" )
            {
                if ( programCheckWordCount( lineNumber, line, 2 ) ) errors |= !programTab( lineNumber, line, run );
                else errors = true;
            }
            else if ( command == "rendering-interpolation" )
            {
                if ( programCheckWordCount( lineNumber, line, 2 ) ) errors |= !programRenderingInterpolation( lineNumber, line, run );
                else errors = true;
            }
            else if ( command == "rendering-gradientestimator" )
            {
                if ( programCheckWordCount( lineNumber, line, 2 ) ) errors |= !programRenderingGradientEstimator( lineNumber, line, run );
                else errors = true;
            }
            else if ( command == "rendering-baseshading" )
            {
                if ( programCheckWordCount( lineNumber, line, 2 ) ) errors |= !programRenderingBaseShading( lineNumber, line, run );
                else errors = true;
            }
            else if ( command == "rendering-check" || command == "rendering-uncheck" )
            {
                if ( programCheckWordCount( lineNumber, line, 2 ) ) errors |= !programRenderingCheckOrUncheck( lineNumber, line, run );
                else errors = true;
            }
            else if ( command == "tf-load" )
            {
                if ( programCheckWordCount( lineNumber, line, 2 ) )
                {
                    if ( run ) loadTransferFunction( words.at( 1 ) );
                }
                else errors = true;
            }
            else if ( command == "render" )
            {
                if ( run ) render();
            }
            else if ( command == "camera-load" )
            {
                if ( programCheckWordCount( lineNumber, line, 2 ) )
                {
                    if ( run ) loadCamera( words.at( 1 ) );
                }
                else errors = true;
            }
            else if ( command == "vmi-viewpoints" )
            {
                if ( programCheckWordCount( lineNumber, line, 3 ) ) errors |= !programVmiViewpoints( lineNumber, line, run );
                else errors = true;
            }
            else if ( command == "vmi-check" || command == "vmi-uncheck" )
            {
                if ( programCheckWordCount( lineNumber, line, 2 ) ) errors |= !programVmiCheckOrUncheck( lineNumber, line, run );
                else errors = true;
            }
            else if ( command == "vmi-run" )
            {
                if ( run ) computeSelectedVmi();
            }
            else if ( command == "vmi-vomigradient" )
            {
                if ( run )
                {
                    if ( m_vomiGradientPushButton->isEnabled() ) computeVomiGradient();
                    else
                    {
                        logProgramError( lineNumber, "No es pot calcular el gradient de la VoMI", line );
                        errors = true;
                    }
                }
            }
            else if ( command == "vmi-load" || command == "vmi-save" )
            {
                if ( programCheckWordCount( lineNumber, line, 3 ) ) errors |= !programVmiLoadOrSave( lineNumber, line, run );
                else errors = true;
            }
            else if ( command == "screenshot" )
            {
                if ( programCheckWordCount( lineNumber, line, 2 ) )
                {
                    if ( run ) m_viewer->screenshot( words.at( 1 ) );
                }
                else errors = true;
            }
            else
            {
                logProgramError( lineNumber, "Ordre desconeguda", line );
                errors = true;
            }
        }

        if ( errors )
        {
            QMessageBox::warning( this, tr("Errors in program"), tr("The errors have been written in the log.") );
            break;
        }
        else
        {
            in.seek( 0 );
            run = true;
        }
    }

    programFile.close();

    m_interactive = true;
}


void QExperimental3DExtension::logProgramError( int lineNumber, const QString &error, const QString &line ) const
{
    DEBUG_LOG( "[E3DP](" + QString::number( lineNumber ) + ") " + error + ": " + line );
    ERROR_LOG( "[E3DP](" + QString::number( lineNumber ) + ") " + error + ": " + line );
}


bool QExperimental3DExtension::programCheckWordCount( int lineNumber, const QString &line, int wordCount ) const
{
    if ( line.split( ' ', QString::SkipEmptyParts ).size() < wordCount )
    {
        logProgramError( lineNumber, "Falten paràmetres", line );
        return false;
    }
    else return true;
}


bool QExperimental3DExtension::programTab( int lineNumber, const QString &line, bool run )
{
    QString tab = line.split( ' ', QString::SkipEmptyParts ).at( 1 );

    if ( tab == "rendering" )
    {
        if ( run ) m_controlsTabWidget->setCurrentWidget( m_renderingTab );
    }
    else if ( tab == "camera" )
    {
        if ( run ) m_controlsTabWidget->setCurrentWidget( m_cameraTab );
    }
    else if ( tab == "obscurance" )
    {
        if ( run ) m_controlsTabWidget->setCurrentWidget( m_obscuranceTab );
    }
    else if ( tab == "smi" )
    {
        if ( run ) m_controlsTabWidget->setCurrentWidget( m_smiTab );
    }
    else if ( tab == "vmi" )
    {
        if ( run ) m_controlsTabWidget->setCurrentWidget( m_vmiTab );
    }
    else if ( tab == "program" )
    {
        if ( run ) m_controlsTabWidget->setCurrentWidget( m_programTab );
    }
    else
    {
        logProgramError( lineNumber, "Paràmetre/s incorrecte/s", line );
        return false;
    }

    return true;
}


bool QExperimental3DExtension::programRenderingInterpolation( int lineNumber, const QString &line, bool run )
{
    QString interpolation = line.split( ' ', QString::SkipEmptyParts ).at( 1 );

    if ( interpolation == "nn" )
    {
        if ( run ) m_interpolationComboBox->setCurrentIndex( 0 );
    }
    else if ( interpolation == "lic" )
    {
        if ( run ) m_interpolationComboBox->setCurrentIndex( 1 );
    }
    else if ( interpolation == "lci" )
    {
        if ( run ) m_interpolationComboBox->setCurrentIndex( 2 );
    }
    else
    {
        logProgramError( lineNumber, "Paràmetre/s incorrecte/s", line );
        return false;
    }

    return true;
}


bool QExperimental3DExtension::programRenderingGradientEstimator( int lineNumber, const QString &line, bool run )
{
    QString gradientEstimator = line.split( ' ', QString::SkipEmptyParts ).at( 1 );

    if ( gradientEstimator == "finitedifference" )
    {
        if ( run ) m_gradientEstimatorComboBox->setCurrentIndex( 0 );
    }
    else if ( gradientEstimator == "4dlr1" )
    {
        if ( run ) m_gradientEstimatorComboBox->setCurrentIndex( 1 );
    }
    else if ( gradientEstimator == "4dlr2" )
    {
        if ( run ) m_gradientEstimatorComboBox->setCurrentIndex( 2 );
    }
    else
    {
        logProgramError( lineNumber, "Paràmetre/s incorrecte/s", line );
        return false;
    }

    return true;
}


bool QExperimental3DExtension::programRenderingBaseShading( int lineNumber, const QString &line, bool run )
{
    QStringList words = line.split( ' ', QString::SkipEmptyParts );
    const QString &base = words.at( 1 );

    if ( base == "ambient" )
    {
        if ( run ) m_baseAmbientLightingRadioButton->setChecked( true );
    }
    else if ( base == "diffuse" )
    {
        if ( run ) m_baseDiffuseLightingRadioButton->setChecked( true );

        if ( words.size() > 2 )
        {
            if ( words.at( 2 ) == "specular" )
            {
                if ( run )
                {
                    m_baseSpecularLightingCheckBox->setChecked( true );
                    if ( words.size() > 3 ) m_baseSpecularLightingPowerDoubleSpinBox->setValue( words.at( 3 ).toDouble() );
                }
            }
            else
            {
                logProgramError( lineNumber, "Paràmetre/s incorrecte/s", line );
                return false;
            }
        }
    }
    else if ( base == "vomi" )
    {
        if ( run )
        {
            if ( m_baseVomiRadioButton->isEnabled() )
            {
                m_baseVomiRadioButton->setChecked( true );
                if ( words.size() > 2 ) m_baseVomiFactorDoubleSpinBox->setValue( words.at( 2 ).toDouble() );
            }
            else
            {
                logProgramError( lineNumber, "No es pot activar VoMI", line );
                return false;
            }
        }
    }
    else if ( base == "cvomi" )
    {
        if ( run )
        {
            if ( m_baseColorVomiRadioButton->isEnabled() )
            {
                m_baseColorVomiRadioButton->setChecked( true );
                if ( words.size() > 2 ) m_baseColorVomiFactorDoubleSpinBox->setValue( words.at( 2 ).toDouble() );
            }
            else
            {
                logProgramError( lineNumber, "No es pot activar color VoMI", line );
                return false;
            }
        }
    }
    else if ( base == "saliency" )
    {
        if ( run )
        {
            if ( m_baseVoxelSalienciesRadioButton->isEnabled() )
            {
                m_baseVoxelSalienciesRadioButton->setChecked( true );
                if ( words.size() > 2 ) m_baseVoxelSalienciesFactorDoubleSpinBox->setValue( words.at( 2 ).toDouble() );
            }
            else
            {
                logProgramError( lineNumber, "No es pot activar voxel saliencies", line );
                return false;
            }
        }
    }
    else
    {
        logProgramError( lineNumber, "Paràmetre/s incorrecte/s", line );
        return false;
    }

    return true;
}


bool QExperimental3DExtension::programRenderingCheckOrUncheck( int lineNumber, const QString &line, bool run )
{
    QStringList words = line.split( ' ', QString::SkipEmptyParts );
    bool check = words.at( 0 ) == "rendering-check";
    const QString &checkbox = words.at( 1 );

    if ( checkbox == "contour" )
    {
        if ( run )
        {
            m_contourCheckBox->setChecked( check );
            if ( check && words.size() > 2 ) m_contourDoubleSpinBox->setValue( words.at( 2 ).toDouble() );
        }
    }
    else if ( checkbox == "obscurance" )
    {
        if ( run )
        {
            if ( m_obscuranceCheckBox->isEnabled() )
            {
                m_obscuranceCheckBox->setChecked( check );

                if ( check && words.size() > 2 )
                {
                    m_obscuranceFactorDoubleSpinBox->setValue( words.at( 2 ).toDouble() );

                    if ( words.size() > 3 )
                    {
                        m_obscuranceLowFilterDoubleSpinBox->setValue( words.at( 3 ).toDouble() );
                        if ( words.size() > 4 ) m_obscuranceHighFilterDoubleSpinBox->setValue( words.at( 4 ).toDouble() );
                    }
                }
            }
            else
            {
                logProgramError( lineNumber, "No es pot activar les obscurances", line );
                return false;
            }
        }
    }
    else if ( checkbox == "vomi" )
    {
        if ( run )
        {
            if ( m_vomiCheckBox->isEnabled() )
            {
                m_vomiCheckBox->setChecked( check );
                if ( check && words.size() > 2 ) m_vomiFactorDoubleSpinBox->setValue( words.at( 2 ).toDouble() );
            }
            else
            {
                logProgramError( lineNumber, "No es pot activar VoMI", line );
                return false;
            }
        }
    }
    else if ( checkbox == "cvomi" )
    {
        if ( run )
        {
            if ( m_colorVomiCheckBox->isEnabled() )
            {
                m_colorVomiCheckBox->setChecked( check );
                if ( check && words.size() > 2 ) m_colorVomiFactorDoubleSpinBox->setValue( words.at( 2 ).toDouble() );
            }
            else
            {
                logProgramError( lineNumber, "No es pot activar color VoMI", line );
                return false;
            }
        }
    }
    else if ( checkbox == "opacity-vomi" )
    {
        if ( run )
        {
            if ( m_opacityVomiCheckBox->isEnabled() )
            {
                m_opacityVomiCheckBox->setChecked( check );
            }
            else
            {
                logProgramError( lineNumber, "No es pot activar l'opacitat de VoMI", line );
                return false;
            }
        }
    }
    else if ( checkbox == "opacity-saliency" )
    {
        if ( run )
        {
            if ( m_opacitySaliencyCheckBox->isEnabled() )
            {
                m_opacitySaliencyCheckBox->setChecked( check );
            }
            else
            {
                logProgramError( lineNumber, "No es pot activar l'opacitat de la saliency", line );
                return false;
            }
        }
    }
    else if ( checkbox == "celshading" )
    {
        if ( run )
        {
            m_celShadingCheckBox->setChecked( check );
            if ( check && words.size() > 2 ) m_celShadingQuantumsSpinBox->setValue( words.at( 2 ).toInt() );
        }
    }
    else
    {
        logProgramError( lineNumber, "Paràmetre/s incorrecte/s", line );
        return false;
    }

    return true;
}


bool QExperimental3DExtension::programVmiViewpoints( int lineNumber, const QString &line, bool run )
{
    QStringList words = line.split( ' ', QString::SkipEmptyParts );
    const QString &distribution = words.at( 1 );

    if ( distribution == "uni" )
    {
        int number = words.at( 2 ).toInt();

        switch ( number )
        {
            case 4: if ( run ) m_vmiViewpointDistributionWidget->setToUniform4(); break;
            case 6: if ( run ) m_vmiViewpointDistributionWidget->setToUniform6(); break;
            case 8: if ( run ) m_vmiViewpointDistributionWidget->setToUniform8(); break;
            case 12: if ( run ) m_vmiViewpointDistributionWidget->setToUniform12(); break;
            case 20: if ( run ) m_vmiViewpointDistributionWidget->setToUniform20(); break;
            default: logProgramError( lineNumber, "Nombre incorrecte de punts uniformes", line ); return false;
        }
    }
    else if ( distribution == "q-uni" )
    {
        if ( run ) m_vmiViewpointDistributionWidget->setToQuasiUniform( words.at( 2 ).toInt() );
    }
    else
    {
        logProgramError( lineNumber, "Paràmetre/s incorrecte/s", line );
        return false;
    }

    return true;
}


bool QExperimental3DExtension::programVmiCheckOrUncheck( int lineNumber, const QString &line, bool run )
{
    QStringList words = line.split( ' ', QString::SkipEmptyParts );
    bool check = words.at( 0 ) == "vmi-check";
    const QString &checkbox = words.at( 1 );

    if ( checkbox == "viewpointentropy" )
    {
        if ( run ) m_computeViewpointEntropyCheckBox->setChecked( check );
    }
    else if ( checkbox == "vmi" )
    {
        if ( run ) m_computeVmiCheckBox->setChecked( check );
    }
    else if ( checkbox == "unstabilities" )
    {
        if ( run ) m_computeViewpointUnstabilitiesCheckBox->setChecked( check );
    }
    else if ( checkbox == "bestviews" )
    {
        if ( run ) m_computeBestViewsCheckBox->setChecked( check );

        if ( check && words.size() > 3 )
        {
            if ( words.at( 2 ) == "n" )
            {
                if ( run )
                {
                    m_computeBestViewsNRadioButton->setChecked( true );
                    m_computeBestViewsNSpinBox->setValue( words.at( 3 ).toInt() );
                }
            }
            else if ( words.at( 2 ) == "threshold" )
            {
                if ( run )
                {
                    m_computeBestViewsThresholdRadioButton->setChecked( true );
                    m_computeBestViewsThresholdDoubleSpinBox->setValue( words.at( 3 ).toDouble() );
                }
            }
            else
            {
                logProgramError( lineNumber, "Paràmetre/s incorrecte/s", line );
                return false;
            }
        }
    }
    else if ( checkbox == "guidedtour" )
    {
        if ( run ) m_computeGuidedTourCheckBox->setChecked( check );
    }
    else if ( checkbox == "vomi" )
    {
        if ( run ) m_computeVomiCheckBox->setChecked( check );
    }
    else if ( checkbox == "vvomi" )
    {
        if ( run ) m_computeViewpointVomiCheckBox->setChecked( check );
    }
    else if ( checkbox == "cvomi" )
    {
        if ( run )
        {
            m_computeColorVomiCheckBox->setChecked( check );
            if ( check && words.size() > 2 ) loadColorVomiPalette( words.at( 2 ) );
        }
    }
    else
    {
        logProgramError( lineNumber, "Paràmetre/s incorrecte/s", line );
        return false;
    }

    return true;
}


bool QExperimental3DExtension::programVmiLoadOrSave( int lineNumber, const QString &line, bool run )
{
    QStringList words = line.split( ' ', QString::SkipEmptyParts );
    bool load = words.at( 0 ) == "vmi-load";
    const QString &measure = words.at( 1 );
    const QString &fileName = words.at( 2 );

    if ( measure == "viewpointentropy" )
    {
        if ( run )
        {
            if ( load ) loadViewpointEntropy( fileName );
            else
            {
                if ( m_saveViewpointEntropyPushButton->isEnabled() ) saveViewpointEntropy( fileName );
                else
                {
                    logProgramError( lineNumber, "No es pot desar l'entropia dels punts de vista", line );
                    return false;
                }
            }
        }
    }
    else if ( measure == "vmi" )
    {
        if ( run )
        {
            if ( load ) loadVmi( fileName );
            else
            {
                if ( m_saveVmiPushButton->isEnabled() ) saveVmi( fileName );
                else
                {
                    logProgramError( lineNumber, "No es pot desar la VMI", line );
                    return false;
                }
            }
        }
    }
    else if ( measure == "unstabilities" )
    {
        if ( run )
        {
            if ( load ) loadViewpointUnstabilities( fileName );
            else
            {
                if ( m_saveViewpointUnstabilitiesPushButton->isEnabled() ) saveViewpointUnstabilities( fileName );
                else
                {
                    logProgramError( lineNumber, "No es pot desar la inestabilitat dels punts de vista", line );
                    return false;
                }
            }
        }
    }
    else if ( measure == "bestviews" )
    {
        if ( run )
        {
            if ( load ) loadBestViews( fileName );
            else
            {
                if ( m_saveBestViewsPushButton->isEnabled() ) saveBestViews( fileName );
                else
                {
                    logProgramError( lineNumber, "No es pot desar les millors vistes", line );
                    return false;
                }
            }
        }
    }
    else if ( measure == "guidedtour" )
    {
        if ( run )
        {
            if ( load ) loadGuidedTour( fileName );
            else
            {
                if ( m_saveGuidedTourPushButton->isEnabled() ) saveGuidedTour( fileName );
                else
                {
                    logProgramError( lineNumber, "No es pot desar el guided tour", line );
                    return false;
                }
            }
        }
    }
    else if ( measure == "vomi" )
    {
        if ( run )
        {
            if ( load ) loadVomi( fileName );
            else
            {
                if ( m_saveVomiPushButton->isEnabled() ) saveVomi( fileName );
                else
                {
                    logProgramError( lineNumber, "No es pot desar la VoMI", line );
                    return false;
                }
            }
        }
    }
    else if ( measure == "vvomi" )
    {
        if ( run )
        {
            if ( load ) loadViewpointVomi( fileName );
            else
            {
                if ( m_saveViewpointVomiPushButton->isEnabled() ) saveViewpointVomi( fileName );
                else
                {
                    logProgramError( lineNumber, "No es pot desar la VoMI dels punts de vista", line );
                    return false;
                }
            }
        }
    }
    else if ( measure == "cvomi" )
    {
        if ( run )
        {
            if ( load ) loadColorVomi( fileName );
            else
            {
                if ( m_saveColorVomiPushButton->isEnabled() ) saveColorVomi( fileName );
                else
                {
                    logProgramError( lineNumber, "No es pot desar la color VoMI", line );
                    return false;
                }
            }
        }
    }
    else
    {
        logProgramError( lineNumber, "Paràmetre/s incorrecte/s", line );
        return false;
    }

    return true;
}


void QExperimental3DExtension::opacityVomiChecked( bool checked )
{
    if ( checked )
    {
        m_opacitySaliencyCheckBox->setChecked( false );
        m_opacityFilteringCheckBox->setChecked( false );
        m_opacityProbabilisticAmbientOcclusionCheckBox->setChecked( false );
        m_opacityLowLabel->setEnabled( true );
        m_opacityLowThresholdLabel->setEnabled( true );
        m_opacityLowThresholdDoubleSpinBox->setEnabled( true );
        m_opacityLowFactorLabel->setEnabled( true );
        m_opacityLowFactorDoubleSpinBox->setEnabled( true );
        m_opacityHighLabel->setEnabled( true );
        m_opacityHighThresholdLabel->setEnabled( true );
        m_opacityHighThresholdDoubleSpinBox->setEnabled( true );
        m_opacityHighFactorLabel->setEnabled( true );
        m_opacityHighFactorDoubleSpinBox->setEnabled( true );
    }
    else
    {
        m_opacityLowLabel->setEnabled( false );
        m_opacityLowThresholdLabel->setEnabled( false );
        m_opacityLowThresholdDoubleSpinBox->setEnabled( false );
        m_opacityLowFactorLabel->setEnabled( false );
        m_opacityLowFactorDoubleSpinBox->setEnabled( false );
        m_opacityHighLabel->setEnabled( false );
        m_opacityHighThresholdLabel->setEnabled( false );
        m_opacityHighThresholdDoubleSpinBox->setEnabled( false );
        m_opacityHighFactorLabel->setEnabled( false );
        m_opacityHighFactorDoubleSpinBox->setEnabled( false );
    }
}


void QExperimental3DExtension::opacitySaliencyChecked( bool checked )
{
    if ( checked )
    {
        m_opacityVomiCheckBox->setChecked( false );
        m_opacityFilteringCheckBox->setChecked( false );
        m_opacityProbabilisticAmbientOcclusionCheckBox->setChecked( false );
        m_opacityLowLabel->setEnabled( true );
        m_opacityLowThresholdLabel->setEnabled( true );
        m_opacityLowThresholdDoubleSpinBox->setEnabled( true );
        m_opacityLowFactorLabel->setEnabled( true );
        m_opacityLowFactorDoubleSpinBox->setEnabled( true );
        m_opacityHighLabel->setEnabled( true );
        m_opacityHighThresholdLabel->setEnabled( true );
        m_opacityHighThresholdDoubleSpinBox->setEnabled( true );
        m_opacityHighFactorLabel->setEnabled( true );
        m_opacityHighFactorDoubleSpinBox->setEnabled( true );
    }
    else
    {
        m_opacityLowLabel->setEnabled( false );
        m_opacityLowThresholdLabel->setEnabled( false );
        m_opacityLowThresholdDoubleSpinBox->setEnabled( false );
        m_opacityLowFactorLabel->setEnabled( false );
        m_opacityLowFactorDoubleSpinBox->setEnabled( false );
        m_opacityHighLabel->setEnabled( false );
        m_opacityHighThresholdLabel->setEnabled( false );
        m_opacityHighThresholdDoubleSpinBox->setEnabled( false );
        m_opacityHighFactorLabel->setEnabled( false );
        m_opacityHighFactorDoubleSpinBox->setEnabled( false );
    }
}


void QExperimental3DExtension::opacityFilteringChecked( bool checked )
{
    if ( checked )
    {
        m_opacityVomiCheckBox->setChecked( false );
        m_opacitySaliencyCheckBox->setChecked( false );
        m_opacityProbabilisticAmbientOcclusionCheckBox->setChecked( false );
        m_opacityLowLabel->setEnabled( true );
        m_opacityLowThresholdLabel->setEnabled( true );
        m_opacityLowThresholdDoubleSpinBox->setEnabled( true );
        m_opacityLowFactorLabel->setEnabled( true );
        m_opacityLowFactorDoubleSpinBox->setEnabled( true );
        m_opacityHighLabel->setEnabled( true );
        m_opacityHighThresholdLabel->setEnabled( true );
        m_opacityHighThresholdDoubleSpinBox->setEnabled( true );
        m_opacityHighFactorLabel->setEnabled( true );
        m_opacityHighFactorDoubleSpinBox->setEnabled( true );
    }
    else
    {
        m_opacityLowLabel->setEnabled( false );
        m_opacityLowThresholdLabel->setEnabled( false );
        m_opacityLowThresholdDoubleSpinBox->setEnabled( false );
        m_opacityLowFactorLabel->setEnabled( false );
        m_opacityLowFactorDoubleSpinBox->setEnabled( false );
        m_opacityHighLabel->setEnabled( false );
        m_opacityHighThresholdLabel->setEnabled( false );
        m_opacityHighThresholdDoubleSpinBox->setEnabled( false );
        m_opacityHighFactorLabel->setEnabled( false );
        m_opacityHighFactorDoubleSpinBox->setEnabled( false );
    }
}


void QExperimental3DExtension::opacityProbabilisticAmbientOcclusionChecked( bool checked )
{
    if ( checked )
    {
        m_opacityVomiCheckBox->setChecked( false );
        m_opacitySaliencyCheckBox->setChecked( false );
        m_opacityFilteringCheckBox->setChecked( false );
        m_opacityLowLabel->setEnabled( true );
        m_opacityLowThresholdLabel->setEnabled( true );
        m_opacityLowThresholdDoubleSpinBox->setEnabled( true );
        m_opacityLowFactorLabel->setEnabled( true );
        m_opacityLowFactorDoubleSpinBox->setEnabled( true );
        m_opacityHighLabel->setEnabled( true );
        m_opacityHighThresholdLabel->setEnabled( true );
        m_opacityHighThresholdDoubleSpinBox->setEnabled( true );
        m_opacityHighFactorLabel->setEnabled( true );
        m_opacityHighFactorDoubleSpinBox->setEnabled( true );
    }
    else
    {
        m_opacityLowLabel->setEnabled( false );
        m_opacityLowThresholdLabel->setEnabled( false );
        m_opacityLowThresholdDoubleSpinBox->setEnabled( false );
        m_opacityLowFactorLabel->setEnabled( false );
        m_opacityLowFactorDoubleSpinBox->setEnabled( false );
        m_opacityHighLabel->setEnabled( false );
        m_opacityHighThresholdLabel->setEnabled( false );
        m_opacityHighThresholdDoubleSpinBox->setEnabled( false );
        m_opacityHighFactorLabel->setEnabled( false );
        m_opacityHighFactorDoubleSpinBox->setEnabled( false );
    }
}


void QExperimental3DExtension::setVmiOneViewpointMaximum( int maximum )
{
    m_vmiOneViewpointSpinBox->setMaximum( maximum );
}


void QExperimental3DExtension::setVmiiOneViewpointMaximum( int maximum )
{
    m_vmiiOneViewpointSpinBox->setMaximum( maximum );
}


void QExperimental3DExtension::getFileNameToSaveTour()
{
    QString fileName = getFileNameToSave( Experimental3DSettings::TourDir, tr("Save tour"), tr("PNG files (*.png);;All files (*)"), "png" );

    if ( fileName.isNull() ) return;

    if ( !fileName.contains( "%1" ) )
    {
        int i = fileName.lastIndexOf( "." );
        fileName.insert( i, "%1" );
    }

    m_saveNextTourLineEdit->setText( fileName );
}


void QExperimental3DExtension::gaussianFilter()
{
    vtkImageCast *cast = vtkImageCast::New();
    cast->SetInput( m_volume->getImage() );
    cast->SetOutputScalarTypeToFloat();
    cast->Update();

#ifndef CUDA_AVAILABLE
    vtkImageGaussianSmooth *gaussian = vtkImageGaussianSmooth::New();
    gaussian->SetInput( cast->GetOutput() );
    gaussian->SetDimensionality( 3 );
    gaussian->SetRadiusFactor( m_filteringRadiusSpinBox->value() );
    gaussian->SetStandardDeviation( 1.0 );
    gaussian->Update();

    vtkImageMathematics *substract = vtkImageMathematics::New();
    substract->SetInput1( gaussian->GetOutput() );
    substract->SetInput2( cast->GetOutput() );
    substract->SetOperationToSubtract();
    substract->Update();

    vtkImageData *difference = substract->GetOutput();
    float *data = reinterpret_cast<float*>( difference->GetScalarPointer() );
    m_spatialImportanceFunction.resize( m_volume->getSize() );
    memcpy( m_spatialImportanceFunction.data(), data, m_spatialImportanceFunction.size() * sizeof(float) );
    double *range = difference->GetScalarRange();
    m_maximumSpatialImportanceFunction = qMax( qAbs( range[0] ), qAbs( range[1] ) );

    gaussian->Delete();
    substract->Delete();
#else // CUDA_AVAILABLE
    m_spatialImportanceFunction = cfGaussianDifference( cast->GetOutput(), m_filteringRadiusSpinBox->value() );
    int size = m_volume->getSize();
    m_maximumSpatialImportanceFunction = 0.0f;
    for ( int i = 0; i < size; i++ )
    {
        float f = qAbs( m_spatialImportanceFunction.at( i ) );
        if ( f > m_maximumSpatialImportanceFunction ) m_maximumSpatialImportanceFunction = f;
    }
#endif // CUDA_AVAILABLE

    m_baseFilteringAmbientOcclusionRadioButton->setEnabled( true );
    m_filteringAmbientOcclusionCheckBox->setEnabled( true );
    m_opacityFilteringCheckBox->setEnabled( true );

    cast->Delete();
}


void QExperimental3DExtension::boxMeanFilter()
{
    vtkImageCast *cast = vtkImageCast::New();
    cast->SetInput( m_volume->getImage() );
    cast->SetOutputScalarTypeToFloat();
    cast->Update();

#ifndef CUDA_AVAILABLE
    QMessageBox::information( this, tr("Operation only available with CUDA"), "The box filter is only implemented in CUDA. Compile with CUDA support to use it." );
#else // CUDA_AVAILABLE
    m_spatialImportanceFunction = cfBoxMeanDifference( cast->GetOutput(), m_filteringRadiusSpinBox->value() );
    int size = m_volume->getSize();
    m_maximumSpatialImportanceFunction = 0.0f;
    for ( int i = 0; i < size; i++ )
    {
        float f = qAbs( m_spatialImportanceFunction.at( i ) );
        if ( f > m_maximumSpatialImportanceFunction ) m_maximumSpatialImportanceFunction = f;
    }
#endif // CUDA_AVAILABLE

    m_baseFilteringAmbientOcclusionRadioButton->setEnabled( true );
    m_filteringAmbientOcclusionCheckBox->setEnabled( true );
    m_opacityFilteringCheckBox->setEnabled( true );

    cast->Delete();
}


void QExperimental3DExtension::probabilisticAmbientOcclusionGaussianChebychev()
{
    vtkImageCast *cast = vtkImageCast::New();
    cast->SetInput( m_volume->getImage() );
    cast->SetOutputScalarTypeToFloat();
    cast->Update();

#ifndef CUDA_AVAILABLE
    QMessageBox::information( this, tr("Operation only available with CUDA"), "The PAO Gaussian Chebychev is only implemented in CUDA. Compile with CUDA support to use it." );
#else // CUDA_AVAILABLE
    m_probabilisticAmbientOcclusion = cfProbabilisticAmbientOcclusionGaussianChebychev( cast->GetOutput(), m_probabilisticAmbientOcclusionRadiusSpinBox->value() );
#ifndef QT_NO_DEBUG
    int size = m_volume->getSize();
    for ( int i = 0; i < size; i++ )
    {
        if ( m_probabilisticAmbientOcclusion.at( i ) < 0.0f || m_probabilisticAmbientOcclusion.at( i ) > 1.0f )
        {
            DEBUG_LOG( QString( "pao[%1] = %2" ).arg( i ).arg( m_probabilisticAmbientOcclusion.at( i ) ) );
        }
    }
#endif // QT_NO_DEBUG
#endif // CUDA_AVAILABLE

    m_baseProbabilisticAmbientOcclusionRadioButton->setEnabled( true );
    m_probabilisticAmbientOcclusionCheckBox->setEnabled( true );
    m_opacityProbabilisticAmbientOcclusionCheckBox->setEnabled( true );

    cast->Delete();
}


void QExperimental3DExtension::probabilisticAmbientOcclusionBoxMeanChebychev()
{
    vtkImageCast *cast = vtkImageCast::New();
    cast->SetInput( m_volume->getImage() );
    cast->SetOutputScalarTypeToFloat();
    cast->Update();

#ifndef CUDA_AVAILABLE
    QMessageBox::information( this, tr("Operation only available with CUDA"), "The PAO box mean Chebychev is only implemented in CUDA. Compile with CUDA support to use it." );
#else // CUDA_AVAILABLE
    m_probabilisticAmbientOcclusion = cfProbabilisticAmbientOcclusionBoxMeanChebychev( cast->GetOutput(), m_probabilisticAmbientOcclusionRadiusSpinBox->value() );
#ifndef QT_NO_DEBUG
    int size = m_volume->getSize();
    for ( int i = 0; i < size; i++ )
    {
        if ( m_probabilisticAmbientOcclusion.at( i ) < 0.0f || m_probabilisticAmbientOcclusion.at( i ) > 1.0f )
        {
            DEBUG_LOG( QString( "pao[%1] = %2" ).arg( i ).arg( m_probabilisticAmbientOcclusion.at( i ) ) );
        }
    }
#endif // QT_NO_DEBUG
#endif // CUDA_AVAILABLE

    m_baseProbabilisticAmbientOcclusionRadioButton->setEnabled( true );
    m_probabilisticAmbientOcclusionCheckBox->setEnabled( true );
    m_opacityProbabilisticAmbientOcclusionCheckBox->setEnabled( true );

    cast->Delete();
}


void QExperimental3DExtension::probabilisticAmbientOcclusionGaussian()
{
    vtkImageCast *cast = vtkImageCast::New();
    cast->SetInput( m_volume->getImage() );
    cast->SetOutputScalarTypeToFloat();
    cast->Update();

#ifndef CUDA_AVAILABLE
    QMessageBox::information( this, tr("Operation only available with CUDA"), "The PAO Gaussian is only implemented in CUDA. Compile with CUDA support to use it." );
#else // CUDA_AVAILABLE
    m_probabilisticAmbientOcclusion = cfProbabilisticAmbientOcclusionGaussian( cast->GetOutput(), m_probabilisticAmbientOcclusionRadiusSpinBox->value() );
#ifndef QT_NO_DEBUG
    int size = m_volume->getSize();
    for ( int i = 0; i < size; i++ )
    {
        if ( m_probabilisticAmbientOcclusion.at( i ) < 0.0f || m_probabilisticAmbientOcclusion.at( i ) > 1.0f )
        {
            DEBUG_LOG( QString( "pao[%1] = %2" ).arg( i ).arg( m_probabilisticAmbientOcclusion.at( i ) ) );
        }
    }
#endif // QT_NO_DEBUG
#endif // CUDA_AVAILABLE

    m_baseProbabilisticAmbientOcclusionRadioButton->setEnabled( true );
    m_probabilisticAmbientOcclusionCheckBox->setEnabled( true );
    m_opacityProbabilisticAmbientOcclusionCheckBox->setEnabled( true );

    cast->Delete();
}


void QExperimental3DExtension::probabilisticAmbientOcclusionCube()
{
    vtkImageCast *cast = vtkImageCast::New();
    cast->SetInput( m_volume->getImage() );
    cast->SetOutputScalarTypeToFloat();
    cast->Update();

#ifndef CUDA_AVAILABLE
    QMessageBox::information( this, tr("Operation only available with CUDA"), "The PAO cube is only implemented in CUDA. Compile with CUDA support to use it." );
#else // CUDA_AVAILABLE
    m_probabilisticAmbientOcclusion = cfProbabilisticAmbientOcclusionCube( cast->GetOutput(), m_probabilisticAmbientOcclusionRadiusSpinBox->value() );
#ifndef QT_NO_DEBUG
    int size = m_volume->getSize();
    for ( int i = 0; i < size; i++ )
    {
        if ( m_probabilisticAmbientOcclusion.at( i ) < 0.0f || m_probabilisticAmbientOcclusion.at( i ) > 1.0f )
        {
            DEBUG_LOG( QString( "pao[%1] = %2" ).arg( i ).arg( m_probabilisticAmbientOcclusion.at( i ) ) );
        }
    }
#endif // QT_NO_DEBUG
#endif // CUDA_AVAILABLE

    m_baseProbabilisticAmbientOcclusionRadioButton->setEnabled( true );
    m_probabilisticAmbientOcclusionCheckBox->setEnabled( true );
    m_opacityProbabilisticAmbientOcclusionCheckBox->setEnabled( true );

    cast->Delete();
}


void QExperimental3DExtension::probabilisticAmbientOcclusionSphere()
{
    vtkImageCast *cast = vtkImageCast::New();
    cast->SetInput( m_volume->getImage() );
    cast->SetOutputScalarTypeToFloat();
    cast->Update();

#ifndef CUDA_AVAILABLE
    QMessageBox::information( this, tr("Operation only available with CUDA"), "The PAO sphere is only implemented in CUDA. Compile with CUDA support to use it." );
#else // CUDA_AVAILABLE
    m_probabilisticAmbientOcclusion = cfProbabilisticAmbientOcclusionSphere( cast->GetOutput(), m_probabilisticAmbientOcclusionRadiusSpinBox->value() );
#ifndef QT_NO_DEBUG
    int size = m_volume->getSize();
    for ( int i = 0; i < size; i++ )
    {
        if ( m_probabilisticAmbientOcclusion.at( i ) < 0.0f || m_probabilisticAmbientOcclusion.at( i ) > 1.0f )
        {
            DEBUG_LOG( QString( "pao[%1] = %2" ).arg( i ).arg( m_probabilisticAmbientOcclusion.at( i ) ) );
        }
    }
#endif // QT_NO_DEBUG
#endif // CUDA_AVAILABLE

    m_baseProbabilisticAmbientOcclusionRadioButton->setEnabled( true );
    m_probabilisticAmbientOcclusionCheckBox->setEnabled( true );
    m_opacityProbabilisticAmbientOcclusionCheckBox->setEnabled( true );

    cast->Delete();
}


void QExperimental3DExtension::probabilisticAmbientOcclusionTangentSphereVariance()
{
    vtkImageCast *cast = vtkImageCast::New();
    cast->SetInput( m_volume->getImage() );
    cast->SetOutputScalarTypeToFloat();
    cast->Update();

#ifndef CUDA_AVAILABLE
    QMessageBox::information( this, tr("Operation only available with CUDA"), "The PAOTS variance is only implemented in CUDA. Compile with CUDA support to use it." );
#else // CUDA_AVAILABLE
    m_probabilisticAmbientOcclusion = cfProbabilisticAmbientOcclusionTangentSphereVariance( cast->GetOutput(), m_probabilisticAmbientOcclusionRadiusSpinBox->value() );
#ifndef QT_NO_DEBUG
    int size = m_volume->getSize();
    for ( int i = 0; i < size; i++ )
    {
        if ( m_probabilisticAmbientOcclusion.at( i ) < 0.0f || m_probabilisticAmbientOcclusion.at( i ) > 1.0f )
        {
            DEBUG_LOG( QString( "paots[%1] = %2" ).arg( i ).arg( m_probabilisticAmbientOcclusion.at( i ) ) );
        }
    }
#endif // QT_NO_DEBUG
#endif // CUDA_AVAILABLE

    m_baseProbabilisticAmbientOcclusionRadioButton->setEnabled( true );
    m_probabilisticAmbientOcclusionCheckBox->setEnabled( true );
    m_opacityProbabilisticAmbientOcclusionCheckBox->setEnabled( true );

    cast->Delete();
}


void QExperimental3DExtension::probabilisticAmbientOcclusionTangentSphereCdf()
{
    vtkImageCast *cast = vtkImageCast::New();
    cast->SetInput( m_volume->getImage() );
    cast->SetOutputScalarTypeToFloat();
    cast->Update();

#ifndef CUDA_AVAILABLE
    QMessageBox::information( this, tr("Operation only available with CUDA"), "The PAOTS cdf is only implemented in CUDA. Compile with CUDA support to use it." );
#else // CUDA_AVAILABLE
    m_probabilisticAmbientOcclusion = cfProbabilisticAmbientOcclusionTangentSphereCdf( cast->GetOutput(), m_probabilisticAmbientOcclusionRadiusSpinBox->value() );
#ifndef QT_NO_DEBUG
    int size = m_volume->getSize();
    for ( int i = 0; i < size; i++ )
    {
        if ( m_probabilisticAmbientOcclusion.at( i ) < 0.0f || m_probabilisticAmbientOcclusion.at( i ) > 1.0f )
        {
            DEBUG_LOG( QString( "paots[%1] = %2" ).arg( i ).arg( m_probabilisticAmbientOcclusion.at( i ) ) );
        }
    }
#endif // QT_NO_DEBUG
#endif // CUDA_AVAILABLE

    m_baseProbabilisticAmbientOcclusionRadioButton->setEnabled( true );
    m_probabilisticAmbientOcclusionCheckBox->setEnabled( true );
    m_opacityProbabilisticAmbientOcclusionCheckBox->setEnabled( true );

    cast->Delete();
}


void QExperimental3DExtension::probabilisticAmbientOcclusionTangentSphereGaussian()
{
    vtkImageCast *cast = vtkImageCast::New();
    cast->SetInput( m_volume->getImage() );
    cast->SetOutputScalarTypeToFloat();
    cast->Update();

#ifndef CUDA_AVAILABLE
    QMessageBox::information( this, tr("Operation only available with CUDA"), "The PAOTS Gaussian is only implemented in CUDA. Compile with CUDA support to use it." );
#else // CUDA_AVAILABLE
    m_probabilisticAmbientOcclusion = cfProbabilisticAmbientOcclusionTangentSphereGaussian( cast->GetOutput(), m_probabilisticAmbientOcclusionRadiusSpinBox->value() );
#ifndef QT_NO_DEBUG
    int size = m_volume->getSize();
    for ( int i = 0; i < size; i++ )
    {
        if ( m_probabilisticAmbientOcclusion.at( i ) < 0.0f || m_probabilisticAmbientOcclusion.at( i ) > 1.0f )
        {
            DEBUG_LOG( QString( "paots[%1] = %2" ).arg( i ).arg( m_probabilisticAmbientOcclusion.at( i ) ) );
        }
    }
#endif // QT_NO_DEBUG
#endif // CUDA_AVAILABLE

    m_baseProbabilisticAmbientOcclusionRadioButton->setEnabled( true );
    m_probabilisticAmbientOcclusionCheckBox->setEnabled( true );
    m_opacityProbabilisticAmbientOcclusionCheckBox->setEnabled( true );

    cast->Delete();
}


void QExperimental3DExtension::probabilisticAmbientOcclusionGradient()
{
    m_voxelSaliencies = m_volume->computeVomiGradient( m_probabilisticAmbientOcclusion );
    m_maximumSaliency = 1.0f;
    m_baseVoxelSalienciesRadioButton->setEnabled( true );
    m_opacityLabel->setEnabled( true );
    m_opacitySaliencyCheckBox->setEnabled( true );
}


void QExperimental3DExtension::volumeVariance()
{
    vtkImageCast *cast = vtkImageCast::New();
    cast->SetInput( m_volume->getImage() );
    cast->SetOutputScalarTypeToFloat();
    cast->Update();

#ifndef CUDA_AVAILABLE
    QMessageBox::information( this, tr("Operation only available with CUDA"), "The volume variance is only implemented in CUDA. Compile with CUDA support to use it." );
#else // CUDA_AVAILABLE
    m_volumeVariance = cfVolumeVariance( cast->GetOutput(), m_probabilisticAmbientOcclusionRadiusSpinBox->value() );
#ifndef QT_NO_DEBUG
    int size = m_volume->getSize();
    for ( int i = 0; i < size; i++ )
    {
        if ( m_volumeVariance.at( i ) < 0.0f )
        {
            DEBUG_LOG( QString( "vv[%1] = %2" ).arg( i ).arg( m_volumeVariance.at( i ) ) );
        }
    }
#endif // QT_NO_DEBUG
#endif // CUDA_AVAILABLE

    m_opacityVarianceCheckBox->setEnabled( true );

    cast->Delete();
}


} // namespace udg
