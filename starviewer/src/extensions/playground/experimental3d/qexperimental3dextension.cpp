#include "qexperimental3dextension.h"

#include <QButtonGroup>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QSet>
#include <QSettings>
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
#include "volumereslicer.h"


namespace udg {


QExperimental3DExtension::QExperimental3DExtension( QWidget *parent )
 : QWidget( parent ),
   m_volume( 0 ),
   m_computingObscurance( false ), m_obscuranceMainThread( 0 ), m_obscurance( 0 )
{
    setupUi( this );

    createConnections();

    QButtonGroup *bestViewpointsRadioButtons = new QButtonGroup( this );
    bestViewpointsRadioButtons->addButton( m_computeBestViewsNRadioButton );
    bestViewpointsRadioButtons->addButton( m_computeBestViewsThresholdRadioButton );

    m_colorVomiPalette << Vector3Float( 1.0f, 1.0f, 1.0f );
}


QExperimental3DExtension::~QExperimental3DExtension()
{
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
    m_viewer->setInput( input );
    m_volume = m_viewer->getVolume();

    unsigned short max = m_volume->getRangeMax();
    m_transferFunctionEditor->setRange( 0.0, max );
    TransferFunction defaultTransferFunction;
    defaultTransferFunction.addPoint( 0, QColor( 0, 0, 0, 0 ) );
    defaultTransferFunction.addPoint( max, QColor( 255, 255, 255, 255 ) );
    m_transferFunctionEditor->setTransferFunction( defaultTransferFunction );

    doVisualization();
}


void QExperimental3DExtension::createConnections()
{
    // visualització
    connect( m_backgroundColorPushButton, SIGNAL( clicked() ), SLOT( chooseBackgroundColor() ) );
    connect( m_diffuseCheckBox, SIGNAL( toggled(bool) ), SLOT( enableSpecularLighting(bool) ) );
    connect( m_specularCheckBox, SIGNAL( toggled(bool) ), m_specularPowerLabel, SLOT( setEnabled(bool) ) );
    connect( m_specularCheckBox, SIGNAL( toggled(bool) ), m_specularPowerDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_contourCheckBox, SIGNAL( toggled(bool) ), m_contourDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_loadTransferFunctionPushButton, SIGNAL( clicked() ), SLOT( loadTransferFunction() ) );
    connect( m_saveTransferFunctionPushButton, SIGNAL( clicked() ), SLOT( saveTransferFunction() ) );
    connect( m_visualizationOkPushButton, SIGNAL( clicked() ), SLOT( doVisualization() ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceFilterLowLabel, SLOT( setEnabled(bool) ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceFilterLowDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceFilterHighLabel, SLOT( setEnabled(bool) ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceFilterHighDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_vomiCheckBox, SIGNAL( toggled(bool) ), m_vomiFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_vomiCheckBox, SIGNAL( toggled(bool) ), m_vomiFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_vomiCheckBox, SIGNAL( toggled(bool) ), SLOT( vomiChecked(bool) ) );
    connect( m_voxelSalienciesCheckBox, SIGNAL( toggled(bool) ), m_voxelSalienciesFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_voxelSalienciesCheckBox, SIGNAL( toggled(bool) ), m_voxelSalienciesFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_voxelSalienciesCheckBox, SIGNAL( toggled(bool) ), SLOT( voxelSalienciesChecked(bool) ) );
    connect( m_colorVomiCheckBox, SIGNAL( toggled(bool) ), m_colorVomiFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_colorVomiCheckBox, SIGNAL( toggled(bool) ), m_colorVomiFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_colorVomiCheckBox, SIGNAL( toggled(bool) ), SLOT( colorVomiChecked(bool) ) );

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
    connect( m_loadVmiPushButton, SIGNAL( clicked() ), SLOT( loadVmi() ) );
    connect( m_saveVmiPushButton, SIGNAL( clicked() ), SLOT( saveVmi() ) );
    connect( m_loadViewpointUnstabilitiesPushButton, SIGNAL( clicked() ), SLOT( loadViewpointUnstabilities() ) );
    connect( m_saveViewpointUnstabilitiesPushButton, SIGNAL( clicked() ), SLOT( saveViewpointUnstabilities() ) );
    connect( m_loadBestViewsPushButton, SIGNAL( clicked() ), SLOT( loadBestViews() ) );
    connect( m_saveBestViewsPushButton, SIGNAL( clicked() ), SLOT( saveBestViews() ) );
    connect( m_loadGuidedTourPushButton, SIGNAL( clicked() ), SLOT( loadGuidedTour() ) );
    connect( m_saveGuidedTourPushButton, SIGNAL( clicked() ), SLOT( saveGuidedTour() ) );
    connect( m_loadVomiPushButton, SIGNAL( clicked() ), SLOT( loadVomi() ) );
    connect( m_saveVomiPushButton, SIGNAL( clicked() ), SLOT( saveVomi() ) );
    connect( m_loadVoxelSalienciesPushButton, SIGNAL( clicked() ), SLOT( loadVoxelSaliencies() ) );
    connect( m_saveVoxelSalienciesPushButton, SIGNAL( clicked() ), SLOT( saveVoxelSaliencies() ) );
    connect( m_loadViewpointVomiPushButton, SIGNAL( clicked() ), SLOT( loadViewpointVomi() ) );
    connect( m_saveViewpointVomiPushButton, SIGNAL( clicked() ), SLOT( saveViewpointVomi() ) );
    connect( m_loadEvmiPushButton, SIGNAL( clicked() ), SLOT( loadEvmi() ) );
    connect( m_saveEvmiPushButton, SIGNAL( clicked() ), SLOT( saveEvmi() ) );
    connect( m_loadColorVomiPalettePushButton, SIGNAL( clicked() ), SLOT( loadColorVomiPalette() ) );
    connect( m_loadColorVomiPushButton, SIGNAL( clicked() ), SLOT( loadColorVomi() ) );
    connect( m_saveColorVomiPushButton, SIGNAL( clicked() ), SLOT( saveColorVomi() ) );
    connect( m_tourBestViewsPushButton, SIGNAL( clicked() ), SLOT( tourBestViews() ) );
    connect( m_guidedTourPushButton, SIGNAL( clicked() ), SLOT( guidedTour() ) );
    connect( m_vomiGradientPushButton, SIGNAL( clicked() ), SLOT( computeVomiGradient() ) );

    // Program
    connect( m_loadAndRunProgramPushButton, SIGNAL( clicked() ), SLOT( loadAndRunProgram() ) );
}


void QExperimental3DExtension::loadTransferFunction( const QString &fileName )
{
    TransferFunction *transferFunction;

    if ( fileName.endsWith( ".xml" ) ) transferFunction = TransferFunctionIO::fromXmlFile( fileName );
    else transferFunction = TransferFunctionIO::fromFile( fileName );

    m_transferFunctionEditor->setTransferFunction( *transferFunction );
    delete transferFunction;
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
    m_specularCheckBox->setEnabled( on );

    if ( on )
    {
        m_specularPowerLabel->setEnabled( m_specularCheckBox->isChecked() );
        m_specularPowerDoubleSpinBox->setEnabled( m_specularCheckBox->isChecked() );
    }
    else
    {
        m_specularPowerLabel->setEnabled( false );
        m_specularPowerDoubleSpinBox->setEnabled( false );
    }
}


void QExperimental3DExtension::loadTransferFunction()
{
    QString transferFunctionFileName = getFileNameToLoad( "transferFunctionDir", tr("Load transfer function"), tr("XML files (*.xml);;Transfer function files (*.tf);;All files (*)") );
    if ( !transferFunctionFileName.isNull() ) loadTransferFunction( transferFunctionFileName );
}


void QExperimental3DExtension::saveTransferFunction()
{
    QString transferFunctionFileName = getFileNameToSave( "transferFunctionDir", tr("Save transfer function"), tr("XML files (*.xml);;Transfer function files (*.tf);;All files (*)"), "xml" );

    if ( !transferFunctionFileName.isNull() )
    {
        if ( transferFunctionFileName.endsWith( ".xml" ) )
            TransferFunctionIO::toXmlFile( transferFunctionFileName, m_transferFunctionEditor->transferFunction() );
        else
            TransferFunctionIO::toFile( transferFunctionFileName, m_transferFunctionEditor->transferFunction() );
    }
}


void QExperimental3DExtension::doVisualization()
{
    m_volume->setInterpolation( static_cast<Experimental3DVolume::Interpolation>( m_interpolationComboBox->currentIndex() ) );
    m_volume->setGradientEstimator( static_cast<Experimental3DVolume::GradientEstimator>( m_gradientEstimatorComboBox->currentIndex() ) );

    if ( m_diffuseCheckBox->isChecked() ) m_viewer->updateShadingTable();

    m_volume->setLighting( m_diffuseCheckBox->isChecked(), m_specularCheckBox->isChecked(), m_specularPowerDoubleSpinBox->value() );
    m_volume->setContour( m_contourCheckBox->isChecked(), m_contourDoubleSpinBox->value() );
    m_volume->setObscurance( m_obscuranceCheckBox->isChecked(), m_obscurance, m_obscuranceFactorDoubleSpinBox->value(), m_obscuranceFilterLowDoubleSpinBox->value(), m_obscuranceFilterHighDoubleSpinBox->value() );

    if ( m_vomiCheckBox->isChecked() ) m_volume->renderVomi( m_vomi, m_maximumVomi, m_vomiFactorDoubleSpinBox->value(), m_vomiCheckBox->checkState() == Qt::Checked );

    if ( m_voxelSalienciesCheckBox->isChecked() ) m_volume->renderVoxelSaliencies( m_voxelSaliencies, m_maximumSaliency, m_voxelSalienciesFactorDoubleSpinBox->value(), m_diffuseCheckBox->isChecked() );

    if ( m_colorVomiCheckBox->isChecked() ) m_volume->renderColorVomi( m_colorVomi, m_maximumColorVomi, m_colorVomiFactorDoubleSpinBox->value(), m_colorVomiCheckBox->checkState() == Qt::Checked );

    m_volume->setTransferFunction( m_transferFunctionEditor->transferFunction() );
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

    if ( !cameraFileName.isNull() )
    {
        QFile cameraFile( cameraFileName );

        if ( !cameraFile.open( QFile::ReadOnly | QFile::Text ) )
        {
            ERROR_LOG( QString( "No es pot llegir el fitxer " ) + cameraFileName );
            QMessageBox::warning( this, tr("Can't load"), QString( tr("Can't load from file ") ) + cameraFileName );
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
            QMessageBox::warning( this, tr("Can't save"), QString( tr("Can't save to file ") ) + cameraFileName );
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
            m_volume->setObscurance( false, 0, 1.0, 0.0, 1.0 );
            m_viewer->render();
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
            m_volume->setObscurance( false, 0, 1.0, 0.0, 1.0 );
            m_viewer->render();
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
            QMessageBox::warning( this, tr("Can't load obscurance"), QString( tr("Can't load obscurance from file ") ) + obscuranceFileName );
        }
    }
}


void QExperimental3DExtension::saveObscurance()
{
    QString obscuranceFileName = getFileNameToSave( "obscuranceDir", tr("Save obscurance"), tr("Data files (*.dat);;All files (*)"), "dat" );

    if ( !obscuranceFileName.isNull() )
    {
        if ( !m_obscurance->save( obscuranceFileName ) )
            QMessageBox::warning( this, tr("Can't save obscurance"), QString( tr("Can't save obscurance to file ") ) + obscuranceFileName );
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
    bool computeVmi = m_computeVmiCheckBox->isChecked();
    bool computeViewpointUnstabilities = m_computeViewpointUnstabilitiesCheckBox->isChecked();
    bool computeBestViews = m_computeBestViewsCheckBox->isChecked();
    bool computeGuidedTour = m_computeGuidedTourCheckBox->isChecked();
    bool computeVomi = m_computeVomiCheckBox->isChecked();
    bool computeVoxelSaliencies = m_computeVoxelSalienciesCheckBox->isChecked();
    bool computeViewpointVomi = m_computeViewpointVomiCheckBox->isChecked();
    bool computeEvmi = m_computeEvmiCheckBox->isChecked();
    bool computeColorVomi = m_computeColorVomiCheckBox->isChecked();

    // Si no hi ha res a calcular marxem
    if ( !computeVmi && !computeViewpointUnstabilities && !computeBestViews && !computeGuidedTour && !computeVomi && !computeVoxelSaliencies && !computeViewpointVomi && !computeEvmi && !computeColorVomi ) return;

    setCursor( QCursor( Qt::WaitCursor ) );

    // Guardem la càmera
    Vector3 position, focus, up;
    m_viewer->getCamera( position, focus, up );

    // Obtenir direccions
    float distance = ( position - focus ).length();
    ViewpointGenerator viewpointGenerator = m_vmiViewpointDistributionWidget->viewpointGenerator( distance );
    QVector<Vector3> viewpoints = viewpointGenerator.viewpoints();
    int nViewpoints = viewpoints.size();

    // Dependències
    if ( computeGuidedTour && m_bestViews.isEmpty() ) computeBestViews = true;
    if ( computeBestViews && m_vmi.size() != nViewpoints ) computeVmi = true;
    if ( computeViewpointVomi && m_vomi.isEmpty() ) computeVomi = true;
    if ( computeEvmi && m_vomi.isEmpty() ) computeVomi = true;

    // Inicialitzar progrés
    int nSteps = 3; // ray casting (p(O|V)), p(V), p(O)
    if ( computeVomi || computeVoxelSaliencies || computeViewpointVomi || computeColorVomi ) nSteps ++; // VoMI + voxel saliencies + viewpoint VoMI + color VoMI
    if ( computeVmi || computeViewpointUnstabilities || computeEvmi ) nSteps++; // VMI + viewpoint unstabilities + EVMI
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

    // VoMI + voxel saliencies + viewpoint VoMI + color VoMI
    if ( computeVomi || computeVoxelSaliencies || computeViewpointVomi || computeColorVomi )
    {
        computeVomiRelatedMeasures( viewpointGenerator, viewProbabilities, objectProbabilities, pOvFiles, computeVomi, computeVoxelSaliencies, computeViewpointVomi, computeColorVomi );
        m_vmiTotalProgressBar->setValue( ++step );
        m_vmiTotalProgressBar->repaint();
    }

    // VMI + viewpont unstabilities + EVMI
    if ( computeVmi || computeViewpointUnstabilities || computeEvmi )
    {
        computeVmiRelatedMeasures( viewpointGenerator, viewProbabilities, objectProbabilities, pOvFiles, computeVmi, computeViewpointUnstabilities, computeEvmi );
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

    doVisualization();

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
    int nViewpoints = viewProbabilities.size();
    unsigned int nObjects = m_volume->getSize();
    QVector<float> objectProbabilities( nObjects ); // vector p(O), inicialitzat a 0
    float *objectProbabilitiesInView = new float[nObjects]; // vector p(O|v)

    m_vmiProgressBar->setValue( 0 );

    for ( int i = 0; i < nViewpoints; i++ )
    {
        pOvFiles[i]->reset();   // reset per tornar al principi
        pOvFiles[i]->read( reinterpret_cast<char*>( objectProbabilitiesInView ), nObjects * sizeof(float) );    // llegim...
        pOvFiles[i]->reset();   // ... i després fem un reset per tornar al principi i buidar el buffer (amb un peek queda el buffer ple, i es gasta molta memòria)

        for ( unsigned int j = 0; j < nObjects; j++ ) objectProbabilities[j] += viewProbabilities.at( i ) * objectProbabilitiesInView[j];

        m_vmiProgressBar->setValue( 100 * ( i + 1 ) / nViewpoints );
        m_vmiProgressBar->repaint();
    }

    delete[] objectProbabilitiesInView;

    for ( unsigned int j = 0; j < nObjects; j++ ) Q_ASSERT( objectProbabilities.at( j ) == objectProbabilities.at( j ) );

    return objectProbabilities;
}


void QExperimental3DExtension::computeVomiRelatedMeasures( const ViewpointGenerator &viewpointGenerator, const QVector<float> &viewProbabilities, const QVector<float> &objectProbabilities,
                                                           const QVector<QTemporaryFile*> &pOvFiles, bool computeVomi, bool computeVoxelSaliencies, bool computeViewpointVomi, bool computeColorVomi )
{
    const QVector<Vector3> &viewpoints = viewpointGenerator.viewpoints();
    int nViewpoints = viewProbabilities.size();
    int nObjects = objectProbabilities.size();

    if ( computeVomi )
    {
        m_vomi.resize( nObjects );
        m_maximumVomi = 0.0f;
    }
    if ( computeVoxelSaliencies )
    {
        m_voxelSaliencies.resize( nObjects );
        m_maximumSaliency = 0.0f;
    }
    if ( computeViewpointVomi )
    {
        m_viewpointVomi.resize( nViewpoints );
        m_viewpointVomi.fill( 0.0f );
    }
    if ( computeColorVomi )
    {
        m_colorVomi.resize( nObjects );
        m_maximumColorVomi = 0.0f;
    }

    /*
     * Fer un peek per llegir el fitxer sencer és costós. Podem aprofitar que sabem com estan distribuïts els veïns per acotar el tros de fitxer que cal llegir de manera que el que valor que busquem sigui a dins.
     * N'hi haurà prou llegint la llesca actual, l'anterior i la següent.
     */
    int *dimensions = m_volume->getImage()->GetDimensions();
    int dimX = dimensions[0], dimY = dimensions[1], dimZ = dimensions[2], dimXY = dimX * dimY, dimXY3 = dimXY * 3;
    qint64 sizeToRead = dimXY3 * sizeof(float), sizeToReadOnEdge = dimXY * 2 * sizeof(float);
    float **pOV = new float*[nViewpoints];  // p(O|V) (un tros de la matriu cada vegada)
    for ( int i = 0; i < nViewpoints; i++ ) pOV[i] = new float[dimXY3];
    QVector<float> pVoi( nViewpoints ); // p(V|oi)
    QVector<float> pVoj( nViewpoints ); // p(V|oj)

    QVector<Vector3Float> viewpointColors;
    if ( computeColorVomi )
    {
        QMap< QPair<int, int>, QVector<int> > viewpointSubSets; // (nPunts,nColors) -> punts-millor-distribuïts
        {
            viewpointSubSets[qMakePair(42, 6)] = ( QVector<int>() << 13 << 17 << 23 << 27 << 33 << 39 );
        }

        viewpointColors.resize( nViewpoints );
        int nColors = m_colorVomiPalette.size();

        if ( nColors == 1 ) // tots el mateix
        {
            DEBUG_LOG( "tots els colors iguals" );
            for ( int i = 0; i < nViewpoints; i++ ) viewpointColors[i] = m_colorVomiPalette.at( 0 );
        }
        else if ( nViewpoints <= nColors )  // agafem els primers colors
        {
            DEBUG_LOG( "primers colors" );
            for ( int i = 0; i < nViewpoints; i++ ) viewpointColors[i] = m_colorVomiPalette.at( i );
        }
        else    // trobar els més separats -> que la distància mínima entre ells sigui màxima entre totes les mínimes
        {
            QVector<int> bestIndices;
            QPair<int, int> mapIndex( nViewpoints, nColors );

            if ( viewpointSubSets.contains( mapIndex ) )
            {
                DEBUG_LOG( "índexs precalculats" );
                bestIndices = viewpointSubSets.value( mapIndex );
            }
            else
            {
                DEBUG_LOG( "calculem els índexs" );
                double maxMinDistance = 0.0;

                class Combination
                {
                private:
                    int m_n;
                    int m_k;
                    QVector<int> m_data;
                public:
                    Combination( int n, int k )
                    {
                        Q_ASSERT( n >= 0 && k >= 0 );
                        m_n = n;
                        m_k = k;
                        m_data.resize( k );
                        for ( int i = 0; i < k; ++i ) m_data[i] = i;
                    }   // Combination(n,k)
                    QVector<int> data() const
                    {
                        return m_data;
                    }
                    QString toString() const
                    {
                        QString s = "{ ";
                        for ( int i = 0; i < m_k; ++i ) s += QString::number( m_data.at( i ) ) + " ";
                        s += "}";
                        return s;
                    }   // toString()
                    Combination* successor() const
                    {
                        if ( m_data[0] == m_n - m_k) return 0;
                        Combination *ans = new Combination( m_n, m_k );
                        int i;
                        for ( i = 0; i < m_k; ++i ) ans->m_data[i] = m_data.at( i );
                        for ( i = m_k - 1; i > 0 && ans->m_data.at( i ) == m_n - m_k + i; --i );
                        ++ans->m_data[i];
                        for ( int j = i; j < m_k - 1; ++j ) ans->m_data[j+1] = ans->m_data[j] + 1;
                        return ans;
                    }   // successor()
                    static quint64 choose( int n, int k )
                    {
                        if ( n < k ) return 0;  // special case
                        if ( n == k ) return 1;
                        int delta, iMax;
                        if ( k < n - k )    // ex: choose(100,3)
                        {
                            delta = n - k;
                            iMax = k;
                        }
                        else                // ex: choose(100,97)
                        {
                            delta = k;
                            iMax = n - k;
                        }
                        quint64 ans = delta + 1;
                        for ( int i = 2; i <= iMax; ++i ) ans = ( ans * ( delta + i ) ) / i;
                        return ans;
                    }   // choose()
                };  // Combination class

                quint64 nCombinations = Combination::choose( nViewpoints, nColors );
                Combination *c = new Combination( nViewpoints, nColors );
                for ( quint64 i = 0; i < nCombinations; i++ )
                {
                    //DEBUG_LOG( QString( "%1: %2" ).arg( i ).arg( c->toString() ) );

                    QVector<int> indices = c->data();
                    double minDistance = ( viewpoints.at( indices.at( 0 ) ) - viewpoints.at( indices.at( 1 ) ) ).length();
                    bool mayBeBetter = minDistance > maxMinDistance;
                    for ( int j = 0; j < nColors - 1 && mayBeBetter; j++ )
                    {
                        const Vector3 &v1 = viewpoints.at( indices.at( j ) );
                        for ( int k = j + 1; k < nColors && mayBeBetter; k++ )
                        {
                            double distance = ( v1 - viewpoints.at( indices.at( k ) ) ).length();
                            if ( distance < minDistance )
                            {
                                minDistance = distance;
                                mayBeBetter = minDistance > maxMinDistance;
                            }
                        }
                    }
                    if ( minDistance > maxMinDistance )
                    {
                        maxMinDistance = minDistance;
                        bestIndices = indices;
                    }

                    Combination *t = c;
                    c = c->successor();
                    delete t;
                }
            }

            DEBUG_LOG( "best indices:" );
            for ( int i = 0; i < nColors; i++ ) DEBUG_LOG( QString::number( bestIndices.at( i ) ) );

            for ( int i = 0; i < nColors; i++ ) viewpointColors[bestIndices.at(i)] = m_colorVomiPalette.at( i );

            float maxChange;
            do
            {
                maxChange = 0.0f;
                for ( int i = 0; i < nViewpoints; i++ )
                {
                    if ( bestIndices.contains( i ) ) continue;
                    Vector3 viewpoint = viewpoints.at( i );
                    QVector<int> neighbours = viewpointGenerator.neighbours( i );
                    Vector3Float color;
                    double totalWeight = 0.0;
                    int nNeighbours = neighbours.size();
                    for ( int j = 0; j < nNeighbours; j++ )
                    {
                        int neighbourIndex = neighbours.at( j );
                        double distance = ( viewpoint - viewpoints.at( neighbourIndex ) ).length();
                        double weight = 1.0 / distance;
                        color += weight * viewpointColors.at( neighbourIndex );
                        totalWeight += weight;
                    }
                    color /= totalWeight;
                    float change = ( color - viewpointColors.at( i ) ).length();
                    viewpointColors[i] = color;
                    if ( change > maxChange ) maxChange = change;
                }
            } while ( maxChange > 0.1f );
        }

        DEBUG_LOG( "colors: " );
        for ( int i = 0; i < nViewpoints; i++ )
        {
            DEBUG_LOG( QString( "v%1: %2 -> %3" ).arg( i+1 ).arg( viewpoints.at( i ).toString() ).arg( viewpointColors.at( i ).toString() ) );
        }
    }

    m_vmiProgressBar->setValue( 0 );

    // iterem pel volum en l'ordre dels vòxels
    for ( int z = 0, i = 0; z < dimZ; z++ )
    {
        DEBUG_LOG( QString( "llesca %1/%2" ).arg( z + 1 ).arg( dimZ ) );

        // actualitzem pOV
        for ( int k = 0; k < nViewpoints; k++ )
        {
            // quan z == 0, es salta la primera llesca, que seria z == -1
            if ( z == 0 ) pOvFiles[k]->peek( reinterpret_cast<char*>( &(pOV[k][dimXY]) ), sizeToReadOnEdge );
            else if ( z == dimZ - 1 ) pOvFiles[k]->peek( reinterpret_cast<char*>( pOV[k] ), sizeToReadOnEdge );
            else pOvFiles[k]->peek( reinterpret_cast<char*>( pOV[k] ), sizeToRead );
        }

        int pOvShift = ( z - 1 ) * dimXY;

        for ( int y = 0; y < dimY; y++ )
        {
            for ( int x = 0; x < dimX; x++, i++ )
            {
                Q_ASSERT( i == x + y * dimX + z * dimXY );

                float poi = objectProbabilities.at( i );    // p(oi)
                Q_ASSERT( poi == poi );

                // p(V|oi)
                if ( poi == 0.0 ) pVoi.fill( 0.0f );    // si p(oi) == 0 vol dir que el vòxel no es veu des d'enlloc --> p(V|oi) ha de ser tot zeros
                else for ( int k = 0; k < nViewpoints; k++ ) pVoi[k] = viewProbabilities.at( k ) * pOV[k][i - pOvShift] / poi;

                if ( computeVomi )
                {
                    float vomi = InformationTheory<float>::kullbackLeiblerDivergence( pVoi, viewProbabilities );
                    Q_ASSERT( vomi == vomi );
                    m_vomi[i] = vomi;
                    if ( vomi > m_maximumVomi ) m_maximumVomi = vomi;
                }

                if ( computeVoxelSaliencies )
                {
//                        int neighbours[6] = { x - 1 + y * dimX + z * dimXY, x + 1 + y * dimX + z * dimXY,
//                                              x + ( y - 1 ) * dimX + z * dimXY, x + ( y + 1 ) * dimX + z * dimXY,
//                                              x + y * dimX + ( z - 1 ) * dimXY, x + y * dimX + ( z + 1 ) * dimXY };
//                        bool validNeighbours[6] = { x > 0, x + 1 < dimX,
//                                                    y > 0, y + 1 < dimY,
//                                                    z > 0, z + 1 < dimZ };
                    int neighbours[26] = { x-1 + (y-1) * dimX + (z-1) * dimXY, x-1 + (y-1) * dimX +  z    * dimXY, x-1 + (y-1) * dimX + (z+1) * dimXY,
                                           x-1 +  y    * dimX + (z-1) * dimXY, x-1 +  y    * dimX +  z    * dimXY, x-1 +  y    * dimX + (z+1) * dimXY,
                                           x-1 + (y+1) * dimX + (z-1) * dimXY, x-1 + (y+1) * dimX +  z    * dimXY, x-1 + (y+1) * dimX + (z+1) * dimXY,
                                           x   + (y-1) * dimX + (z-1) * dimXY, x   + (y-1) * dimX +  z    * dimXY, x   + (y-1) * dimX + (z+1) * dimXY,
                                           x   +  y    * dimX + (z-1) * dimXY,                                     x   +  y    * dimX + (z+1) * dimXY,
                                           x   + (y+1) * dimX + (z-1) * dimXY, x   + (y+1) * dimX +  z    * dimXY, x   + (y+1) * dimX + (z+1) * dimXY,
                                           x+1 + (y-1) * dimX + (z-1) * dimXY, x+1 + (y-1) * dimX +  z    * dimXY, x+1 + (y-1) * dimX + (z+1) * dimXY,
                                           x+1 +  y    * dimX + (z-1) * dimXY, x+1 +  y    * dimX +  z    * dimXY, x+1 +  y    * dimX + (z+1) * dimXY,
                                           x+1 + (y+1) * dimX + (z-1) * dimXY, x+1 + (y+1) * dimX +  z    * dimXY, x+1 + (y+1) * dimX + (z+1) * dimXY };
                    bool validNeighbours[26] = {   x > 0    &&   y > 0    &&   z > 0   ,   x > 0    &&   y > 0                 ,   x > 0    &&   y > 0    && z+1 < dimZ,
                                                   x > 0                  &&   z > 0   ,   x > 0                               ,   x > 0                  && z+1 < dimZ,
                                                   x > 0    && y+1 < dimY &&   z > 0   ,   x > 0    && y+1 < dimY              ,   x > 0    && y+1 < dimY && z+1 < dimZ,
                                                                 y > 0    &&   z > 0   ,                 y > 0                 ,                 y > 0    && z+1 < dimZ,
                                                                               z > 0   ,                                                                     z+1 < dimZ,
                                                               y+1 < dimY &&   z > 0   ,               y+1 < dimY              ,               y+1 < dimY && z+1 < dimZ,
                                                 x+1 < dimX &&   y > 0    &&   z > 0   , x+1 < dimX &&   y > 0                 , x+1 < dimX &&   y > 0    && z+1 < dimZ,
                                                 x+1 < dimX               &&   z > 0   , x+1 < dimX                            , x+1 < dimX               && z+1 < dimZ,
                                                 x+1 < dimX && y+1 < dimY &&   z > 0   , x+1 < dimX && y+1 < dimY              , x+1 < dimX && y+1 < dimY && z+1 < dimZ };
                    const float SQRT_1_2 = 1.0f / sqrt( 2.0f ), SQRT_1_3 = 1.0f / sqrt( 3.0f );
                    float weights[26] = { SQRT_1_3, SQRT_1_2, SQRT_1_3,
                                          SQRT_1_2,   1.0f  , SQRT_1_2,
                                          SQRT_1_3, SQRT_1_2, SQRT_1_3,
                                          SQRT_1_2,   1.0f  , SQRT_1_2,
                                            1.0f  ,             1.0f  ,
                                          SQRT_1_2,   1.0f  , SQRT_1_2,
                                          SQRT_1_3, SQRT_1_2, SQRT_1_3,
                                          SQRT_1_2,   1.0f  , SQRT_1_2,
                                          SQRT_1_3, SQRT_1_2, SQRT_1_3 };

                    float saliency = 0.0f;
                    float totalWeight = 0.0f;

                    // iterem pels veïns
                    for ( int j = 0; j < 26; j++ )
                    {
                        if ( !validNeighbours[j] ) continue;

                        totalWeight += weights[j];

                        float poj = objectProbabilities.at( neighbours[j] );    // p(oj)
                        Q_ASSERT( poj == poj );
                        float poij = poi + poj; // p(ô)

                        if ( poij == 0.0f ) continue;

                        // p(V|oj)
                        if ( poj == 0.0f ) pVoj.fill( 0.0f );   // si p(oj) == 0 vol dir que el vòxel no es veu des d'enlloc --> p(V|oj) ha de ser tot zeros
                        else for ( int k = 0; k < nViewpoints; k++ ) pVoj[k] = viewProbabilities.at( k ) * pOV[k][neighbours[j] - pOvShift] / poj;

                        float s = weights[j] * InformationTheory<float>::jensenShannonDivergence( poi / poij, poj / poij, pVoi, pVoj );
                        Q_ASSERT( s == s );
                        saliency += s;
                    }

                    saliency /= totalWeight;
                    m_voxelSaliencies[i] = saliency;
                    if ( saliency > m_maximumSaliency ) m_maximumSaliency = saliency;
                }

                if ( computeViewpointVomi )
                {
                    for ( int k = 0; k < nViewpoints; k++ ) m_viewpointVomi[k] += m_vomi[i] * pVoi[k];
                }

                if ( computeColorVomi )
                {
                    Vector3Float white( 1.0f, 1.0f, 1.0f );
                    Vector3Float colorVomi;

                    for ( int k = 0; k < nViewpoints; k++ )
                    {
                        float pvo = pVoi.at( k );
                        if ( pvo > 0.0f ) colorVomi += pvo * log( pvo / viewProbabilities.at( k ) ) * ( white - viewpointColors.at( k ) );
                    }

                    colorVomi /= log( 2.0f );
                    Q_ASSERT( colorVomi.x == colorVomi.x && colorVomi.y == colorVomi.y && colorVomi.z == colorVomi.z );
                    m_colorVomi[i] = colorVomi;

                    if ( colorVomi.x > m_maximumColorVomi ) m_maximumColorVomi = colorVomi.x;
                    if ( colorVomi.y > m_maximumColorVomi ) m_maximumColorVomi = colorVomi.y;
                    if ( colorVomi.z > m_maximumColorVomi ) m_maximumColorVomi = colorVomi.z;
                }
            }
        }

        if ( z > 0 )
        {
            // avancem una llesca en tots els fitxers
            for ( int k = 0; k < nViewpoints; k++ ) pOvFiles[k]->read( reinterpret_cast<char*>( pOV[k] ), dimXY * sizeof(float) );
        }

        m_vmiProgressBar->setValue( 100 * ( z + 1 ) / dimZ );
        m_vmiProgressBar->repaint();
    }

    DEBUG_LOG( "esborrem pOV" );
    for ( int i = 0; i < nViewpoints; i++ ) delete[] pOV[i];
    delete[] pOV;

    if ( computeVomi )
    {
        m_vomiCheckBox->setEnabled( true );
        m_saveVomiPushButton->setEnabled( true );
        m_vomiGradientPushButton->setEnabled( true );
    }

    if ( computeVoxelSaliencies )
    {
        m_voxelSalienciesCheckBox->setEnabled( true );
        m_saveVoxelSalienciesPushButton->setEnabled( true );
    }

    if ( computeViewpointVomi ) m_saveViewpointVomiPushButton->setEnabled( true );

    if ( computeColorVomi )
    {
        m_colorVomiCheckBox->setEnabled( true );
        m_saveColorVomiPushButton->setEnabled( true );
    }
}


void QExperimental3DExtension::computeVmiRelatedMeasures( const ViewpointGenerator &viewpointGenerator, const QVector<float> &viewProbabilities, const QVector<float> &objectProbabilities,
                                                          const QVector<QTemporaryFile*> &pOvFiles, bool computeVmi, bool computeViewpointUnstabilities, bool computeEvmi )
{
    int nViewpoints = viewProbabilities.size();
    int nObjects = objectProbabilities.size();

    if ( computeVmi ) m_vmi.resize( nViewpoints );
    if ( computeViewpointUnstabilities ) m_viewpointUnstabilities.resize( nViewpoints );
    if ( computeEvmi ) m_evmi.resize( nViewpoints );

    QVector<float> ppO; // p'(O)

    if ( computeEvmi )
    {
        ppO.resize( nObjects );

        float total = 0.0f;

        for ( int i = 0; i < nObjects; i++ )
        {

            ppO[i] = objectProbabilities.at( i ) * m_vomi.at( i );
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

        if ( computeVmi )
        {
            float vmi = InformationTheory<float>::kullbackLeiblerDivergence( objectProbabilitiesInView, objectProbabilities );
            Q_ASSERT( vmi == vmi );
            m_vmi[i] = vmi;
            DEBUG_LOG( QString( "VMI(v%1) = %2" ).arg( i + 1 ).arg( vmi ) );
        }

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

    if ( computeVmi ) m_saveVmiPushButton->setEnabled( true );
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


void QExperimental3DExtension::loadVmi()
{
    QString vmiFileName = getFileNameToLoad( "vmiDir", tr("Load VMI"), tr("Data files (*.dat);;All files (*)") );

    if ( !vmiFileName.isNull() )
    {
        QFile vmiFile( vmiFileName );

        if ( !vmiFile.open( QFile::ReadOnly ) )
        {
            DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + vmiFileName );
            QMessageBox::warning( this, tr("Can't load VMI"), QString( tr("Can't load VMI from file ") ) + vmiFileName );
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
}


void QExperimental3DExtension::saveVmi()
{
    QString vmiFileName = getFileNameToSave( "vmiDir", tr("Save VMI"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );

    if ( !vmiFileName.isNull() )
    {
        bool saveAsText = vmiFileName.endsWith( ".txt" );
        QFile vmiFile( vmiFileName );
        QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
        if ( saveAsText ) mode = mode | QIODevice::Text;

        if ( !vmiFile.open( mode ) )
        {
            DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + vmiFileName );
            QMessageBox::warning( this, tr("Can't save VMI"), QString( tr("Can't save VMI to file ") ) + vmiFileName );
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
}


void QExperimental3DExtension::loadViewpointUnstabilities()
{
    QString viewpointUnstabilitiesFileName = getFileNameToLoad( "viewpointUnstabilitiesDir", tr("Load viewpoint unstabilities"), tr("Data files (*.dat);;All files (*)") );

    if ( !viewpointUnstabilitiesFileName.isNull() )
    {
        QFile viewpointUnstabilitiesFile( viewpointUnstabilitiesFileName );

        if ( !viewpointUnstabilitiesFile.open( QFile::ReadOnly ) )
        {
            DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + viewpointUnstabilitiesFileName );
            QMessageBox::warning( this, tr("Can't load viewpoint unstabilities"), QString( tr("Can't load viewpoint unstabilities from file ") ) + viewpointUnstabilitiesFileName );
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
}


void QExperimental3DExtension::saveViewpointUnstabilities()
{
    QString viewpointUnstabilitiesFileName = getFileNameToSave( "viewpointUnstabilitiesDir", tr("Save viewpoint unstabilities"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );

    if ( !viewpointUnstabilitiesFileName.isNull() )
    {
        bool saveAsText = viewpointUnstabilitiesFileName.endsWith( ".txt" );
        QFile viewpointUnstabilitiesFile( viewpointUnstabilitiesFileName );
        QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
        if ( saveAsText ) mode = mode | QIODevice::Text;

        if ( !viewpointUnstabilitiesFile.open( mode ) )
        {
            DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + viewpointUnstabilitiesFileName );
            QMessageBox::warning( this, tr("Can't save viewpoint unstabilities"), QString( tr("Can't save viewpoint unstabilities to file ") ) + viewpointUnstabilitiesFileName );
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
}


void QExperimental3DExtension::loadBestViews()
{
    QString bestViewsFileName = getFileNameToLoad( "bestViewsDir", tr("Load best views"), tr("Data files (*.dat);;All files (*)") );

    if ( !bestViewsFileName.isNull() )
    {
        QFile bestViewsFile( bestViewsFileName );

        if ( !bestViewsFile.open( QFile::ReadOnly ) )
        {
            DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + bestViewsFileName );
            QMessageBox::warning( this, tr("Can't load best views"), QString( tr("Can't load best views from file ") ) + bestViewsFileName );
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
}


void QExperimental3DExtension::saveBestViews()
{
    QString bestViewsFileName = getFileNameToSave( "bestViewsDir", tr("Save best views"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );

    if ( !bestViewsFileName.isNull() )
    {
        bool saveAsText = bestViewsFileName.endsWith( ".txt" );
        QFile bestViewsFile( bestViewsFileName );
        QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
        if ( saveAsText ) mode = mode | QIODevice::Text;

        if ( !bestViewsFile.open( mode ) )
        {
            DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + bestViewsFileName );
            QMessageBox::warning( this, tr("Can't save best views"), QString( tr("Can't save best views to file ") ) + bestViewsFileName );
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
}


void QExperimental3DExtension::loadGuidedTour()
{
    QString guidedTourFileName = getFileNameToLoad( "guidedTourDir", tr("Load guided tour"), tr("Data files (*.dat);;All files (*)") );

    if ( !guidedTourFileName.isNull() )
    {
        QFile guidedTourFile( guidedTourFileName );

        if ( !guidedTourFile.open( QFile::ReadOnly ) )
        {
            DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + guidedTourFileName );
            QMessageBox::warning( this, tr("Can't load guided tour"), QString( tr("Can't load guided tour from file ") ) + guidedTourFileName );
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
}


void QExperimental3DExtension::saveGuidedTour()
{
    QString guidedTourFileName = getFileNameToSave( "guidedTourDir", tr("Save guided tour"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );

    if ( !guidedTourFileName.isNull() )
    {
        bool saveAsText = guidedTourFileName.endsWith( ".txt" );
        QFile guidedTourFile( guidedTourFileName );
        QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
        if ( saveAsText ) mode = mode | QIODevice::Text;

        if ( !guidedTourFile.open( mode ) )
        {
            DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + guidedTourFileName );
            QMessageBox::warning( this, tr("Can't save guided tour"), QString( tr("Can't save guided tour to file ") ) + guidedTourFileName );
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
}


void QExperimental3DExtension::loadVomi()
{
    QString vomiFileName = getFileNameToLoad( "vomiDir", tr("Load VoMI"), tr("Data files (*.dat);;All files (*)") );

    if ( !vomiFileName.isNull() )
    {
        QFile vomiFile( vomiFileName );

        if ( !vomiFile.open( QFile::ReadOnly ) )
        {
            DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + vomiFileName );
            QMessageBox::warning( this, tr("Can't load VoMI"), QString( tr("Can't load VoMI from file ") ) + vomiFileName );
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

        m_vomiCheckBox->setEnabled( true );
        m_saveVomiPushButton->setEnabled( true );
        m_vomiGradientPushButton->setEnabled( true );
    }
}


void QExperimental3DExtension::saveVomi()
{
    QString vomiFileName = getFileNameToSave( "vomiDir", tr("Save VoMI"), tr("Data files (*.dat);;All files (*)"), "dat" );

    if ( !vomiFileName.isNull() )
    {
        QFile vomiFile( vomiFileName );

        if ( !vomiFile.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + vomiFileName );
            QMessageBox::warning( this, tr("Can't save VoMI"), QString( tr("Can't save VoMI to file ") ) + vomiFileName );
            return;
        }

        QDataStream out( &vomiFile );

        unsigned int nObjects = m_volume->getSize();

        for ( unsigned int i = 0; i < nObjects; i++ ) out << m_vomi.at( i );

        vomiFile.close();
    }
}


void QExperimental3DExtension::vomiChecked( bool checked )
{
    if ( checked )
    {
        m_voxelSalienciesCheckBox->setChecked( false );
        m_colorVomiCheckBox->setChecked( false );
    }
}


void QExperimental3DExtension::loadVoxelSaliencies()
{
    QString voxelSalienciesFileName = getFileNameToLoad( "voxelSalienciesDir", tr("Load voxel saliencies"), tr("Data files (*.dat);;All files (*)") );

    if ( !voxelSalienciesFileName.isNull() )
    {
        QFile voxelSalienciesFile( voxelSalienciesFileName );

        if ( !voxelSalienciesFile.open( QFile::ReadOnly ) )
        {
            DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + voxelSalienciesFileName );
            QMessageBox::warning( this, tr("Can't load voxel saliencies"), QString( tr("Can't load voxel saliencies from file ") ) + voxelSalienciesFileName );
            return;
        }

        unsigned int nObjects = m_volume->getSize();
        m_voxelSaliencies.resize( nObjects );
        m_maximumSaliency = 0.0f;

        QDataStream in( &voxelSalienciesFile );

        for ( unsigned int i = 0; i < nObjects && !in.atEnd(); i++ )
        {
            in >> m_voxelSaliencies[i];
            if ( m_voxelSaliencies.at( i ) > m_maximumSaliency ) m_maximumSaliency = m_voxelSaliencies.at( i );
        }

        voxelSalienciesFile.close();

        m_voxelSalienciesCheckBox->setEnabled( true );
        m_saveVoxelSalienciesPushButton->setEnabled( true );
    }
}


void QExperimental3DExtension::saveVoxelSaliencies()
{
    QString voxelSalienciesFileName = getFileNameToSave( "voxelSalienciesDir", tr("Save voxel saliencies"), tr("Data files (*.dat);;All files (*)"), "dat" );

    if ( !voxelSalienciesFileName.isNull() )
    {
        QFile voxelSalienciesFile( voxelSalienciesFileName );

        if ( !voxelSalienciesFile.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + voxelSalienciesFileName );
            QMessageBox::warning( this, tr("Can't save voxel saliencies"), QString( tr("Can't save voxel saliencies to file ") ) + voxelSalienciesFileName );
            return;
        }

        QDataStream out( &voxelSalienciesFile );

        unsigned int nObjects = m_volume->getSize();

        for ( unsigned int i = 0; i < nObjects; i++ ) out << m_voxelSaliencies.at( i );

        voxelSalienciesFile.close();
    }
}


void QExperimental3DExtension::voxelSalienciesChecked( bool checked )
{
    if ( checked )
    {
        m_vomiCheckBox->setChecked( false );
        m_colorVomiCheckBox->setChecked( false );
    }
}


void QExperimental3DExtension::loadViewpointVomi()
{
    QString viewpointVomiFileName = getFileNameToLoad( "viewpointVomiDir", tr("Load viewpoint VoMI"), tr("Data files (*.dat);;All files (*)") );

    if ( !viewpointVomiFileName.isNull() )
    {
        QFile viewpointVomiFile( viewpointVomiFileName );

        if ( !viewpointVomiFile.open( QFile::ReadOnly ) )
        {
            DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + viewpointVomiFileName );
            QMessageBox::warning( this, tr("Can't load viewpoint VoMI"), QString( tr("Can't load viewpoint VoMI from file ") ) + viewpointVomiFileName );
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
}


void QExperimental3DExtension::saveViewpointVomi()
{
    QString viewpointVomiFileName = getFileNameToSave( "viewpointVomiDir", tr("Save viewpoint VoMI"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );

    if ( !viewpointVomiFileName.isNull() )
    {
        bool saveAsText = viewpointVomiFileName.endsWith( ".txt" );
        QFile viewpointVomiFile( viewpointVomiFileName );
        QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
        if ( saveAsText ) mode = mode | QIODevice::Text;

        if ( !viewpointVomiFile.open( mode ) )
        {
            DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + viewpointVomiFileName );
            QMessageBox::warning( this, tr("Can't save viewpoint VoMI"), QString( tr("Can't save viewpoint VoMI to file ") ) + viewpointVomiFileName );
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
}


void QExperimental3DExtension::loadEvmi()
{
    QString evmiFileName = getFileNameToLoad( "evmiDir", tr("Load EVMI"), tr("Data files (*.dat);;All files (*)") );

    if ( !evmiFileName.isNull() )
    {
        QFile evmiFile( evmiFileName );

        if ( !evmiFile.open( QFile::ReadOnly ) )
        {
            DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + evmiFileName );
            QMessageBox::warning( this, tr("Can't load EVMI"), QString( tr("Can't load EVMI from file ") ) + evmiFileName );
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
}


void QExperimental3DExtension::saveEvmi()
{
    QString evmiFileName = getFileNameToSave( "evmiDir", tr("Save EVMI"), tr("Data files (*.dat);;Text files (*.txt);;All files (*)"), "dat" );

    if ( !evmiFileName.isNull() )
    {
        bool saveAsText = evmiFileName.endsWith( ".txt" );
        QFile evmiFile( evmiFileName );
        QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
        if ( saveAsText ) mode = mode | QIODevice::Text;

        if ( !evmiFile.open( mode ) )
        {
            DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + evmiFileName );
            QMessageBox::warning( this, tr("Can't save EVMI"), QString( tr("Can't save EVMI to file ") ) + evmiFileName );
            return;
        }

        int nViewpoints = m_evmi.size();

        if ( saveAsText )
        {
            QTextStream out( &evmiFile );
            for ( int i = 0; i < nViewpoints; i++ ) out << "evmi(v" << i + 1 << ") = " << m_evmi.at( i ) << "\n";
        }
        else
        {
            QDataStream out( &evmiFile );
            for ( int i = 0; i < nViewpoints; i++ ) out << m_evmi.at( i );
        }

        evmiFile.close();
    }
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
        QMessageBox::warning( this, tr("Can't load color VoMI palette"), QString( tr("Can't load color VoMI palette from file ") ) + fileName );
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

    if ( !colorVomiFileName.isNull() )
    {
        QFile colorVomiFile( colorVomiFileName );

        if ( !colorVomiFile.open( QFile::ReadOnly ) )
        {
            DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + colorVomiFileName );
            QMessageBox::warning( this, tr("Can't load color VoMI"), QString( tr("Can't load color VoMI from file ") ) + colorVomiFileName );
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

        m_colorVomiCheckBox->setEnabled( true );
        m_saveColorVomiPushButton->setEnabled( true );
    }
}


void QExperimental3DExtension::saveColorVomi()
{
    QString colorVomiFileName = getFileNameToSave( "colorVomiDir", tr("Save color VoMI"), tr("Data files (*.dat);;All files (*)"), "dat" );

    if ( !colorVomiFileName.isNull() )
    {
        QFile colorVomiFile( colorVomiFileName );

        if ( !colorVomiFile.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + colorVomiFileName );
            QMessageBox::warning( this, tr("Can't save color VoMI"), QString( tr("Can't save color VoMI to file ") ) + colorVomiFileName );
            return;
        }

        QDataStream out( &colorVomiFile );

        unsigned int nObjects = m_volume->getSize();

        for ( unsigned int i = 0; i < nObjects; i++ ) out << m_colorVomi.at( i ).x << m_colorVomi.at( i ).y << m_colorVomi.at( i ).z;

        colorVomiFile.close();
    }
}


void QExperimental3DExtension::colorVomiChecked( bool checked )
{
    if ( checked )
    {
        m_vomiCheckBox->setChecked( false );
        m_voxelSalienciesCheckBox->setChecked( false );
    }
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
    m_voxelSalienciesCheckBox->setEnabled( true );
    m_maximumSaliency = 1.0f;
}


void QExperimental3DExtension::loadAndRunProgram()
{
    QString programFileName = getFileNameToLoad( "programDir", tr("Load program"), tr("Text files (*.txt);;All files (*)") );

    if ( !programFileName.isNull() )
    {
        QFile programFile( programFileName );

        if ( !programFile.open( QFile::ReadOnly | QFile::Text ) )
        {
            DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + programFileName );
            QMessageBox::warning( this, tr("Can't load program"), QString( tr("Can't load program from file ") ) + programFileName );
            return;
        }

        QTextStream in( &programFile );
        bool run = false;
        bool errors = false;

        for ( int i = 0; i < 2; i++ )
        {
            int lineNumber = 0;

            while ( !in.atEnd() )
            {
                lineNumber++;

                QString line = in.readLine();
                QStringList words = line.split( ' ', QString::SkipEmptyParts );

                if ( words.isEmpty() ) continue;

                QString command = words.at( 0 );

                if ( command == "tab" )
                {
                    if ( words.size() < 2 )
                    {
                        logProgramError( lineNumber, "Falta el nom de la pestanya", line );
                        errors = true;
                        continue;
                    }

                    const QString &tab = words.at( 1 );

                    if ( tab == "visualization" )
                    {
                        if ( run ) m_controlsTabWidget->setCurrentWidget( m_visualizationTab );
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
                        logProgramError( lineNumber, "El nom de la pestanya és incorrecte", tab );
                        errors = true;
                    }
                }
                else if ( command == "tf-load" )
                {
                    if ( words.size() < 2 )
                    {
                        logProgramError( lineNumber, "Falta el nom del fitxer", line );
                        errors = true;
                        continue;
                    }

                    if ( run ) loadTransferFunction( words.at( 1 ) );
                }
                else if ( command == "visualization-ok" )
                {
                    if ( run ) doVisualization();
                }
                else if ( command == "vmi-viewpoints" )
                {
                    if ( words.size() < 3 )
                    {
                        logProgramError( lineNumber, "Falten arguments per a la distribució de punts de VMI", line );
                        errors = true;
                        continue;
                    }

                    if ( words.at( 1 ) == "uni" )
                    {
                        int number = words.at( 2 ).toInt();

                        switch ( number )
                        {
                            case 4: if ( run ) m_vmiViewpointDistributionWidget->setToUniform4(); break;
                            case 6: if ( run ) m_vmiViewpointDistributionWidget->setToUniform6(); break;
                            case 8: if ( run ) m_vmiViewpointDistributionWidget->setToUniform8(); break;
                            case 12: if ( run ) m_vmiViewpointDistributionWidget->setToUniform12(); break;
                            case 20: if ( run ) m_vmiViewpointDistributionWidget->setToUniform20(); break;
                            default: logProgramError( lineNumber, "Nombre incorrecte de punts uniformes", words.at( 2 ) ); errors = true; break;
                        }
                    }
                    else if ( words.at( 1 ) == "q-uni" )
                    {
                        if ( run ) m_vmiViewpointDistributionWidget->setToQuasiUniform( words.at( 2 ).toInt() );
                    }
                    else
                    {
                        logProgramError( lineNumber, "Tipus de distribució de punts de VMI incorrecta", words.at( 1 ) );
                        errors = true;
                    }
                }
                else if ( command == "vmi-check" || command == "vmi-uncheck" )
                {
                    bool check = command == "vmi-check";

                    for ( int j = 1; j < words.size(); j++ )
                    {
                        const QString &word = words.at( j );

                        if ( word == "vmi" )
                        {
                            if ( run ) m_computeVmiCheckBox->setChecked( check );
                        }
                        else if ( word == "unstabilities" )
                        {
                            if ( run ) m_computeViewpointUnstabilitiesCheckBox->setChecked( check );
                        }
                        else if ( word == "bestviews" )
                        {
                            if ( run ) m_computeBestViewsCheckBox->setChecked( check );
                        }
                        else if ( word == "guidedtour" )
                        {
                            if ( run ) m_computeGuidedTourCheckBox->setChecked( check );
                        }
                        else if ( word == "vomi" )
                        {
                            if ( run ) m_computeVomiCheckBox->setChecked( check );
                        }
                        else if ( word == "saliencies" )
                        {
                            if ( run ) m_computeVoxelSalienciesCheckBox->setChecked( check );
                        }
                        else if ( word == "vvomi" )
                        {
                            if ( run ) m_computeViewpointVomiCheckBox->setChecked( check );
                        }
                        else if ( word == "evmi" )
                        {
                            if ( run ) m_computeEvmiCheckBox->setChecked( check );
                        }
                        else if ( word == "cvomi" )
                        {
                            if ( run ) m_computeColorVomiCheckBox->setChecked( check );
                        }
                        else
                        {
                            logProgramError( lineNumber, "Nom de checkbox incorrecte", word );
                            errors = true;
                        }
                    }
                }
                else if ( command == "vmi-bestviews" )
                {
                    if ( words.size() < 3 )
                    {
                        logProgramError( lineNumber, "Falten arguments per als paràmetres de càlculs de les millors vistes", line );
                        errors = true;
                        continue;
                    }

                    if ( words.at( 1 ) == "n" )
                    {
                        if ( run )
                        {
                            m_computeBestViewsNRadioButton->setChecked( true );
                            m_computeBestViewsNSpinBox->setValue( words.at( 2 ).toInt() );
                        }
                    }
                    else if ( words.at( 1 ) == "threshold" )
                    {
                        if ( run )
                        {
                            m_computeBestViewsThresholdRadioButton->setChecked( true );
                            m_computeBestViewsThresholdDoubleSpinBox->setValue( words.at( 2 ).toDouble() );
                        }
                    }
                    else
                    {
                        logProgramError( lineNumber, "Paràmetre incorrecte pel nombre de vistes", words.at( 1 ) );
                        errors = true;
                    }
                }
                else if ( command == "vmi-loadpalette" )
                {
                    if ( words.size() < 2 )
                    {
                        logProgramError( lineNumber, "Falten el nom del fitxer de paleta", line );
                        errors = true;
                        continue;
                    }

                    if ( run ) loadColorVomiPalette( words.at( 1 ) );
                }
                else if ( command == "vmi-run" )
                {
                    if ( run ) computeSelectedVmi();
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
    }
}


void QExperimental3DExtension::logProgramError( int lineNumber, const QString &error, const QString &extra ) const
{
    DEBUG_LOG( "[E3DP](" + QString::number( lineNumber ) + ") " + error + ": " + extra );
    ERROR_LOG( "[E3DP](" + QString::number( lineNumber ) + ") " + error + ": " + extra );
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


}
