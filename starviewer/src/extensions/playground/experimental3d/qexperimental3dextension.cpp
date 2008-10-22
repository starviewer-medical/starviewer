#include "qexperimental3dextension.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QSettings>

#include "experimental3dvolume.h"
#include "transferfunctionio.h"


namespace udg {


QExperimental3DExtension::QExperimental3DExtension( QWidget *parent )
 : QWidget( parent ), m_volume( 0 )
{
    setupUi( this );

    createConnections();
}


QExperimental3DExtension::~QExperimental3DExtension()
{
}


void QExperimental3DExtension::setInput( Volume *input )
{
    m_viewer->setInput( input );
    m_volume = m_viewer->getVolume();

    unsigned char max = m_volume->getRangeMax();
    m_transferFunctionEditor->setMaximum( max );
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
    connect( m_loadTransferFunctionPushButton, SIGNAL( clicked() ), SLOT( loadTransferFunction() ) );
    connect( m_saveTransferFunctionPushButton, SIGNAL( clicked() ), SLOT( saveTransferFunction() ) );
    connect( m_visualizationOkPushButton, SIGNAL( clicked() ), SLOT( doVisualization() ) );
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
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString transferFunctionDir = settings.value( "transferFunctionDir", QString() ).toString();
    QString transferFunctionFileName = QFileDialog::getOpenFileName( this, tr("Load transfer function"), transferFunctionDir,
                                                                     tr("Transfer function files (*.tf);;XML files (*.xml);;All files (*)") );

    if ( !transferFunctionFileName.isNull() )
    {
        TransferFunction *transferFunction;

        if ( transferFunctionFileName.endsWith( ".xml" ) ) transferFunction = TransferFunctionIO::fromXmlFile( transferFunctionFileName );
        else transferFunction = TransferFunctionIO::fromFile( transferFunctionFileName );

        m_transferFunctionEditor->setTransferFunction( *transferFunction );
        delete transferFunction;

        QFileInfo transferFunctionFileInfo( transferFunctionFileName );
        settings.setValue( "transferFunctionDir", transferFunctionFileInfo.absolutePath() );
    }

    settings.endGroup();
}


void QExperimental3DExtension::saveTransferFunction()
{
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString transferFunctionDir = settings.value( "transferFunctionDir", QString() ).toString();
    QFileDialog saveDialog( this, tr("Save transfer function"), transferFunctionDir,
                            tr("Transfer function files (*.tf);;XML files (*.xml);;All files (*)") );
    saveDialog.setAcceptMode( QFileDialog::AcceptSave );
    saveDialog.setDefaultSuffix( "tf" );

    if ( saveDialog.exec() == QDialog::Accepted )
    {
        QString transferFunctionFileName = saveDialog.selectedFiles().first();

        if ( transferFunctionFileName.endsWith( ".xml" ) )
            TransferFunctionIO::toXmlFile( transferFunctionFileName, m_transferFunctionEditor->getTransferFunction() );
        else
            TransferFunctionIO::toFile( transferFunctionFileName, m_transferFunctionEditor->getTransferFunction() );

        QFileInfo transferFunctionFileInfo( transferFunctionFileName );
        settings.setValue( "transferFunctionDir", transferFunctionFileInfo.absolutePath() );
    }

    settings.endGroup();
}


void QExperimental3DExtension::doVisualization()
{
    m_volume->setInterpolation( static_cast<Experimental3DVolume::Interpolation>( m_interpolationComboBox->currentIndex() ) );
    m_volume->setGradientEstimator( static_cast<Experimental3DVolume::GradientEstimator>( m_gradientEstimatorComboBox->currentIndex() ) );
    m_volume->setLighting( m_diffuseCheckBox->isChecked(), m_specularCheckBox->isChecked(), m_specularPowerDoubleSpinBox->value() );
    m_volume->setTransferFunction( m_transferFunctionEditor->getTransferFunction() );
    m_viewer->render();
}


}
