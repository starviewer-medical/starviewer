#include "qexperimental3dextension.h"

#include <QButtonGroup>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QSet>
#include <QSettings>
#include <QStringListModel>
#include <QTemporaryFile>
#include <QTextStream>

#include "experimental3dvolume.h"
#include "informationtheory.h"
#include "logging.h"
#include "mathtools.h"
#include "obscurancemainthread.h"
#include "transferfunctionio.h"
#include "vector3.h"
#include "viewpointgenerator.h"
#include "viewpointinformationchannel.h"
#include "volumereslicer.h"


namespace udg {


QExperimental3DExtension::QExperimental3DExtension( QWidget *parent )
 : QWidget( parent ), m_volume( 0 ),
   m_computingObscurance( false ), m_obscuranceMainThread( 0 ), m_obscurance( 0 ), m_interactive( true )
{
    setupUi( this );

    createConnections();

    QButtonGroup *bestViewpointsRadioButtons = new QButtonGroup( this );
    bestViewpointsRadioButtons->addButton( m_computeBestViewsNRadioButton );
    bestViewpointsRadioButtons->addButton( m_computeBestViewsThresholdRadioButton );

    m_colorVomiPalette << Vector3Float( 1.0f, 1.0f, 1.0f );

    m_recentTransferFunctionsModel = new QStringListModel( this );
    m_recentTransferFunctionsListView->setModel( m_recentTransferFunctionsModel );
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
    connect( m_baseVoxelSalienciesRadioButton, SIGNAL( toggled(bool) ), m_baseVoxelSalienciesFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_baseVoxelSalienciesRadioButton, SIGNAL( toggled(bool) ), m_baseVoxelSalienciesFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
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
    connect( m_computeVmiPushButton, SIGNAL( clicked() ), SLOT( computeSelectedVmi() ) );
    connect( m_loadViewpointEntropyPushButton, SIGNAL( clicked() ), SLOT( loadViewpointEntropy() ) );
    connect( m_saveViewpointEntropyPushButton, SIGNAL( clicked() ), SLOT( saveViewpointEntropy() ) );
    connect( m_loadEntropyPushButton, SIGNAL( clicked() ), SLOT( loadEntropy() ) );
    connect( m_saveEntropyPushButton, SIGNAL( clicked() ), SLOT( saveEntropy() ) );
    connect( m_loadVmiPushButton, SIGNAL( clicked() ), SLOT( loadVmi() ) );
    connect( m_saveVmiPushButton, SIGNAL( clicked() ), SLOT( saveVmi() ) );
    connect( m_loadMiPushButton, SIGNAL( clicked() ), SLOT( loadMi() ) );
    connect( m_saveMiPushButton, SIGNAL( clicked() ), SLOT( saveMi() ) );
    connect( m_loadVomiPushButton, SIGNAL( clicked() ), SLOT( loadVomi() ) );
    connect( m_saveVomiPushButton, SIGNAL( clicked() ), SLOT( saveVomi() ) );
    connect( m_loadViewpointVomiPushButton, SIGNAL( clicked() ), SLOT( loadViewpointVomi() ) );
    connect( m_saveViewpointVomiPushButton, SIGNAL( clicked() ), SLOT( saveViewpointVomi() ) );
    connect( m_loadColorVomiPalettePushButton, SIGNAL( clicked() ), SLOT( loadColorVomiPalette() ) );
    connect( m_loadColorVomiPushButton, SIGNAL( clicked() ), SLOT( loadColorVomi() ) );
    connect( m_saveColorVomiPushButton, SIGNAL( clicked() ), SLOT( saveColorVomi() ) );
    connect( m_loadViewpointUnstabilitiesPushButton, SIGNAL( clicked() ), SLOT( loadViewpointUnstabilities() ) );
    connect( m_saveViewpointUnstabilitiesPushButton, SIGNAL( clicked() ), SLOT( saveViewpointUnstabilities() ) );
    connect( m_loadBestViewsPushButton, SIGNAL( clicked() ), SLOT( loadBestViews() ) );
    connect( m_saveBestViewsPushButton, SIGNAL( clicked() ), SLOT( saveBestViews() ) );
    connect( m_loadGuidedTourPushButton, SIGNAL( clicked() ), SLOT( loadGuidedTour() ) );
    connect( m_saveGuidedTourPushButton, SIGNAL( clicked() ), SLOT( saveGuidedTour() ) );
    connect( m_loadEvmiPushButton, SIGNAL( clicked() ), SLOT( loadEvmi() ) );
    connect( m_saveEvmiPushButton, SIGNAL( clicked() ), SLOT( saveEvmi() ) );
    connect( m_tourBestViewsPushButton, SIGNAL( clicked() ), SLOT( tourBestViews() ) );
    connect( m_guidedTourPushButton, SIGNAL( clicked() ), SLOT( guidedTour() ) );
    connect( m_vomiGradientPushButton, SIGNAL( clicked() ), SLOT( computeVomiGradient() ) );

    // Program
    connect( m_loadAndRunProgramPushButton, SIGNAL( clicked() ), SLOT( loadAndRunProgram() ) );
}


void QExperimental3DExtension::loadTransferFunction()
{
    QString transferFunctionFileName = getFileNameToLoad( "transferFunctionDir", tr("Load transfer function"), tr("XML files (*.xml);;Transfer function files (*.tf);;All files (*)") );
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
    QString transferFunctionFileName = getFileNameToSave( "transferFunctionDir", tr("Save transfer function"), tr("XML files (*.xml);;Transfer function files (*.tf);;All files (*)"), "xml" );

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

    const double ALMOST_1 = 0.9;

    int *dimensions = m_volume->getImage()->GetDimensions();
    int maxDimension = qMax( qMax( dimensions[0], dimensions[1] ), dimensions[2] );
    double maxDistance = speedFactor * maxDimension / 4.0;

    DEBUG_LOG( "Tour:" );

    Vector3 previousPoint = viewpoints.at( 0 );
    DEBUG_LOG( previousPoint.toString() );

    Vector3 currentPoint = previousPoint;
    setViewpoint( currentPoint );

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
    else if ( m_baseVomiRadioButton->isChecked() ) m_volume->addVomi( m_vomi, m_maximumVomi, m_baseVomiFactorDoubleSpinBox->value() );
    else if ( m_baseVomiCoolWarmRadioButton->isChecked() ) m_volume->addVomiCoolWarm( m_vomi, m_maximumVomi, m_baseVomiCoolWarmFactorDoubleSpinBox->value(),
                                                                                      m_baseVomiCoolWarmYDoubleSpinBox->value(), m_baseVomiCoolWarmBDoubleSpinBox->value() );
    else if ( m_baseColorVomiRadioButton->isChecked() ) m_volume->addColorVomi( m_colorVomi, m_maximumColorVomi, m_baseColorVomiFactorDoubleSpinBox->value() );
    else if ( m_baseVoxelSalienciesRadioButton->isChecked() ) m_volume->addVoxelSaliencies( m_voxelSaliencies, m_maximumSaliency, m_baseVoxelSalienciesFactorDoubleSpinBox->value() );

    if ( m_contourCheckBox->isChecked() ) m_volume->addContour( m_contourDoubleSpinBox->value() );
    if ( m_obscuranceCheckBox->isChecked() ) m_volume->addObscurance( m_obscurance, m_obscuranceFactorDoubleSpinBox->value(), m_obscuranceLowFilterDoubleSpinBox->value(), m_obscuranceHighFilterDoubleSpinBox->value() );
    if ( m_vomiCheckBox->isChecked() ) m_volume->addVomi( m_vomi, m_maximumVomi, m_vomiFactorDoubleSpinBox->value() );
    if ( m_vomiCoolWarmCheckBox->isChecked() ) m_volume->addVomiCoolWarm( m_vomi, m_maximumVomi, m_vomiCoolWarmFactorDoubleSpinBox->value(),
                                                                          m_vomiCoolWarmYDoubleSpinBox->value(), m_vomiCoolWarmBDoubleSpinBox->value() );
    if ( m_colorVomiCheckBox->isChecked() ) m_volume->addColorVomi( m_colorVomi, m_maximumColorVomi, m_colorVomiFactorDoubleSpinBox->value() );
    if ( m_opacityVomiCheckBox->isChecked() ) m_volume->addOpacity( m_vomi, m_maximumVomi, m_opacityLowThresholdDoubleSpinBox->value(), m_opacityLowFactorDoubleSpinBox->value(),
                                                                                           m_opacityHighThresholdDoubleSpinBox->value(), m_opacityHighFactorDoubleSpinBox->value() );
    if ( m_opacitySaliencyCheckBox->isChecked() ) m_volume->addOpacity( m_voxelSaliencies, m_maximumSaliency, m_opacityLowThresholdDoubleSpinBox->value(), m_opacityLowFactorDoubleSpinBox->value(),
                                                                                                              m_opacityHighThresholdDoubleSpinBox->value(), m_opacityHighFactorDoubleSpinBox->value() );
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
    QString cameraFileName = getFileNameToLoad( "cameraDir", tr("Load camera parameters"), tr("Camera files (*.cam);;All files (*)") );
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
    QString cameraFileName = getFileNameToSave( "cameraDir", tr("Save camera parameters"), tr("Camera files (*.cam);;All files (*)"), "cam" );

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
    /*
    // Describe the new vector between the camera and the target
    dx = cameraX - targetX;
    dy = cameraY - targetY;
    dz = cameraZ - targetZ;

    // Calculate the new "up" vector for the camera
    upX = -dx * dy;
    upY =  dz * dz + dx * dx;
    upZ = -dz * dy;
    */
    //Vector3 up( -viewpoint.x * viewpoint.y, viewpoint.z * viewpoint.z + viewpoint.x * viewpoint.x, -viewpoint.z * viewpoint.y );
    //Vector3 up( viewpoint.x * viewpoint.y + viewpoint.z * viewpoint.z, viewpoint.y * viewpoint.z + viewpoint.x * viewpoint.x, viewpoint.z * viewpoint.x + viewpoint.y * viewpoint.y );
    //Vector3 up( 0.0, 2.0 * qAbs( viewpoint.x ) + qAbs( viewpoint.z  ), qAbs( viewpoint.y ) );
    double max = qMax( qAbs( viewpoint.x ), qMax( qAbs( viewpoint.y ), qAbs( viewpoint.z ) ) );
    //Vector3 up( max - qAbs( viewpoint.x ), 2.0 * qAbs( viewpoint.x ) + qAbs( viewpoint.z  ), qAbs( viewpoint.y ) );
    Vector3 up( qMax( max - qAbs( viewpoint.y - viewpoint.z ), 0.0 ), qAbs( viewpoint.x ) + qAbs( viewpoint.z  ), qAbs( viewpoint.y ) );
    up.normalize();

    /*
    Vector3 up( 0.0, 1.0, 0.0 );
    Vector3 position = viewpoint;
    double dotProduct = qAbs( position.normalize() * up );

    if ( dotProduct > 0.9 ) up.set( 0.0, 0.0, 1.0 );
    else if ( dotProduct > 0.8 )
    {
        double a = 10.0 * ( dotProduct - 0.8 ); // a està entre 0 i 1; 1 ha de ser tot z, 0 ha de ser tot y
        up.set( 0.0, 1.0 - a, a );
        up.normalize();

        if ( qAbs( position.x ) < 0.1 && MathTools::haveSameSign( position.y, position.z ) )    // si x és propera a 0 i y i z tenen el mateix signe ens va malament
        {
            // fem el nou producte escalar per assegurar que vagi bé
            dotProduct = qAbs( position * up ); // position ja es manté normalitzada d'abans

            if ( dotProduct > 0.7 )
            {
                double b = ( dotProduct - 0.7 ) / 0.3;  // b està entre 0 i 1
                double c = 1.0 - ( 10.0 * qAbs( position.x ) ); // c està entre 0 (quan x és 0.1) i 1 (quan x és 0)
                up.set( b * c, 1.0 - a, a );
                up.normalize();
            }
        }
    }
    */

    //DEBUG_LOG( "viewpoint = " + viewpoint.toString() + ", up = " + up.toString() );
    m_viewer->setCamera( viewpoint, Vector3(), up );
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

        if ( m_obscuranceCheckBox->isChecked() )
        {
            m_obscuranceCheckBox->setChecked( false );
            render();
        }

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
    QString obscuranceFileName = getFileNameToLoad( "obscuranceDir", tr("Load obscurance"), tr("Data files (*.dat);;All files (*)") );

    if ( !obscuranceFileName.isNull() )
    {
        if ( m_obscuranceCheckBox->isChecked() )
        {
            m_obscuranceCheckBox->setChecked( false );
            render();
        }

        m_obscuranceCheckBox->setEnabled( false );

        delete m_obscurance;

        m_obscurance = new Obscurance( m_volume->getSize(), ObscuranceMainThread::hasColor( static_cast<ObscuranceMainThread::Variant>( m_obscuranceVariantComboBox->currentIndex() ) ),
                                       m_obscuranceDoublePrecisionRadioButton->isChecked() );
        bool ok = m_obscurance->load( obscuranceFileName );

        if ( ok )
        {
            m_obscuranceSavePushButton->setEnabled( true );
            m_obscuranceCheckBox->setEnabled( true );
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
    QString obscuranceFileName = getFileNameToSave( "obscuranceDir", tr("Save obscurance"), tr("Data files (*.dat);;All files (*)"), "dat" );

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
    bool computeVomi = m_computeVomiCheckBox->isChecked();
    bool computeViewpointVomi = m_computeViewpointVomiCheckBox->isChecked();
    bool computeColorVomi = m_computeColorVomiCheckBox->isChecked();

    // Si no hi ha res a calcular marxem
    if ( !computeViewpointEntropy && !computeEntropy && !computeVmi && !computeMi && !computeVomi && !computeViewpointVomi && !computeColorVomi ) return;

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

    // Filtratge de punts de vista
    if ( !m_tourLineEdit->text().isEmpty() )
    {
        int nViewpoints = m_vmiViewpointDistributionWidget->numberOfViewpoints();
        int selectedViewpoint = m_tourLineEdit->text().toInt() - 1;

        if ( selectedViewpoint >= 0 && selectedViewpoint < nViewpoints )
        {
            QVector<bool> filter( nViewpoints );

            filter[selectedViewpoint] = true;

            QVector<int> neighbours = viewpointGenerator.neighbours( selectedViewpoint );
            for ( int i = 0; i < neighbours.size(); i++ ) filter[neighbours.at( i )] = true;

            viewpointInformationChannel.filterViewpoints( filter );
        }
    }

    connect( &viewpointInformationChannel, SIGNAL( totalProgressMaximum(int) ), m_vmiTotalProgressBar, SLOT( setMaximum(int) ) );
    connect( &viewpointInformationChannel, SIGNAL( totalProgressMaximum(int) ), m_vmiTotalProgressBar, SLOT( repaint() ) ); // no sé per què però cal això perquè s'actualitzi quan toca
    connect( &viewpointInformationChannel, SIGNAL( totalProgress(int) ), m_vmiTotalProgressBar, SLOT( setValue(int) ) );
    connect( &viewpointInformationChannel, SIGNAL( partialProgress(int) ), m_vmiProgressBar, SLOT( setValue(int) ) );

    viewpointInformationChannel.compute( computeViewpointEntropy, computeEntropy, computeVmi, computeMi, computeVomi, computeViewpointVomi, computeColorVomi );

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

    // Restaurem els paràmetres normals (en realitat només cal si es fa amb CPU)
    render();
    m_viewer->setCamera( position, focus, up );

    setCursor( QCursor( Qt::ArrowCursor ) );
}


void QExperimental3DExtension::computeSelectedVmiOld()
{
    // Què ha demanat l'usuari
    bool computeViewpointUnstabilities = m_computeViewpointUnstabilitiesCheckBox->isChecked();
    bool computeBestViews = m_computeBestViewsCheckBox->isChecked();
    bool computeGuidedTour = m_computeGuidedTourCheckBox->isChecked();
    bool computeEvmi = m_computeEvmiCheckBox->isChecked();

    // Si no hi ha res a calcular marxem
    if ( !computeViewpointUnstabilities && !computeBestViews && !computeGuidedTour && !computeEvmi ) return;

    setCursor( QCursor( Qt::WaitCursor ) );

    // Guardem la càmera
    Vector3 position, focus, up;
    m_viewer->getCamera( position, focus, up );

    // Obtenir direccions
    float distance = ( position - focus ).length();
    ViewpointGenerator viewpointGenerator = m_vmiViewpointDistributionWidget->viewpointGenerator( distance );
    QVector<Vector3> viewpoints = viewpointGenerator.viewpoints();
    int nViewpoints = viewpoints.size();

    if ( !m_tourLineEdit->text().isEmpty() )
    {
        int selectedViewpoint = m_tourLineEdit->text().toInt() - 1;
        if ( selectedViewpoint >= 0 && selectedViewpoint < nViewpoints )
        {
            viewpoints.clear();
            viewpoints << viewpointGenerator.viewpoint( selectedViewpoint );
            QVector<int> neighbours = viewpointGenerator.neighbours( selectedViewpoint );
            for ( int i = 0; i < neighbours.size(); i++ ) viewpoints << viewpointGenerator.viewpoint( neighbours.at( i ) );
            nViewpoints = viewpoints.size();
        }
    }

    // Dependències
    if ( computeGuidedTour && m_bestViews.isEmpty() ) computeBestViews = true;
    //if ( computeBestViews && m_vmi.size() != nViewpoints ) computeVmi = true;
    //if ( computeViewpointVomi && m_vomi.isEmpty() ) computeVomi = true;
    //if ( computeEvmi && m_vomi.isEmpty() ) computeVomi = true;

    // Inicialitzar progrés
    int nSteps = 3; // ray casting (p(O|V)), p(V), p(O)
    if ( computeViewpointUnstabilities || computeEvmi ) nSteps++;  // viewpoint unstabilities + EVMI
    if ( computeBestViews ) nSteps++;   // best views
    if ( computeGuidedTour ) nSteps++;  // guided tour
    int step = 0;
    {
        m_vmiProgressBar->setValue( 0 );
        m_vmiProgressBar->repaint();
        m_vmiTotalProgressBar->setMaximum( nSteps );
        m_vmiTotalProgressBar->setValue( step );
        m_vmiTotalProgressBar->repaint();
    }

    QVector<float> viewProbabilities( nViewpoints );                                            // vector p(V), inicialitzat a 0
    QVector<float> objectProbabilities;                                                         // vector p(O)
    QVector<QTemporaryFile*> pOvFiles = createObjectProbabilitiesPerViewFiles( nViewpoints );   // matriu p(O|V) (cada fitxer una fila p(O|v))

    float totalViewedVolume;

    // p(O|V) (i acumulació de p(V))
    {
        totalViewedVolume = vmiRayCasting( viewpoints, pOvFiles, viewProbabilities );
        m_vmiTotalProgressBar->setValue( ++step );
        m_vmiTotalProgressBar->repaint();
    }

    // p(V)
    {
        normalizeViewProbabilities( viewProbabilities, totalViewedVolume );
        m_vmiTotalProgressBar->setValue( ++step );
        m_vmiTotalProgressBar->repaint();
    }

    // p(O)
    {
        objectProbabilities = getObjectProbabilities( viewProbabilities, pOvFiles );
        m_vmiTotalProgressBar->setValue( ++step );
        m_vmiTotalProgressBar->repaint();
    }

    // viewpont unstabilities + EVMI
    if ( computeViewpointUnstabilities || computeEvmi )
    {
        computeVmiRelatedMeasures( viewpointGenerator, viewProbabilities, objectProbabilities, pOvFiles, computeViewpointUnstabilities, computeEvmi );
        m_vmiTotalProgressBar->setValue( ++step );
        m_vmiTotalProgressBar->repaint();
    }

    // best views
    if ( computeBestViews )
    {
        this->computeBestViews( viewpoints, viewProbabilities, objectProbabilities, pOvFiles );
        m_vmiTotalProgressBar->setValue( ++step );
        m_vmiTotalProgressBar->repaint();
    }

    // guided tour
    if ( computeGuidedTour )
    {
        this->computeGuidedTour( viewpointGenerator, viewProbabilities, pOvFiles );
        m_vmiTotalProgressBar->setValue( ++step );
        m_vmiTotalProgressBar->repaint();
    }

    deleteObjectProbabilitiesPerViewFiles( pOvFiles );

    render();

    // Restaurem la càmera
    m_viewer->setCamera( position, focus, up );

    DEBUG_LOG( "fi" );

    setCursor( QCursor( Qt::ArrowCursor ) );
}


QVector<QTemporaryFile*> QExperimental3DExtension::createObjectProbabilitiesPerViewFiles( int nViewpoints )
{
    DEBUG_LOG( "creem els fitxers temporals" );

    QVector<QTemporaryFile*> pOvFiles( nViewpoints );   // matriu p(O|V) (cada fitxer una fila p(O|v))

    for ( int i = 0; i < nViewpoints; i++ )
    {
        pOvFiles[i] = new QTemporaryFile( "pOvXXXXXX.tmp" );    // els fitxers temporals es creen al directori de treball

        if ( !pOvFiles[i]->open() )
        {
            DEBUG_LOG( QString( "No s'ha pogut obrir el fitxer: error %1" ).arg( pOvFiles[i]->errorString() ) );
            for ( int j = 0; j < i; j++ ) pOvFiles[j]->close();
            pOvFiles.clear();   // retornarm un vector buit si hi ha hagut problemes
            break;
        }
    }

    return pOvFiles;
}


void QExperimental3DExtension::deleteObjectProbabilitiesPerViewFiles( QVector<QTemporaryFile*> &files )
{
    DEBUG_LOG( "destruïm els fitxers temporals" );

    for ( int i = 0; i < files.size(); i++ )
    {
        files.at( i )->close();
        delete files.at( i );
    }

    files.clear();
}


float QExperimental3DExtension::vmiRayCasting( const QVector<Vector3> &viewpoints, const QVector<QTemporaryFile*> &pOvFiles, QVector<float> &viewedVolumePerView )
{
    int nViewpoints = viewpoints.size();
    float totalViewedVolume = 0.0f;

    m_vmiProgressBar->setValue( 0 );
    m_vmiProgressBar->repaint();

    m_volume->startVmiMode();

    for ( int i = 0; i < nViewpoints; i++ )
    {
        m_volume->startVmiSecondPass();
        setViewpoint( viewpoints.at( i ) ); // render
        QVector<float> objectProbabilitiesInView = m_volume->finishVmiSecondPass(); // p(O|v)

        // p(V)
        float viewedVolume = m_volume->viewedVolumeInVmiSecondPass();
        viewedVolumePerView[i] = viewedVolume;
        totalViewedVolume += viewedVolume;

        // p(O|V)
        pOvFiles.at( i )->write( reinterpret_cast<const char*>( objectProbabilitiesInView.data() ), objectProbabilitiesInView.size() * sizeof(float) );

        m_vmiProgressBar->setValue( 100 * ( i + 1 ) / nViewpoints );
        m_vmiProgressBar->repaint();
    }

    return totalViewedVolume;
}


void QExperimental3DExtension::normalizeViewProbabilities( QVector<float> &viewProbabilities, float totalViewedVolume )
{
    if ( totalViewedVolume > 0.0f )
    {
        int nViewpoints = viewProbabilities.size();

        m_vmiProgressBar->setValue( 0 );

        for ( int i = 0; i < nViewpoints; i++ )
        {
            viewProbabilities[i] /= totalViewedVolume;
            Q_ASSERT( viewProbabilities.at( i ) == viewProbabilities.at( i ) );
            DEBUG_LOG( QString( "p(v%1) = %2" ).arg( i + 1 ).arg( viewProbabilities.at( i ) ) );
            m_vmiProgressBar->setValue( 100 * ( i + 1 ) / nViewpoints );
            m_vmiProgressBar->repaint();
        }
    }
}


QVector<float> QExperimental3DExtension::getObjectProbabilities( const QVector<float> &viewProbabilities, const QVector<QTemporaryFile*> &pOvFiles )
{
    class POThread : public QThread {
        public:
            POThread( QVector<float> &objectProbabilities, float *objectProbabilitiesInView, int start, int end )
                : m_viewProbability( 0.0f ), m_objectProbabilities( objectProbabilities ), m_objectProbabilitiesInView( objectProbabilitiesInView ), m_start( start ), m_end( end )
            {
            }
            void setViewProbability( float viewProbability )
            {
                m_viewProbability = viewProbability;
            }
            virtual void run()
            {
                for ( int i = m_start; i < m_end; i++ ) m_objectProbabilities[i] += m_viewProbability * m_objectProbabilitiesInView[i];
            }
        private:
            float m_viewProbability;
            QVector<float> &m_objectProbabilities;
            float *m_objectProbabilitiesInView;
            int m_start, m_end;
    };

    int nViewpoints = viewProbabilities.size();
    unsigned int nObjects = m_volume->getSize();
    QVector<float> objectProbabilities( nObjects ); // vector p(O), inicialitzat a 0
    float *objectProbabilitiesInView = new float[nObjects]; // vector p(O|v)

    m_vmiProgressBar->setValue( 0 );

    int nThreads = QThread::idealThreadCount();
    POThread **poThreads = new POThread*[nThreads];
    int nObjectsPerThread = nObjects / nThreads + 1;
    int start = 0, end = nObjectsPerThread;

    for ( int i = 0; i < nThreads; i++ )
    {
        poThreads[i] = new POThread( objectProbabilities, objectProbabilitiesInView, start, end );
        start += nObjectsPerThread;
        end += nObjectsPerThread;
        if ( end > static_cast<int>( nObjects ) ) end = nObjects;
    }

    for ( int i = 0; i < nViewpoints; i++ )
    {
        pOvFiles[i]->reset();   // reset per tornar al principi
        pOvFiles[i]->read( reinterpret_cast<char*>( objectProbabilitiesInView ), nObjects * sizeof(float) );    // llegim...
        pOvFiles[i]->reset();   // ... i després fem un reset per tornar al principi i buidar el buffer (amb un peek queda el buffer ple, i es gasta molta memòria)

        //for ( unsigned int j = 0; j < nObjects; j++ ) objectProbabilities[j] += viewProbabilities.at( i ) * objectProbabilitiesInView[j]; // vell

        for ( int j = 0; j < nThreads; j++ )
        {
            poThreads[j]->setViewProbability( viewProbabilities.at( i ) );
            poThreads[j]->start();
        }

        for ( int j = 0; j < nThreads; j++ ) poThreads[j]->wait();

        m_vmiProgressBar->setValue( 100 * ( i + 1 ) / nViewpoints );
        m_vmiProgressBar->repaint();
    }

    for ( int i = 0; i < nThreads; i++ ) delete poThreads[i];
    delete[] poThreads;

    delete[] objectProbabilitiesInView;

#ifndef QT_NO_DEBUG
    for ( unsigned int j = 0; j < nObjects; j++ ) Q_ASSERT( objectProbabilities.at( j ) == objectProbabilities.at( j ) );
#endif

    return objectProbabilities;
}


void QExperimental3DExtension::computeVmiRelatedMeasures( const ViewpointGenerator &viewpointGenerator, const QVector<float> &viewProbabilities, const QVector<float> &objectProbabilities,
                                                          const QVector<QTemporaryFile*> &pOvFiles, bool computeViewpointUnstabilities, bool computeEvmi )
{
    int nViewpoints = viewProbabilities.size();
    int nObjects = objectProbabilities.size();

    if ( computeViewpointUnstabilities ) m_viewpointUnstabilities.resize( nViewpoints );
    if ( computeEvmi ) m_evmi.resize( nViewpoints );

    QVector<float> ppO; // p'(O)

    if ( computeEvmi )
    {
        ppO.resize( nObjects );

        float total = 0.0f;

        for ( int i = 0; i < nObjects; i++ )
        {

            //ppO[i] = objectProbabilities.at( i ) * m_vomi.at( i );
            ppO[i] = objectProbabilities.at( i ) * qMax( m_vomi.at( i ), 0.01f * m_maximumSaliency );   // prova per evitar infinits a EVMI
            total += ppO.at( i );
        }

        for ( int i = 0; i < nObjects; i++ ) ppO[i] /= total;
    }

    m_vmiProgressBar->setValue( 0 );

    QVector<float> objectProbabilitiesInView( nObjects );   // vector p(O|vi)

    for ( int i = 0; i < nViewpoints; i++ )
    {
        pOvFiles[i]->reset();   // reset per tornar al principi
        pOvFiles[i]->read( reinterpret_cast<char*>( objectProbabilitiesInView.data() ), nObjects * sizeof(float) ); // llegim...
        pOvFiles[i]->reset();   // ... i després fem un reset per tornar al principi i buidar el buffer (amb un peek queda el buffer ple, i es gasta molta memòria)

        if ( computeViewpointUnstabilities )
        {
            float pvi = viewProbabilities.at( i );  // p(vi)

            QVector<int> neighbours = viewpointGenerator.neighbours( i );
            int nNeighbours = neighbours.size();
            float viewpointUnstability = 0.0f;
            QVector<float> objectProbabilitiesInNeighbour( nObjects );  // vector p(O|vj)

            for ( int j = 0; j < nNeighbours; j++ )
            {
                int neighbour = neighbours.at( j );
                float pvj = viewProbabilities.at( neighbour );  // p(vj)
                float pvij = pvi + pvj; // p(v̂)

                if ( pvij == 0.0f ) continue;

                pOvFiles[neighbour]->reset();   // reset per tornar al principi
                pOvFiles[neighbour]->read( reinterpret_cast<char*>( objectProbabilitiesInNeighbour.data() ), nObjects * sizeof(float) );    // llegim...
                pOvFiles[neighbour]->reset();   // ... i després fem un reset per tornar al principi i buidar el buffer (amb un peek queda el buffer ple, i es gasta molta memòria)

                float viewpointDissimilarity = InformationTheory<float>::jensenShannonDivergence( pvi / pvij, pvj / pvij, objectProbabilitiesInView, objectProbabilitiesInNeighbour );
                viewpointUnstability += viewpointDissimilarity;
            }

            viewpointUnstability /= nNeighbours;
            m_viewpointUnstabilities[i] = viewpointUnstability;
            DEBUG_LOG( QString( "U(v%1) = %2" ).arg( i + 1 ).arg( viewpointUnstability ) );
        }

        if ( computeEvmi )
        {
            float evmi = InformationTheory<float>::kullbackLeiblerDivergence( objectProbabilitiesInView, ppO );
            Q_ASSERT( evmi == evmi );
            m_evmi[i] = evmi;
            DEBUG_LOG( QString( "EVMI(v%1) = %2" ).arg( i + 1 ).arg( evmi ) );
        }

        m_vmiProgressBar->setValue( 100 * ( i + 1 ) / nViewpoints );
        m_vmiProgressBar->repaint();
    }

    if ( computeViewpointUnstabilities ) m_saveViewpointUnstabilitiesPushButton->setEnabled( true );
    if ( computeEvmi ) m_saveEvmiPushButton->setEnabled( true );
}


void QExperimental3DExtension::computeBestViews( const QVector<Vector3> &viewpoints, const QVector<float> &viewProbabilities, const QVector<float> &objectProbabilities, const QVector<QTemporaryFile*> &pOvFiles )
{
    int nViewpoints = viewpoints.size();
    int nObjects = objectProbabilities.size();

    m_bestViews.clear();

    // millor vista
    float minVmi = m_vmi.at( 0 );
    int minVmiIndex = 0;

    for ( int i = 1; i < nViewpoints; i++ )
    {
        float vmi = m_vmi.at( i );

        if ( vmi < minVmi )
        {
            minVmi = vmi;
            minVmiIndex = i;
        }
    }

    m_bestViews << qMakePair( minVmiIndex, viewpoints.at( minVmiIndex ) );

    QList<int> viewpointIndexList;
    for ( int i = 0; i < nViewpoints; i++ ) viewpointIndexList << i;
    viewpointIndexList.removeAt( minVmiIndex );

    float pvv = viewProbabilities.at( minVmiIndex );    // p(v̂)
    QVector<float> pOvv( nObjects );    // vector p(O|v̂)
    pOvFiles[minVmiIndex]->reset();     // reset per tornar al principi
    pOvFiles[minVmiIndex]->read( reinterpret_cast<char*>( pOvv.data() ), nObjects * sizeof(float) );    // llegim...
    pOvFiles[minVmiIndex]->reset();     // ... i després fem un reset per tornar al principi i buidar el buffer (amb un peek queda el buffer ple, i es gasta molta memòria)

    // límits
    bool limitN = m_computeBestViewsNRadioButton->isChecked();
    int n = qMin( m_computeBestViewsNSpinBox->value(), nViewpoints );
    bool limitThreshold = m_computeBestViewsThresholdRadioButton->isChecked();
    float threshold = m_computeBestViewsThresholdDoubleSpinBox->value();
    float IVO = 0.0f;       // I(V,O)
    for ( int i = 0; i < nViewpoints; i++ ) IVO += viewProbabilities.at( i ) * m_vmi.at( i );   // calcular I(V,O)
    float IvvO = minVmi;    // I(v̂,O)

    if ( limitN )
    {
        DEBUG_LOG( QString( "límit %1 vistes" ).arg( n ) );
    }
    if ( limitThreshold )
    {
        DEBUG_LOG( QString( "límit llindar %1" ).arg( threshold ) );
    }

    DEBUG_LOG( QString( "I(V,O) = %1" ).arg( IVO ) );
    DEBUG_LOG( "Millors vistes:" );
    DEBUG_LOG( QString( "%1: (v%2) = %3; I(v̂,O) = %4; I(v̂,O)/I(V,O) = %5" ).arg( 0 ).arg( minVmiIndex + 1 ).arg( viewpoints.at( minVmiIndex ).toString() ).arg( IvvO ).arg( IvvO / IVO ) );

    if ( limitN )
    {
        m_vmiProgressBar->setValue( 100 / n );
        m_vmiProgressBar->repaint();
    }

    if ( limitThreshold )
    {
        m_vmiProgressBar->setValue( 0 );
        m_vmiProgressBar->setMaximum( 0 );
        m_vmiProgressBar->repaint();
    }

    m_vmiProgressBar->repaint();

    float *pOvi = new float[nObjects];

    while ( ( limitN && m_bestViews.size() < n ) || ( limitThreshold && IvvO / IVO > threshold ) )
    {
        int nRemainingViews = viewpointIndexList.size();
        float pvvMin = 0.0f;
        QVector<float> pOvvMin;
        float IvvOMin = 0.0f;
        int iMin = 0;

        for ( int i = 0; i < nRemainingViews; i++ )
        {
            int viewIndex = viewpointIndexList.at( i );
            float pvi = viewProbabilities.at( viewIndex );
            pOvFiles[viewIndex]->reset();   // reset per tornar al principi
            pOvFiles[viewIndex]->read( reinterpret_cast<char*>( pOvi ), nObjects * sizeof(float) ); // llegim...
            pOvFiles[viewIndex]->reset();   // ... i després fem un reset per tornar al principi i buidar el buffer (amb un peek queda el buffer ple, i es gasta molta memòria)

            float pvvi = pvv + pvi;         // p(v̂) afegint aquesta vista
            QVector<float> pOvvi( pOvv );   // vector p(O|v̂) afegint aquesta vista
            for ( int j = 0; j < nObjects; j++ ) pOvvi[j] = ( pvv * pOvv.at( j ) + pvi * pOvi[j] ) / pvvi;
            float IvviO = InformationTheory<float>::kullbackLeiblerDivergence( pOvvi, objectProbabilities );    // I(v̂,O) afegint aquesta vista

            if ( i == 0 || IvviO < IvvOMin )
            {
                pvvMin = pvvi;
                pOvvMin = pOvvi;
                IvvOMin = IvviO;
                iMin = i;
            }
        }

        pvv = pvvMin;
        pOvv = pOvvMin;
        IvvO = IvvOMin;
        int viewIndex = viewpointIndexList.takeAt( iMin );
        m_bestViews << qMakePair( viewIndex, viewpoints.at( viewIndex ) );
        DEBUG_LOG( QString( "%1: (v%2) = %3; I(v̂,O) = %4; I(v̂,O)/I(V,O) = %5" ).arg( m_bestViews.size() - 1 ).arg( viewIndex + 1 ).arg( viewpoints.at( viewIndex ).toString() ).arg( IvvO ).arg( IvvO / IVO ) );

        if ( limitN ) m_vmiProgressBar->setValue( 100 * m_bestViews.size() / n );
        m_vmiProgressBar->repaint();
    }

    delete[] pOvi;

    if ( limitThreshold )
    {
        m_vmiProgressBar->setMaximum( 100 );
        m_vmiProgressBar->setValue( 100 );
        m_vmiProgressBar->repaint();
    }

    m_saveBestViewsPushButton->setEnabled( true );
    m_tourBestViewsPushButton->setEnabled( true );
}


void QExperimental3DExtension::computeGuidedTour( const ViewpointGenerator &viewpointGenerator, const QVector<float> &viewProbabilities, const QVector<QTemporaryFile*> &pOvFiles )
{
    const QVector<Vector3> &viewpoints = viewpointGenerator.viewpoints();
    int nViewpoints = viewProbabilities.size();
    unsigned int nObjects = m_volume->getSize();

    DEBUG_LOG( "Guided tour:" );

    m_guidedTour.clear();

    m_vmiProgressBar->setValue( 0 );
    m_vmiProgressBar->repaint();

    QList< QPair<int, Vector3> > bestViews = m_bestViews;   // còpia
    int nBestViews = bestViews.size();

    m_guidedTour << bestViews.takeAt( 0 );
    DEBUG_LOG( QString( "%1: (v%2) = %3" ).arg( 0 ).arg( m_guidedTour.last().first + 1 ).arg( m_guidedTour.last().second.toString() ) );

    m_vmiProgressBar->setValue( 100 / nBestViews );
    m_vmiProgressBar->repaint();

    QSet<int> viewpointIndices;
    for ( int i = 0; i < nViewpoints; i++ ) viewpointIndices << i;

    QVector<float> pOvi( nObjects );    // p(O|vi)
    QVector<float> pOvj( nObjects );    // p(O|vj)

    while ( !bestViews.isEmpty() )
    {
        QPair<int, Vector3> current = m_guidedTour.last();
        int i = current.first;
        float pvi = viewProbabilities.at( i );  // p(vi)

        pOvFiles.at( i )->reset();  // reset per tornar al principi
        pOvFiles.at( i )->read( reinterpret_cast<char*>( pOvi.data() ), nObjects * sizeof(float) ); // llegim...
        pOvFiles.at( i )->reset();  // ... i després fem un reset per tornar al principi i buidar el buffer (amb un peek queda el buffer ple, i es gasta molta memòria)

        int target;
        float minDissimilarity;
        int remainingViews = bestViews.size();

        // trobar el target
        for ( int k = 0; k < remainingViews; k++ )
        {
            int j = bestViews.at( k ).first;
            float pvj = viewProbabilities.at( j );  // p(vj)
            float pvij = pvi + pvj; // p(v̂)

            float dissimilarity;

            if ( pvij == 0.0f ) dissimilarity = 0.0f;
            else
            {
                pOvFiles.at( j )->reset();  // reset per tornar al principi
                pOvFiles.at( j )->read( reinterpret_cast<char*>( pOvj.data() ), nObjects * sizeof(float) ); // llegim...
                pOvFiles.at( j )->reset();  // ... i després fem un reset per tornar al principi i buidar el buffer (amb un peek queda el buffer ple, i es gasta molta memòria)

                dissimilarity = InformationTheory<float>::jensenShannonDivergence( pvi / pvij, pvj / pvij, pOvi, pOvj );
            }

            if ( k == 0 || dissimilarity < minDissimilarity )
            {
                target = k;
                minDissimilarity = dissimilarity;
            }
        }

        // p(vi) i p(O|vi) ara fan referència al target
        pvi = viewProbabilities.at( target );   // p(vi)

        pOvFiles.at( target )->reset(); // reset per tornar al principi
        pOvFiles.at( target )->read( reinterpret_cast<char*>( pOvi.data() ), nObjects * sizeof(float) );    // llegim...
        pOvFiles.at( target )->reset(); // ... i després fem un reset per tornar al principi i buidar el buffer (amb un peek queda el buffer ple, i es gasta molta memòria)

        QSet<int> indices( viewpointIndices );
        int currentIndex = i;

        // camí fins al target
        while ( currentIndex != target )
        {
            indices.remove( currentIndex );

            QVector<int> neighbours = viewpointGenerator.neighbours( currentIndex );
            int nNeighbours = neighbours.size();
            bool test = false;  // per comprovar que sempre tria un veí

            for ( int k = 0; k < nNeighbours; k++ )
            {
                int j = neighbours.at( k );

                if ( !indices.contains( j ) ) continue;

                float pvj = viewProbabilities.at( j );  // p(vj)
                float pvij = pvi + pvj; // p(v̂)

                float dissimilarity;

                if ( pvij == 0.0f ) dissimilarity = 0.0f;
                else
                {
                    pOvFiles.at( j )->reset();  // reset per tornar al principi
                    pOvFiles.at( j )->read( reinterpret_cast<char*>( pOvj.data() ), nObjects * sizeof(float) ); // llegim...
                    pOvFiles.at( j )->reset();  // ... i després fem un reset per tornar al principi i buidar el buffer (amb un peek queda el buffer ple, i es gasta molta memòria)

                    dissimilarity = InformationTheory<float>::jensenShannonDivergence( pvi / pvij, pvj / pvij, pOvi, pOvj );
                }

                if ( k == 0 || dissimilarity < minDissimilarity )
                {
                    currentIndex = j;
                    minDissimilarity = dissimilarity;
                    test = true;
                }
            }

            Q_ASSERT( test );

            m_guidedTour << qMakePair( currentIndex, viewpoints.at( currentIndex ) );
            DEBUG_LOG( QString( "%1: (v%2) = %3; dissimilarity = %4" ).arg( m_guidedTour.size() - 1 ).arg( m_guidedTour.last().first + 1 ).arg( m_guidedTour.last().second.toString() ).arg( minDissimilarity ) );
        }

        bestViews.removeAt( m_guidedTour.last().first );    // esborrem el target de bestViews

        // versió vella
        //m_guidedTour << bestViews.takeAt( target );
        //DEBUG_LOG( QString( "%1: (v%2) = %3; dissimilarity = %4" ).arg( m_guidedTour.size() - 1 ).arg( m_guidedTour.last().first + 1 ).arg( m_guidedTour.last().second.toString() ).arg( minDissimilarity ) );

        m_vmiProgressBar->setValue( 100 * ( nBestViews - bestViews.size() ) / nBestViews );
        m_vmiProgressBar->repaint();
    }

    m_saveGuidedTourPushButton->setEnabled( true );
    m_guidedTourPushButton->setEnabled( true );
}


void QExperimental3DExtension::loadViewpointEntropy()
{
    QString viewpointEntropyFileName = getFileNameToLoad( "viewpointEntropyDir", tr("Load viewpoint entropy"), tr("Data files (*.dat);;All files (*)") );
    if ( !viewpointEntropyFileName.isNull() ) loadViewpointEntropy( viewpointEntropyFileName );
}


void QExperimental3DExtension::loadViewpointEntropy( const QString &fileName )
{
    QFile viewpointEntropyFile( fileName );

    if ( !viewpointEntropyFile.open( QFile::ReadOnly ) )
    {
        DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't load viewpoint entropy"), QString( tr("Can't load viewpoint entropy from file ") ) + fileName );
        return;
    }

    m_viewpointEntropy.clear();

    QDataStream in( &viewpointEntropyFile );

    while ( !in.atEnd() )
    {
        float viewpointEntropy;
        in >> viewpointEntropy;
        m_viewpointEntropy << viewpointEntropy;
    }

    viewpointEntropyFile.close();

    m_saveViewpointEntropyPushButton->setEnabled( true );
}


void QExperimental3DExtension::saveViewpointEntropy()
{
    QString viewpointEntropyFileName = getFileNameToSave( "viewpointEntropyDir", tr("Save viewpoint entropy"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
    if ( !viewpointEntropyFileName.isNull() ) saveViewpointEntropy( viewpointEntropyFileName );
}


void QExperimental3DExtension::saveViewpointEntropy( const QString &fileName )
{
    bool saveAsText = fileName.endsWith( ".txt" );
    QFile viewpointEntropyFile( fileName );
    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
    if ( saveAsText ) mode = mode | QIODevice::Text;

    if ( !viewpointEntropyFile.open( mode ) )
    {
        DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't save viewpoint entropy"), QString( tr("Can't save viewpoint entropy to file ") ) + fileName );
        return;
    }

    int nViewpoints = m_viewpointEntropy.size();

    if ( saveAsText )
    {
        QTextStream out( &viewpointEntropyFile );
        for ( int i = 0; i < nViewpoints; i++ ) out << "H(O|v" << i + 1 << ") = " << m_viewpointEntropy.at( i ) << "\n";
    }
    else
    {
        QDataStream out( &viewpointEntropyFile );
        for ( int i = 0; i < nViewpoints; i++ ) out << m_viewpointEntropy.at( i );
    }

    viewpointEntropyFile.close();
}


void QExperimental3DExtension::loadEntropy()
{
    QString entropyFileName = getFileNameToLoad( "entropyDir", tr("Load entropy"), tr("Data files (*.dat);;All files (*)") );
    if ( !entropyFileName.isNull() ) loadEntropy( entropyFileName );
}


void QExperimental3DExtension::loadEntropy( const QString &fileName )
{
    QFile entropyFile( fileName );

    if ( !entropyFile.open( QFile::ReadOnly ) )
    {
        DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't load entropy"), QString( tr("Can't load entropy from file ") ) + fileName );
        return;
    }

    QDataStream in( &entropyFile );

    if ( !in.atEnd() ) in >> m_entropy;

    entropyFile.close();

    m_saveEntropyPushButton->setEnabled( true );
}


void QExperimental3DExtension::saveEntropy()
{
    QString entropyFileName = getFileNameToSave( "entropyDir", tr("Save entropy"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
    if ( !entropyFileName.isNull() ) saveEntropy( entropyFileName );
}


void QExperimental3DExtension::saveEntropy( const QString &fileName )
{
    bool saveAsText = fileName.endsWith( ".txt" );
    QFile entropyFile( fileName );
    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
    if ( saveAsText ) mode = mode | QIODevice::Text;

    if ( !entropyFile.open( mode ) )
    {
        DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't save entropy"), QString( tr("Can't save entropy to file ") ) + fileName );
        return;
    }

    if ( saveAsText )
    {
        QTextStream out( &entropyFile );
        out << "entropy = " << m_entropy;
    }
    else
    {
        QDataStream out( &entropyFile );
        out << m_entropy;
    }

    entropyFile.close();
}


void QExperimental3DExtension::loadVmi()
{
    QString vmiFileName = getFileNameToLoad( "vmiDir", tr("Load VMI"), tr("Data files (*.dat);;All files (*)") );
    if ( !vmiFileName.isNull() ) loadVmi( vmiFileName );
}


void QExperimental3DExtension::loadVmi( const QString &fileName )
{
    QFile vmiFile( fileName );

    if ( !vmiFile.open( QFile::ReadOnly ) )
    {
        DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't load VMI"), QString( tr("Can't load VMI from file ") ) + fileName );
        return;
    }

    m_vmi.clear();

    QDataStream in( &vmiFile );

    while ( !in.atEnd() )
    {
        float vmi;
        in >> vmi;
        m_vmi << vmi;
    }

    vmiFile.close();

    m_saveVmiPushButton->setEnabled( true );
}


void QExperimental3DExtension::saveVmi()
{
    QString vmiFileName = getFileNameToSave( "vmiDir", tr("Save VMI"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
    if ( !vmiFileName.isNull() ) saveVmi( vmiFileName );
}


void QExperimental3DExtension::saveVmi( const QString &fileName )
{
    bool saveAsText = fileName.endsWith( ".txt" );
    QFile vmiFile( fileName );
    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
    if ( saveAsText ) mode = mode | QIODevice::Text;

    if ( !vmiFile.open( mode ) )
    {
        DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't save VMI"), QString( tr("Can't save VMI to file ") ) + fileName );
        return;
    }

    int nViewpoints = m_vmi.size();

    if ( saveAsText )
    {
        QTextStream out( &vmiFile );
        for ( int i = 0; i < nViewpoints; i++ ) out << "VMI(v" << i + 1 << ") = " << m_vmi.at( i ) << "\n";
    }
    else
    {
        QDataStream out( &vmiFile );
        for ( int i = 0; i < nViewpoints; i++ ) out << m_vmi.at( i );
    }

    vmiFile.close();
}


void QExperimental3DExtension::loadMi()
{
    QString miFileName = getFileNameToLoad( "miDir", tr("Load MI"), tr("Data files (*.dat);;All files (*)") );
    if ( !miFileName.isNull() ) loadMi( miFileName );
}


void QExperimental3DExtension::loadMi( const QString &fileName )
{
    QFile miFile( fileName );

    if ( !miFile.open( QFile::ReadOnly ) )
    {
        DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't load MI"), QString( tr("Can't load MI from file ") ) + fileName );
        return;
    }

    QDataStream in( &miFile );

    if ( !in.atEnd() ) in >> m_mi;

    miFile.close();

    m_saveMiPushButton->setEnabled( true );
}


void QExperimental3DExtension::saveMi()
{
    QString miFileName = getFileNameToSave( "miDir", tr("Save MI"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
    if ( !miFileName.isNull() ) saveMi( miFileName );
}


void QExperimental3DExtension::saveMi( const QString &fileName )
{
    bool saveAsText = fileName.endsWith( ".txt" );
    QFile miFile( fileName );
    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
    if ( saveAsText ) mode = mode | QIODevice::Text;

    if ( !miFile.open( mode ) )
    {
        DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't save MI"), QString( tr("Can't save MI to file ") ) + fileName );
        return;
    }

    if ( saveAsText )
    {
        QTextStream out( &miFile );
        out << "MI = " << m_mi;
    }
    else
    {
        QDataStream out( &miFile );
        out << m_mi;
    }

    miFile.close();
}


void QExperimental3DExtension::loadVomi()
{
    QString vomiFileName = getFileNameToLoad( "vomiDir", tr("Load VoMI"), tr("Data files (*.dat);;All files (*)") );
    if ( !vomiFileName.isNull() ) loadVomi( vomiFileName );
}


void QExperimental3DExtension::loadVomi( const QString &fileName )
{
    QFile vomiFile( fileName );

    if ( !vomiFile.open( QFile::ReadOnly ) )
    {
        DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't load VoMI"), QString( tr("Can't load VoMI from file ") ) + fileName );
        return;
    }

    unsigned int nObjects = m_volume->getSize();
    m_vomi.resize( nObjects );
    m_maximumVomi = 0.0f;

    QDataStream in( &vomiFile );

    for ( unsigned int i = 0; i < nObjects && !in.atEnd(); i++ )
    {
        in >> m_vomi[i];
        if ( m_vomi.at( i ) > m_maximumVomi ) m_maximumVomi = m_vomi.at( i );
    }

    vomiFile.close();

    m_baseVomiRadioButton->setEnabled( true );
    m_baseVomiCoolWarmRadioButton->setEnabled( true );
    m_vomiCheckBox->setEnabled( true );
    m_vomiCoolWarmCheckBox->setEnabled( true );
    m_opacityLabel->setEnabled( true );
    m_opacityVomiCheckBox->setEnabled( true );
    m_saveVomiPushButton->setEnabled( true );
    m_vomiGradientPushButton->setEnabled( true );
}


void QExperimental3DExtension::saveVomi()
{
    QString vomiFileName = getFileNameToSave( "vomiDir", tr("Save VoMI"), tr("Data files (*.dat);;All files (*)"), "dat" );
    if ( !vomiFileName.isNull() ) saveVomi( vomiFileName );
}


void QExperimental3DExtension::saveVomi( const QString &fileName )
{
    QFile vomiFile( fileName );

    if ( !vomiFile.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't save VoMI"), QString( tr("Can't save VoMI to file ") ) + fileName );
        return;
    }

    QDataStream out( &vomiFile );

    unsigned int nObjects = m_volume->getSize();

    for ( unsigned int i = 0; i < nObjects; i++ ) out << m_vomi.at( i );

    vomiFile.close();
}


void QExperimental3DExtension::loadViewpointVomi()
{
    QString viewpointVomiFileName = getFileNameToLoad( "viewpointVomiDir", tr("Load viewpoint VoMI"), tr("Data files (*.dat);;All files (*)") );
    if ( !viewpointVomiFileName.isNull() ) loadViewpointVomi( viewpointVomiFileName );
}


void QExperimental3DExtension::loadViewpointVomi( const QString &fileName )
{
    QFile viewpointVomiFile( fileName );

    if ( !viewpointVomiFile.open( QFile::ReadOnly ) )
    {
        DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't load viewpoint VoMI"), QString( tr("Can't load viewpoint VoMI from file ") ) + fileName );
        return;
    }

    m_viewpointVomi.clear();

    QDataStream in( &viewpointVomiFile );

    while ( !in.atEnd() )
    {
        float viewpointVomi;
        in >> viewpointVomi;
        m_viewpointVomi << viewpointVomi;
    }

    viewpointVomiFile.close();

    m_saveViewpointVomiPushButton->setEnabled( true );
}


void QExperimental3DExtension::saveViewpointVomi()
{
    QString viewpointVomiFileName = getFileNameToSave( "viewpointVomiDir", tr("Save viewpoint VoMI"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
    if ( !viewpointVomiFileName.isNull() ) saveViewpointVomi( viewpointVomiFileName );
}


void QExperimental3DExtension::saveViewpointVomi( const QString &fileName )
{
    bool saveAsText = fileName.endsWith( ".txt" );
    QFile viewpointVomiFile( fileName );
    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
    if ( saveAsText ) mode = mode | QIODevice::Text;

    if ( !viewpointVomiFile.open( mode ) )
    {
        DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't save viewpoint VoMI"), QString( tr("Can't save viewpoint VoMI to file ") ) + fileName );
        return;
    }

    int nViewpoints = m_viewpointVomi.size();

    if ( saveAsText )
    {
        QTextStream out( &viewpointVomiFile );
        for ( int i = 0; i < nViewpoints; i++ ) out << "VoMI(v" << i + 1 << ") = " << m_viewpointVomi.at( i ) << "\n";
    }
    else
    {
        QDataStream out( &viewpointVomiFile );
        for ( int i = 0; i < nViewpoints; i++ ) out << m_viewpointVomi.at( i );
    }

    viewpointVomiFile.close();
}


void QExperimental3DExtension::loadColorVomiPalette()
{
    QString colorVomiPaletteFileName = getFileNameToLoad( "colorVomiPaletteDir", tr("Load color VoMI palette"), tr("Text files (*.txt);;All files (*)") );
    if ( !colorVomiPaletteFileName.isNull() ) loadColorVomiPalette( colorVomiPaletteFileName );
}


void QExperimental3DExtension::loadColorVomiPalette( const QString &fileName )
{
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


void QExperimental3DExtension::loadColorVomi()
{
    QString colorVomiFileName = getFileNameToLoad( "colorVomiDir", tr("Load color VoMI"), tr("Data files (*.dat);;All files (*)") );
    if ( !colorVomiFileName.isNull() ) loadColorVomi( colorVomiFileName );
}


void QExperimental3DExtension::loadColorVomi( const QString &fileName )
{
    QFile colorVomiFile( fileName );

    if ( !colorVomiFile.open( QFile::ReadOnly ) )
    {
        DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't load color VoMI"), QString( tr("Can't load color VoMI from file ") ) + fileName );
        return;
    }

    unsigned int nObjects = m_volume->getSize();
    m_colorVomi.resize( nObjects );
    m_maximumColorVomi = 0.0f;

    QDataStream in( &colorVomiFile );

    for ( unsigned int i = 0; i < nObjects && !in.atEnd(); i++ )
    {
        Vector3Float colorVomi;
        in >> colorVomi.x >> colorVomi.y >> colorVomi.z;
        m_colorVomi[i] = colorVomi;

        if ( colorVomi.x > m_maximumColorVomi ) m_maximumColorVomi = colorVomi.x;
        if ( colorVomi.y > m_maximumColorVomi ) m_maximumColorVomi = colorVomi.y;
        if ( colorVomi.z > m_maximumColorVomi ) m_maximumColorVomi = colorVomi.z;
    }

    colorVomiFile.close();

    m_baseColorVomiRadioButton->setEnabled( true );
    m_colorVomiCheckBox->setEnabled( true );
    m_saveColorVomiPushButton->setEnabled( true );
}


void QExperimental3DExtension::saveColorVomi()
{
    QString colorVomiFileName = getFileNameToSave( "colorVomiDir", tr("Save color VoMI"), tr("Data files (*.dat);;All files (*)"), "dat" );
    if ( !colorVomiFileName.isNull() ) saveColorVomi( colorVomiFileName );
}


void QExperimental3DExtension::saveColorVomi( const QString &fileName )
{
    QFile colorVomiFile( fileName );

    if ( !colorVomiFile.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't save color VoMI"), QString( tr("Can't save color VoMI to file ") ) + fileName );
        return;
    }

    QDataStream out( &colorVomiFile );

    unsigned int nObjects = m_volume->getSize();

    for ( unsigned int i = 0; i < nObjects; i++ ) out << m_colorVomi.at( i ).x << m_colorVomi.at( i ).y << m_colorVomi.at( i ).z;

    colorVomiFile.close();
}


void QExperimental3DExtension::loadViewpointUnstabilities()
{
    QString viewpointUnstabilitiesFileName = getFileNameToLoad( "viewpointUnstabilitiesDir", tr("Load viewpoint unstabilities"), tr("Data files (*.dat);;All files (*)") );
    if ( !viewpointUnstabilitiesFileName.isNull() ) loadViewpointUnstabilities( viewpointUnstabilitiesFileName );
}


void QExperimental3DExtension::loadViewpointUnstabilities( const QString &fileName )
{
    QFile viewpointUnstabilitiesFile( fileName );

    if ( !viewpointUnstabilitiesFile.open( QFile::ReadOnly ) )
    {
        DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't load viewpoint unstabilities"), QString( tr("Can't load viewpoint unstabilities from file ") ) + fileName );
        return;
    }

    m_viewpointUnstabilities.clear();

    QDataStream in( &viewpointUnstabilitiesFile );

    while ( !in.atEnd() )
    {
        float viewpointUnstabilities;
        in >> viewpointUnstabilities;
        m_viewpointUnstabilities << viewpointUnstabilities;
    }

    viewpointUnstabilitiesFile.close();

    m_saveViewpointUnstabilitiesPushButton->setEnabled( true );
}


void QExperimental3DExtension::saveViewpointUnstabilities()
{
    QString viewpointUnstabilitiesFileName = getFileNameToSave( "viewpointUnstabilitiesDir", tr("Save viewpoint unstabilities"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
    if ( !viewpointUnstabilitiesFileName.isNull() ) saveViewpointUnstabilities( viewpointUnstabilitiesFileName );
}


void QExperimental3DExtension::saveViewpointUnstabilities( const QString &fileName )
{
    bool saveAsText = fileName.endsWith( ".txt" );
    QFile viewpointUnstabilitiesFile( fileName );
    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
    if ( saveAsText ) mode = mode | QIODevice::Text;

    if ( !viewpointUnstabilitiesFile.open( mode ) )
    {
        DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't save viewpoint unstabilities"), QString( tr("Can't save viewpoint unstabilities to file ") ) + fileName );
        return;
    }

    int nViewpoints = m_viewpointUnstabilities.size();

    if ( saveAsText )
    {
        QTextStream out( &viewpointUnstabilitiesFile );
        for ( int i = 0; i < nViewpoints; i++ ) out << "U(v" << i + 1 << ") = " << m_viewpointUnstabilities.at( i ) << "\n";
    }
    else
    {
        QDataStream out( &viewpointUnstabilitiesFile );
        for ( int i = 0; i < nViewpoints; i++ ) out << m_viewpointUnstabilities.at( i );
    }

    viewpointUnstabilitiesFile.close();
}


void QExperimental3DExtension::loadBestViews()
{
    QString bestViewsFileName = getFileNameToLoad( "bestViewsDir", tr("Load best views"), tr("Data files (*.dat);;All files (*)") );
    if ( !bestViewsFileName.isNull() ) loadBestViews( bestViewsFileName );
}


void QExperimental3DExtension::loadBestViews( const QString &fileName )
{
    QFile bestViewsFile( fileName );

    if ( !bestViewsFile.open( QFile::ReadOnly ) )
    {
        DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't load best views"), QString( tr("Can't load best views from file ") ) + fileName );
        return;
    }

    m_bestViews.clear();

    QDataStream in( &bestViewsFile );

    while ( !in.atEnd() )
    {
        int i;
        Vector3 v;
        in >> i;
        in >> v.x >> v.y >> v.z;
        m_bestViews << qMakePair( i, v );
    }

    bestViewsFile.close();

    m_saveBestViewsPushButton->setEnabled( true );
    m_tourBestViewsPushButton->setEnabled( true );
}


void QExperimental3DExtension::saveBestViews()
{
    QString bestViewsFileName = getFileNameToSave( "bestViewsDir", tr("Save best views"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
    if ( !bestViewsFileName.isNull() ) saveBestViews( bestViewsFileName );
}


void QExperimental3DExtension::saveBestViews( const QString &fileName )
{
    bool saveAsText = fileName.endsWith( ".txt" );
    QFile bestViewsFile( fileName );
    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
    if ( saveAsText ) mode = mode | QIODevice::Text;

    if ( !bestViewsFile.open( mode ) )
    {
        DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't save best views"), QString( tr("Can't save best views to file ") ) + fileName );
        return;
    }

    int nBestViews = m_bestViews.size();

    if ( saveAsText )
    {
        QTextStream out( &bestViewsFile );
        for ( int i = 0; i < nBestViews; i++ ) out << i << ": v" << m_bestViews.at( i ).first + 1 << " " << m_bestViews.at( i ).second.toString() << "\n";
    }
    else
    {
        QDataStream out( &bestViewsFile );
        for ( int i = 0; i < nBestViews; i++ )
        {
            const Vector3 &v = m_bestViews.at( i ).second;
            out << m_bestViews.at( i ).first << v.x << v.y << v.z;
        }
    }

    bestViewsFile.close();
}


void QExperimental3DExtension::loadGuidedTour()
{
    QString guidedTourFileName = getFileNameToLoad( "guidedTourDir", tr("Load guided tour"), tr("Data files (*.dat);;All files (*)") );
    if ( !guidedTourFileName.isNull() ) loadGuidedTour( guidedTourFileName );
}


void QExperimental3DExtension::loadGuidedTour( const QString &fileName )
{
    QFile guidedTourFile( fileName );

    if ( !guidedTourFile.open( QFile::ReadOnly ) )
    {
        DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't load guided tour"), QString( tr("Can't load guided tour from file ") ) + fileName );
        return;
    }

    m_guidedTour.clear();

    QDataStream in( &guidedTourFile );

    while ( !in.atEnd() )
    {
        int i;
        Vector3 v;
        in >> i;
        in >> v.x >> v.y >> v.z;
        m_guidedTour << qMakePair( i, v );
    }

    guidedTourFile.close();

    m_saveGuidedTourPushButton->setEnabled( true );
    m_guidedTourPushButton->setEnabled( true );
}


void QExperimental3DExtension::saveGuidedTour()
{
    QString guidedTourFileName = getFileNameToSave( "guidedTourDir", tr("Save guided tour"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
    if ( !guidedTourFileName.isNull() ) saveGuidedTour( guidedTourFileName );
}


void QExperimental3DExtension::saveGuidedTour( const QString &fileName )
{
    bool saveAsText = fileName.endsWith( ".txt" );
    QFile guidedTourFile( fileName );
    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
    if ( saveAsText ) mode = mode | QIODevice::Text;

    if ( !guidedTourFile.open( mode ) )
    {
        DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't save guided tour"), QString( tr("Can't save guided tour to file ") ) + fileName );
        return;
    }

    int nGuidedTour = m_guidedTour.size();

    if ( saveAsText )
    {
        QTextStream out( &guidedTourFile );
        for ( int i = 0; i < nGuidedTour; i++ ) out << i << ": v" << m_guidedTour.at( i ).first + 1 << " " << m_guidedTour.at( i ).second.toString() << "\n";
    }
    else
    {
        QDataStream out( &guidedTourFile );
        for ( int i = 0; i < nGuidedTour; i++ )
        {
            const Vector3 &v = m_guidedTour.at( i ).second;
            out << m_guidedTour.at( i ).first << v.x << v.y << v.z;
        }
    }

    guidedTourFile.close();
}


void QExperimental3DExtension::loadEvmi()
{
    QString evmiFileName = getFileNameToLoad( "evmiDir", tr("Load EVMI"), tr("Data files (*.dat);;All files (*)") );
    if ( !evmiFileName.isNull() ) loadEvmi( evmiFileName );
}


void QExperimental3DExtension::loadEvmi( const QString &fileName )
{
    QFile evmiFile( fileName );

    if ( !evmiFile.open( QFile::ReadOnly ) )
    {
        DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't load EVMI"), QString( tr("Can't load EVMI from file ") ) + fileName );
        return;
    }

    m_evmi.clear();

    QDataStream in( &evmiFile );

    while ( !in.atEnd() )
    {
        float evmi;
        in >> evmi;
        m_evmi << evmi;
    }

    evmiFile.close();

    m_saveEvmiPushButton->setEnabled( true );
}


void QExperimental3DExtension::saveEvmi()
{
    QString evmiFileName = getFileNameToSave( "evmiDir", tr("Save EVMI"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );
    if ( !evmiFileName.isNull() ) saveEvmi( evmiFileName );
}


void QExperimental3DExtension::saveEvmi( const QString &fileName )
{
    bool saveAsText = fileName.endsWith( ".txt" );
    QFile evmiFile( fileName );
    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
    if ( saveAsText ) mode = mode | QIODevice::Text;

    if ( !evmiFile.open( mode ) )
    {
        DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + fileName );
        if ( m_interactive ) QMessageBox::warning( this, tr("Can't save EVMI"), QString( tr("Can't save EVMI to file ") ) + fileName );
        return;
    }

    int nViewpoints = m_evmi.size();

    if ( saveAsText )
    {
        QTextStream out( &evmiFile );
        for ( int i = 0; i < nViewpoints; i++ ) out << "EVMI(v" << i + 1 << ") = " << m_evmi.at( i ) << "\n";
    }
    else
    {
        QDataStream out( &evmiFile );
        for ( int i = 0; i < nViewpoints; i++ ) out << m_evmi.at( i );
    }

    evmiFile.close();
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
    QString programFileName = getFileNameToLoad( "programDir", tr("Load program"), tr("Text files (*.txt);;All files (*)") );

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
    else if ( checkbox == "evmi" )
    {
        if ( run ) m_computeEvmiCheckBox->setChecked( check );
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
    else if ( measure == "evmi" )
    {
        if ( run )
        {
            if ( load ) loadEvmi( fileName );
            else
            {
                if ( m_saveEvmiPushButton->isEnabled() ) saveEvmi( fileName );
                else
                {
                    logProgramError( lineNumber, "No es pot desar l'EVMI", line );
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


QString QExperimental3DExtension::getFileNameToLoad( const QString &settingsDirKey, const QString &caption, const QString &filter )
{
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString dir = settings.value( settingsDirKey, QString() ).toString();
    QString fileName = QFileDialog::getOpenFileName( this, caption, dir, filter );

    if ( !fileName.isNull() )
    {
        QFileInfo fileInfo( fileName );
        settings.setValue( settingsDirKey, fileInfo.absolutePath() );
    }

    settings.endGroup();

    return fileName;
}


QString QExperimental3DExtension::getFileNameToSave( const QString &settingsDirKey, const QString &caption, const QString &filter, const QString &defaultSuffix )
{
    QString fileName;

    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString dir = settings.value( settingsDirKey, QString() ).toString();
    QFileDialog saveDialog( this, caption, dir, filter );
    saveDialog.setAcceptMode( QFileDialog::AcceptSave );
    saveDialog.setDefaultSuffix( defaultSuffix );

    if ( saveDialog.exec() == QDialog::Accepted )
    {
        fileName = saveDialog.selectedFiles().first();
        QFileInfo fileInfo( fileName );
        settings.setValue( settingsDirKey, fileInfo.absolutePath() );
    }

    settings.endGroup();

    return fileName;
}


void QExperimental3DExtension::opacityVomiChecked( bool checked )
{
    if ( checked )
    {
        m_opacitySaliencyCheckBox->setChecked( false );
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


}
