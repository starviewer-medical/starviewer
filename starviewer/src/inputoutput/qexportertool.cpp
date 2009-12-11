
#include "qexportertool.h"

#include "qviewer.h"
#include "q2dviewer.h"
#include "volume.h"
#include "volumebuilderfromcaptures.h"
#include "dicomimagefilegenerator.h"
#include "image.h"
#include "series.h"
#include "study.h"
#include "patient.h"
#include "inputoutputsettings.h"
#include "localdatabasemanager.h"
#include "pacsdevicemanager.h"
#include "dicommask.h"
#include "queryscreen.h"
#include "singleton.h"
#include <vtkWindowToImageFilter.h>
#include <vtkImageData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkPNGWriter.h>

#include <QDateTime>
#include <QMessageBox>
#include <QPixmap>
#include <QProgressDialog>

namespace udg {

QExporterTool::QExporterTool( QViewer *viewer, QWidget *parent )
: QDialog( parent )
{
    Q_ASSERT( viewer );

    setupUi( this );
    m_viewer = viewer;
    createConnections();
    initialize();
}

QExporterTool::~QExporterTool()
{

}

void QExporterTool::createConnections()
{
    connect( m_saveButton , SIGNAL( clicked() ) , this , SLOT ( generateAndStoreNewSeries() ) );
    connect( m_sendToPacsCheckBox , SIGNAL( toggled(bool) ) , m_pacsNodeComboBox , SLOT( setEnabled(bool) ) );
    connect( m_cancelButton , SIGNAL( clicked() ), this , SLOT( close() ) );
    connect( m_currentImageRadioButton , SIGNAL( clicked() ) , this , SLOT( currentImageRadioButtonClicked()  ) );
    connect( m_allImagesRadioButton , SIGNAL( clicked() ) , this , SLOT( allImagesRadioButtonClicked()  ) );
    connect( m_imagesOfCurrentPhaseRadioButton , SIGNAL( clicked() ) , this , SLOT( imageOfCurrentPhaseRadioButtonClicked()  ) );
    connect( m_phasesOfCurrentImageRadioButton , SIGNAL( clicked() ) , this , SLOT( phasesOfCurrentImageRadioButtonClicked()  ) );
    connect( m_storeToLocalCheckBox, SIGNAL( clicked(bool) ) , this , SLOT( destinationsChanged(bool) ) );
    connect( m_sendToPacsCheckBox, SIGNAL( clicked(bool) ) , this , SLOT( destinationsChanged(bool) ) );
    connect( m_sendToPacsCheckBox , SIGNAL( toggled(bool) ) , m_pacsNodeComboBox , SLOT( setEnabled(bool) ) );
}

void QExporterTool::initialize()
{
    // Depenent de si és un visor 2d o no ( 3D ) habilitem unes opcions o unes altres
    Q2DViewer * q2DViewer = qobject_cast<Q2DViewer *>( m_viewer );

    if ( q2DViewer )
    {
        Volume *input = q2DViewer->getInput();

        if ( input->getNumberOfPhases() == 1 )
        {
            m_imagesOfCurrentPhaseRadioButton->setVisible( false );
            m_phasesOfCurrentImageRadioButton->setVisible( false );
        }

        // Una sola imatge
        if ( q2DViewer->getMaximumSlice() * input->getNumberOfPhases() == 0 )
        {
            m_allImagesRadioButton->setVisible( false );
        }
    }
    else // Si no és un 2DViewer només oferim la opció de guardar la imatge actual
    {
        m_imagesOfCurrentPhaseRadioButton->setVisible( false );
        m_phasesOfCurrentImageRadioButton->setVisible( false );
        m_allImagesRadioButton->setVisible( false );
    }

    // Omplim la llista de pacs. En cas que sigui buida la opció d'enviar a PACS quedarà deshabilitada.
    PacsDeviceManager deviceManager;

    if ( deviceManager.getPACSList().size() == 0 )
    {
        m_sendToPacsCheckBox->setEnabled( false );
    }
    else
    {
        foreach( PacsDevice device , deviceManager.getPACSList() )
        {
            m_pacsNodeComboBox->addItem( QString("%1 - %2").arg( device.getAETitle() , device.getDescription() ) , device.getID() );
        }
    }

    //Tenim el botó de capturar la image actual clicat
    this->currentImageRadioButtonClicked();

}


void QExporterTool::generateAndStoreNewSeries()
{
    VolumeBuilderFromCaptures *builder = new VolumeBuilderFromCaptures();
    builder->setParentStudy( m_viewer->getInput()->getStudy() );

    QProgressDialog progress(this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimum(0);
    if ( m_sendToPacsCheckBox->isChecked() )
        progress.setMaximum(4);
    else
        progress.setMaximum(3);
    progress.setMinimumDuration( 0 );
    progress.setCancelButton( 0 );
    progress.setModal(true);
    progress.setLabelText( tr("Generating images..." ) );
    progress.setValue(0);
    qApp->processEvents();

    m_viewer->getRenderWindow()->OffScreenRenderingOn();

    if ( m_currentImageRadioButton->isChecked() )
    {
        //Capturem la vista
        builder->addCapture( this->captureCurrentView() );
    }
    else if ( m_allImagesRadioButton->isChecked() )
    {
        Q2DViewer * viewer2D = qobject_cast<Q2DViewer *>( m_viewer );

        // Guardem la llesca i la fase acutal
        int currentSlice = viewer2D->getCurrentSlice();
        int currentPhase = viewer2D->getCurrentPhase();

        int maxSlice = viewer2D->getMaximumSlice() + 1;

        // En cas que tinguem fases farem tantes passades com fases
        int phases = viewer2D->getInput()->getNumberOfPhases();

        for( int i = 0; i < maxSlice; i++ )
        {
            viewer2D->setSlice(i);
            for( int j = 0; j < phases; j++ )
            {
                viewer2D->setPhase(j);

                //Capturem la vista
                builder->addCapture( this->captureCurrentView() );
            }
        }
        // restaurem
        viewer2D->setSlice( currentSlice );
        viewer2D->setPhase( currentPhase );

    }
    else if ( m_imagesOfCurrentPhaseRadioButton->isChecked() )
    {
        Q2DViewer * viewer2D = qobject_cast<Q2DViewer *>( m_viewer );

        // Guardem la llesca acutal
        int currentSlice = viewer2D->getCurrentSlice();
        int maxSlice = viewer2D->getMaximumSlice() + 1;

        for( int i = 0; i < maxSlice; i++ )
        {
            viewer2D->setSlice(i);

            //Capturem la vista
            builder->addCapture( this->captureCurrentView() );
        }
        // restaurem
        viewer2D->setSlice( currentSlice );
    }
    else if ( m_phasesOfCurrentImageRadioButton->isChecked() )
    {
        Q2DViewer * viewer2D = qobject_cast<Q2DViewer *>( m_viewer );

        // Guardem la fase acutal
        int currentPhase = viewer2D->getCurrentPhase();
        int phases = viewer2D->getInput()->getNumberOfPhases();

        for( int i = 0; i < phases; i++ )
        {
            viewer2D->setPhase(i);

            //Capturem la vista
            builder->addCapture( this->captureCurrentView() );
        }

        // restaurem
        viewer2D->setPhase( currentPhase );
    }
    else
    {
        DEBUG_LOG( QString("Radio Button no identificat!") );
        return;
    }

    m_viewer->getRenderWindow()->OffScreenRenderingOff();

    builder->setSeriesDescription( m_seriesDescription->text() );

    Volume * generetedVolume = builder->build();

    progress.setLabelText( tr("Generating files..." ) );
    progress.setValue( progress.value() + 1 );
    qApp->processEvents();

    DICOMImageFileGenerator generator;

    Settings settings;

    QString dirPath = settings.getValue( InputOutputSettings::CachePath ).toString() + "/" + generetedVolume->getStudy()->getInstanceUID() + "/" + generetedVolume->getImages().at(0)->getParentSeries()->getInstanceUID();
    generator.setDirPath( dirPath );
    generator.setInput( generetedVolume );

    bool result = generator.generateDICOMFiles();
    
    if ( result )
    {
        DEBUG_LOG ( "Fitxers generats correctament" );

        progress.setLabelText( tr("Inserting into database..." ) );
        progress.setValue( progress.value() + 1 );
        qApp->processEvents();

        LocalDatabaseManager manager;
        manager.save( generetedVolume->getPatient() );
        // TODO Comprovar error

        if ( m_sendToPacsCheckBox->isChecked() ) //Enviem a PACS
        {
            progress.setLabelText( tr("Sending to PACS..." ) );
            progress.setValue( progress.value() + 1 );
            qApp->processEvents();
            QueryScreen * queryScreen = SingletonPointer<QueryScreen>::instance();
            PacsDeviceManager deviceManager;
            PacsDevice device = deviceManager.getPACSDeviceByID( m_pacsNodeComboBox->itemData( m_pacsNodeComboBox->currentIndex() ).toString() );

            DicomMask mask;
            mask.setStudyUID( generetedVolume->getStudy()->getInstanceUID() );
            mask.setSeriesUID( generetedVolume->getImages().at(0)->getParentSeries()->getInstanceUID() );
            queryScreen->storeDicomObjectsToPacs( device , generetedVolume->getStudy() , mask );
        }

        progress.setValue( progress.value() + 1 );
        qApp->processEvents();
    }
    else
    {
        ERROR_LOG ( QString("Error al escriure al directori: %1").arg( dirPath ) );
        QMessageBox::warning(this, tr("Export to DICOM") , tr("The new series has not able to be generated.") );
    }

    delete builder;
    
    this->close();
}

vtkImageData * QExporterTool::captureCurrentView()
{
    vtkWindowToImageFilter * windowToImageFilter = vtkWindowToImageFilter::New();
    windowToImageFilter->SetInput( m_viewer->getRenderWindow() );
    windowToImageFilter->Update();
    windowToImageFilter->Modified();

    vtkImageData *image = vtkImageData::New();
    image->ShallowCopy( windowToImageFilter->GetOutput() );

    return image;
}

void QExporterTool::currentImageRadioButtonClicked()
{
    this->generateCurrentPreview();
    m_numberOfImagesToStore->setText( QString("1/1") );
}

void QExporterTool::allImagesRadioButtonClicked()
{
    Q2DViewer * viewer2D = qobject_cast<Q2DViewer *>( m_viewer );

    if ( viewer2D )
    {
        this->generate2DPreview(0,0);
        m_numberOfImagesToStore->setText( QString("1/%1").arg( QString::number( (viewer2D->getMaximumSlice() + 1) * viewer2D->getInput()->getNumberOfPhases() ) ) );
    }
    else
    {
        DEBUG_LOG( QString("Només està pensat per visors 2D.") );
    }
}

void QExporterTool::imageOfCurrentPhaseRadioButtonClicked()
{
    Q2DViewer * viewer2D = qobject_cast<Q2DViewer *>( m_viewer );

    if ( viewer2D )
    {
        this->generate2DPreview( 0 , viewer2D->getCurrentPhase() );
        m_numberOfImagesToStore->setText( QString("1/%1").arg( QString::number( m_viewer->getInput()->getNumberOfSlicesPerPhase() ) ) );
    }
    else
    {
        DEBUG_LOG( QString("Només està pensat per visors 2D.") );
    }
}

void QExporterTool::phasesOfCurrentImageRadioButtonClicked()
{
    Q2DViewer * viewer2D = qobject_cast<Q2DViewer *>( m_viewer );

    if ( viewer2D )
    {
        this->generate2DPreview( viewer2D->getCurrentSlice() , 0 );        
        m_numberOfImagesToStore->setText( QString("1/%1").arg( QString::number( m_viewer->getInput()->getNumberOfPhases() ) ) );
    }
    else
    {
        DEBUG_LOG( QString("Només està pensat per visors 2D.") );
    }
}

void QExporterTool::destinationsChanged( bool checked )
{
    if ( checked )
    {
        if ( m_sendToPacsCheckBox->isChecked() )
        {
            m_storeToLocalCheckBox->setChecked( true );
        }

        m_saveButton->setEnabled( true );
    }
    else
    {
        if ( ! m_storeToLocalCheckBox->isChecked() )
        {
            m_sendToPacsCheckBox->setChecked( false );
        }

        if ( ! m_storeToLocalCheckBox->isChecked() && ! m_sendToPacsCheckBox->isChecked() )
        {
            m_saveButton->setEnabled( false );
        }
    }
}

void QExporterTool::generate2DPreview(int slice, int phase)
{
    Q2DViewer * viewer2D = qobject_cast<Q2DViewer *>( m_viewer );
    
    Q_ASSERT( viewer2D );

    // Guardem la llesca i la fase acutal
    int currentSlice = viewer2D->getCurrentSlice();
    int currentPhase = viewer2D->getCurrentPhase();

    m_viewer->getRenderWindow()->OffScreenRenderingOn();

    //Assignem la llesca i la fase
    viewer2D->setSlice( slice );
    viewer2D->setPhase( phase );

    generatePreview();

    // restaurem
    viewer2D->setSlice( currentSlice );
    viewer2D->setPhase( currentPhase );

    m_viewer->getRenderWindow()->OffScreenRenderingOff();
}

void QExporterTool::generateCurrentPreview()
{
    m_viewer->getRenderWindow()->OffScreenRenderingOn();

    this->generatePreview();
    
    m_viewer->getRenderWindow()->OffScreenRenderingOff();
}

void QExporterTool::generatePreview()
{
    QString path = QString("%1/preview.png").arg( QDir::tempPath() );
    vtkImageWriter * writer = vtkPNGWriter::New();
    writer->SetInput( captureCurrentView() );
    writer->SetFileName( qPrintable( path ) );
    writer->Write();
    QPixmap pixmap( path );
    m_previewSliceLabel->setPixmap( pixmap.scaledToWidth(150) );

    writer->Delete();
}
}

