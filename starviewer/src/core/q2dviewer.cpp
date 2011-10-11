#include "q2dviewer.h"
#include "drawer.h"
#include "volume.h"
#include "logging.h"
#include "image.h"
#include "series.h"
#include "study.h"
#include "patient.h"
#include "imageplane.h"
#include "mathtools.h"
#include "imageorientationoperationsmapper.h"
#include "transferfunction.h"
#include "windowlevelpresetstooldata.h"
#include "coresettings.h"
#include "qviewerworkinprogresswidget.h"
#include "patientorientation.h"
#include "anatomicalplane.h"
#include "starviewerapplication.h"
#include "imageoverlay.h"
#include "drawerbitmap.h"
// Thickslab
#include "vtkProjectionImageFilter.h"
#include "asynchronousvolumereader.h"
#include "volumereaderjob.h"
#include "qviewercommand.h"
#include "renderqviewercommand.h"
// Qt
#include <QResizeEvent>
#include <QImage>
#include <QPainter>
// Include's bàsics vtk
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>
#include <vtkPropPicker.h>
#include <QVTKWidget.h>
#include <vtkWindowToImageFilter.h>
// Composició d'imatges
#include <vtkImageBlend.h>
// Anotacions
#include <vtkCornerAnnotation.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkProperty2D.h>
#include <vtkProp.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkImageActor.h>
// Grayscale pipeline
// Permet aplicar window/level amb imatges a color
#include <vtkImageMapToWindowLevelColors2.h>
#include <vtkScalarsToColors.h>
#include <vtkColorTransferFunction.h>
// Projecció de punts
#include <vtkMatrix4x4.h>

namespace udg {

const QString Q2DViewer::OverlaysDrawerGroup("Overlays");
const QString Q2DViewer::DisplayShuttersDrawerGroup("DisplayShutters");
const QString Q2DViewer::DummyVolumeObjectName("Dummy Volume");

Q2DViewer::Q2DViewer(QWidget *parent)
: QViewer(parent), m_lastView(Q2DViewer::Axial), m_currentSlice(0), m_currentPhase(0), m_overlayVolume(0), m_blender(0), m_imagePointPicker(0),
  m_cornerAnnotations(0), m_enabledAnnotations(Q2DViewer::AllAnnotation), m_overlapMethod(Q2DViewer::Blend), m_rotateFactor(0),
  m_numberOfPhases(1), m_maxSliceValue(0), m_applyFlip(false), m_isImageFlipped(false), m_slabThickness(1), m_firstSlabSlice(0),
  m_lastSlabSlice(0), m_thickSlabActive(false), m_slabProjectionMode(AccumulatorFactory::Maximum)
{
    m_volumeReaderJob = NULL;
    m_inputFinishedCommand = NULL;

    // Filtre de thick slab + grayscale
    m_thickSlabProjectionFilter = vtkProjectionImageFilter::New();
    m_windowLevelLUTMapper = vtkImageMapToWindowLevelColors2::New();

    // Creem anotacions i actors
    createAnnotations();
    m_imageActor = vtkImageActor::New();
    addActors();

    // Creem el picker per obtenir les coordenades de la imatge
    m_imagePointPicker = vtkPropPicker::New();
    this->getInteractor()->SetPicker(m_imagePointPicker);

    // Creem el drawer, passant-li com a visor l'objecte this
    m_drawer = new Drawer(this);

    m_imageOrientationOperationsMapper = new ImageOrientationOperationsMapper();

    m_alignPosition = Q2DViewer::AlignCenter;

    Settings settings;
    m_mammographyAutoOrientationExceptions = settings.getValue(CoreSettings::MammographyAutoOrientationExceptions).toStringList();
    m_defaultPresetToApply = 0;
}

Q2DViewer::~Q2DViewer()
{
    // Fem delete de tots els objectes vtk dels que hem fet un ::New()
    m_patientOrientationTextActor[0]->Delete();
    m_patientOrientationTextActor[1]->Delete();
    m_patientOrientationTextActor[2]->Delete();
    m_patientOrientationTextActor[3]->Delete();
    m_cornerAnnotations->Delete();
    m_imagePointPicker->Delete();
    m_imageActor->Delete();
    m_windowLevelLUTMapper->Delete();
    m_thickSlabProjectionFilter->Delete();
    // Fem delete d'altres objectes vtk en cas que s'hagin hagut de crear
    if (m_blender)
    {
        m_blender->Delete();
    }
    // TODO hem hagut de fer eliminar primer el drawer per davant d'altres objectes
    // per solucionar el ticket #539, però això denota que hi ha algun problema de
    // disseny que fa que no sigui prou robust. L'ordre en que s'esborren els objectes
    // no ens hauria d'afectar
    // HACK Imposem que s'esborri primer el drawer
    delete m_drawer;
    delete m_imageOrientationOperationsMapper;

    this->deleteInputFinishedCommand();
}

void Q2DViewer::createAnnotations()
{
    // Contenidor d'anotacions
    m_cornerAnnotations = vtkCornerAnnotation::New();
    m_cornerAnnotations->GetTextProperty()->SetFontFamilyToArial();
    m_cornerAnnotations->GetTextProperty()->ShadowOn();

    // Anotacions de l'orientació del pacient
    createOrientationAnnotations();
}

void Q2DViewer::createOrientationAnnotations()
{
    // Informació de referència de la orientació del pacient
    for (int i = 0; i < 4; i++)
    {
        m_patientOrientationTextActor[i] = vtkTextActor::New();
        m_patientOrientationTextActor[i]->SetTextScaleModeToNone();
        m_patientOrientationTextActor[i]->GetTextProperty()->SetFontSize(18);
        m_patientOrientationTextActor[i]->GetTextProperty()->BoldOn();
        m_patientOrientationTextActor[i]->GetTextProperty()->SetFontFamilyToArial();
        m_patientOrientationTextActor[i]->GetTextProperty()->ShadowOn();

        m_patientOrientationTextActor[i]->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
        m_patientOrientationTextActor[i]->GetPosition2Coordinate()->SetCoordinateSystemToNormalizedViewport();
    }
    // Ara posem la informació concreta de cadascuna de les referència d'orientació. 0-4 en sentit anti-horari, començant per 0 = esquerra de la pantalla
    m_patientOrientationTextActor[0]->GetTextProperty()->SetJustificationToLeft();
    m_patientOrientationTextActor[0]->SetPosition(0.01, 0.5);

    m_patientOrientationTextActor[1]->GetTextProperty()->SetJustificationToCentered();
    m_patientOrientationTextActor[1]->SetPosition(0.5, 0.01);

    m_patientOrientationTextActor[2]->GetTextProperty()->SetJustificationToRight();
    m_patientOrientationTextActor[2]->SetPosition(0.99, 0.5);

    m_patientOrientationTextActor[3]->GetTextProperty()->SetJustificationToCentered();
    m_patientOrientationTextActor[3]->GetTextProperty()->SetVerticalJustificationToTop();
    m_patientOrientationTextActor[3]->SetPosition(0.5, 0.99);
}

void Q2DViewer::rotateClockWise(int times)
{
    // Almenys ha de ser 1 (+90º)
    if (times <= 0)
    {
        return;
    }

    rotate(times);
    updateCamera();
    render();
}

void Q2DViewer::rotateCounterClockWise(int times)
{
    // Almenys ha de ser 1 (-90º)
    if (times <= 0)
    {
        return;
    }

    rotate(-times);
    updateCamera();
    render();
}

void Q2DViewer::horizontalFlip()
{
    m_applyFlip = true;
    updateCamera();
    render();
}

void Q2DViewer::verticalFlip()
{
    rotate(2);
    horizontalFlip();
}

PatientOrientation Q2DViewer::getCurrentDisplayedImagePatientOrientation() const
{
    // Si no estem a la vista axial (adquisició original) obtindrem 
    // la orientació a través de la primera imatge
    int index = (m_lastView == Axial) ? m_currentSlice : 0;

    PatientOrientation originalOrientation;
    Image *image = m_mainVolume->getImage(index);
    if (image)
    {
        originalOrientation = image->getPatientOrientation();
        if (originalOrientation.getDICOMFormattedPatientOrientation().isEmpty())
        {
            return PatientOrientation();
        }
    }
    else
    {
        DEBUG_LOG("L'índex d'imatge actual és incorrecte o no hi ha imatges al volum. Això no hauria de passar en aquest mètode.");
        return PatientOrientation();
    }
    
    // Escollim les etiquetes que hem agafar com a referència segons la vista actual
    QString baseRowLabel;
    QString baseColumnLabel;
    switch (m_lastView)
    {
        case Axial:
            baseRowLabel = originalOrientation.getRowDirectionLabel();
            baseColumnLabel = originalOrientation.getColumnDirectionLabel();
            break;

        case Sagital:
            baseRowLabel = originalOrientation.getColumnDirectionLabel();
             // TODO Tenim la normal "al revés", en realitat hauria de ser el contrari
            baseColumnLabel = PatientOrientation::getOppositeOrientationLabel(originalOrientation.getNormalDirectionLabel());
            break;

        case Coronal:
            baseRowLabel = originalOrientation.getRowDirectionLabel();
            // TODO Tenim la normal "al revés", en realitat hauria de ser el contrari
            baseColumnLabel = PatientOrientation::getOppositeOrientationLabel(originalOrientation.getNormalDirectionLabel());
            break;
    }

    // Ara caldrà escollir les etiquetes corresponents en funció de les rotacions i flips
    QString rowLabel;
    QString columnLabel;
    int absoluteRotateFactor = (4 + m_rotateFactor) % 4;
    if (m_lastView == Sagital || m_lastView == Coronal)
    {
        // HACK FLIP De moment necessitem fer aquest truc. Durant el refactoring caldria
        // veure si es pot fer d'una manera millor
        if (m_isImageFlipped)
        {
            absoluteRotateFactor = (absoluteRotateFactor + 2) % 4;
        }
    }
    switch (absoluteRotateFactor)
    {
        case 0:
            rowLabel = baseRowLabel;
            columnLabel = baseColumnLabel;
            break;

        case 1:
            rowLabel = PatientOrientation::getOppositeOrientationLabel(baseColumnLabel);
            columnLabel = baseRowLabel;
            break;

        case 2:
            rowLabel = PatientOrientation::getOppositeOrientationLabel(baseRowLabel);
            columnLabel = PatientOrientation::getOppositeOrientationLabel(baseColumnLabel);
            break;

        case 3:
            rowLabel = baseColumnLabel;
            columnLabel = PatientOrientation::getOppositeOrientationLabel(baseRowLabel);
            break;
    }

    if (m_isImageFlipped)
    {
        rowLabel = PatientOrientation::getOppositeOrientationLabel(rowLabel);
    }
    
    PatientOrientation patientOrientation;
    patientOrientation.setLabels(rowLabel, columnLabel);

    return patientOrientation;
}

QString Q2DViewer::getCurrentAnatomicalPlaneLabel() const
{
    return AnatomicalPlane::getLabelFromPatientOrientation(getCurrentDisplayedImagePatientOrientation());
}

void Q2DViewer::getXYZIndexesForView(int &x, int &y, int &z, CameraOrientationType view)
{
    x = Q2DViewer::getXIndexForView(view);
    y = Q2DViewer::getYIndexForView(view);
    z = Q2DViewer::getZIndexForView(view);
}

int Q2DViewer::getXIndexForView(CameraOrientationType view)
{
    switch (view)
    {
        case Q2DViewer::Axial:
            return 0;

        case Q2DViewer::Sagital:
            return 1;

        case Q2DViewer::Coronal:
            return 0;

        default:
            DEBUG_LOG(QString("El paràmetre 'view' conté un valor no esperat: %1.").arg(view));
            return -1;
    }
}

int Q2DViewer::getYIndexForView(CameraOrientationType view)
{
    switch (view)
    {
        case Q2DViewer::Axial:
            return 1;

        case Q2DViewer::Sagital:
            return 2;

        case Q2DViewer::Coronal:
            return 2;

        default:
            DEBUG_LOG(QString("El paràmetre 'view' conté un valor no esperat: %1.").arg(view));
            return -1;
    }
}

int Q2DViewer::getZIndexForView(CameraOrientationType view)
{
    switch (view)
    {
        case Q2DViewer::Axial:
            return 2;

        case Q2DViewer::Sagital:
            return 0;

        case Q2DViewer::Coronal:
            return 1;

        default:
            DEBUG_LOG(QString("El paràmetre 'view' conté un valor no esperat: %1.").arg(view));
            return -1;
    }
}

void Q2DViewer::updatePatientOrientationAnnotation()
{
    // Obtenim l'orientació que estem presentant de la imatge actual
    PatientOrientation currentPatientOrientation = getCurrentDisplayedImagePatientOrientation();

    // Correspondència d'índexs: 0:Left, 1:Bottom, 2:Right, 3:Top
    m_patientOrientationText[0] = PatientOrientation::getOppositeOrientationLabel(currentPatientOrientation.getRowDirectionLabel());
    m_patientOrientationText[1] = currentPatientOrientation.getColumnDirectionLabel();
    m_patientOrientationText[2] = currentPatientOrientation.getRowDirectionLabel();
    m_patientOrientationText[3] = PatientOrientation::getOppositeOrientationLabel(currentPatientOrientation.getColumnDirectionLabel());
    
    bool textActorShouldBeVisible = m_enabledAnnotations.testFlag(Q2DViewer::PatientOrientationAnnotation);

    for (int i = 0; i < 4; ++i)
    {
        if (!m_patientOrientationText[i].isEmpty())
        {
            m_patientOrientationTextActor[i]->SetInput(qPrintable(m_patientOrientationText[i]));
            m_patientOrientationTextActor[i]->SetVisibility(textActorShouldBeVisible);
        }
        else
        {
            m_patientOrientationTextActor[i]->SetVisibility(false);
        }
    }
}

void Q2DViewer::refreshAnnotations()
{
    if (!m_mainVolume)
    {
        return;
    }

    if (m_enabledAnnotations.testFlag(Q2DViewer::PatientInformationAnnotation))
    {
        m_cornerAnnotations->SetText(3, qPrintable(m_upperRightText));
        m_cornerAnnotations->SetText(1, qPrintable(m_lowerRightText.trimmed()));
    }
    else
    {
        m_cornerAnnotations->SetText(3, "");
        m_cornerAnnotations->SetText(1, "");
    }

    if (m_enabledAnnotations.testFlag(Q2DViewer::PatientOrientationAnnotation))
    {
        for (int j = 0; j < 4; j++)
        {
            if (!m_patientOrientationText[j].isEmpty())
            {
                m_patientOrientationTextActor[j]->VisibilityOn();
            }
            // Si l'etiqueta és buida, el mostrem invisible sempre ja que no tenim informació vàlida que mostrar
            else
            {
                m_patientOrientationTextActor[j]->VisibilityOff();
            }
        }
    }
    else
    {
        for (int j = 0; j < 4; j++)
        {
            m_patientOrientationTextActor[j]->VisibilityOff();
        }
    }

    updateAnnotationsInformation(Q2DViewer::WindowInformationAnnotation | Q2DViewer::SliceAnnotation);
}

double Q2DViewer::getThickness()
{
    double thickness;
    switch (m_lastView)
    {
        case Axial:
        {
                // HACK Fins que se solucioni de forma consistent el ticket #492
                if (isThickSlabActive())
                {
                    // Si hi ha thickslab, llavors el thickness es basa a partir de la
                    // suma de l'espai entre llesques
                    // TODO Repassar que això sigui del tot correcte
                    thickness = m_mainVolume->getSpacing()[2] * m_slabThickness;
                }
                else
                {
                    Image *image = getCurrentDisplayedImage();
                    if (image)
                    {
                        thickness = image->getSliceThickness();
                    }
                    else
                    {
                        thickness = m_mainVolume->getSpacing()[2];
                    }
                }
        }
            break;

        case Sagital:
            thickness = m_mainVolume->getSpacing()[0] * m_slabThickness;
            break;

        case Coronal:
            thickness = m_mainVolume->getSpacing()[1] * m_slabThickness;
            break;
    }
    return thickness;
}

void Q2DViewer::getSliceRange(int &min, int &max)
{
    if (m_mainVolume)
    {
        // Si és un volum 3D normal...
        if (m_numberOfPhases == 1)
        {
            int *extent = m_mainVolume->getWholeExtent();
            min = extent[m_lastView * 2];
            max = extent[m_lastView * 2 + 1];
        }
        // Si tenim 4D
        else
        {
            // TODO Assumim que sempre estem en axial!
            min = 0;
            max = m_mainVolume->getNumberOfSlicesPerPhase() - 1;
        }
    }
    else
    {
        min = max = 0;
    }
}

int* Q2DViewer::getSliceRange()
{
    if (m_mainVolume)
    {
        int *range = new int[2];
        this->getSliceRange(range[0], range[1]);
        return range;
    }
    else
    {
        return NULL;
    }
}

int Q2DViewer::getMinimumSlice()
{
    int min, trash;
    this->getSliceRange(min, trash);
    return min;
}

int Q2DViewer::getMaximumSlice()
{
    int max, trash;
    this->getSliceRange(trash, max);
    return max;
}

void Q2DViewer::addActors()
{
    Q_ASSERT(m_cornerAnnotations);
    Q_ASSERT(m_patientOrientationTextActor[0]);
    Q_ASSERT(m_patientOrientationTextActor[1]);
    Q_ASSERT(m_patientOrientationTextActor[2]);
    Q_ASSERT(m_patientOrientationTextActor[3]);
    Q_ASSERT(m_imageActor);

    vtkRenderer *renderer = getRenderer();
    Q_ASSERT(renderer);
    // Anotacions de texte
    renderer->AddViewProp(m_cornerAnnotations);
    renderer->AddViewProp(m_patientOrientationTextActor[0]);
    renderer->AddViewProp(m_patientOrientationTextActor[1]);
    renderer->AddViewProp(m_patientOrientationTextActor[2]);
    renderer->AddViewProp(m_patientOrientationTextActor[3]);
    renderer->AddViewProp(m_imageActor);
    // TODO Colocar això en un lloc mes adient
    vtkCamera *camera = getActiveCamera();
    Q_ASSERT(camera);
    camera->ParallelProjectionOn();
}

void Q2DViewer::setInput(Volume *volume)
{
    if (!volume)
    {
        return;
    }

    this->cancelCurrentVolumeReaderJob();
    this->deleteInputFinishedCommand();

    this->setNewVolume(volume);
}

void Q2DViewer::setInputAsynchronously(Volume *volume, QViewerCommand *inputFinishedCommand)
{
    if (!volume)
    {
        return;
    }
    DEBUG_LOG(QString("Q2DViewer::setInputAsynchronously to Volume %1").arg(volume->getIdentifier().getValue()));

    this->cancelCurrentVolumeReaderJob();
    this->setInputFinishedCommand(inputFinishedCommand);

    bool allowAsynchronousVolumeLoading = Settings().getValue(CoreSettings::AllowAsynchronousVolumeLoading).toBool();
    if (!volume->isPixelDataLoaded() && allowAsynchronousVolumeLoading)
    {
        this->loadVolumeAsynchronously(volume);
    }
    else
    {
        setNewVolumeAndExecuteCommand(volume);
    }
}

void Q2DViewer::setInputAndRender(Volume *volume)
{
    RenderQViewerCommand *command = new RenderQViewerCommand(this);
    this->setInputAsynchronously(volume, command);
}

void Q2DViewer::executeInputFinishedCommand()
{
    if (m_inputFinishedCommand)
    {
        m_inputFinishedCommand->execute();
    }
}

void Q2DViewer::setInputFinishedCommand(QViewerCommand *command)
{
    /// Ens assegurem que la nova command que ens passen no és la mateixa que tenim actualment
    if (command != m_inputFinishedCommand)
    {
        this->deleteInputFinishedCommand();
        m_inputFinishedCommand = command;
    }
}

void Q2DViewer::deleteInputFinishedCommand()
{
    if (m_inputFinishedCommand)
    {
        delete m_inputFinishedCommand;
    }
    m_inputFinishedCommand = NULL;
}

void Q2DViewer::cancelCurrentVolumeReaderJob()
{
    // TODO: Aquí s'hauria de cancel·lar realment el current job. De moment no podem fer-ho i simplement el desconnectem
    // Quan es faci bé, tenir en compte què passa si algun altre visor el vol continuar descarregant igualment i nosaltres aquí el cancelem?
    if (!m_volumeReaderJob.isNull())
    {
        disconnect(m_volumeReaderJob, SIGNAL(done(ThreadWeaver::Job*)), this, SLOT(volumeReaderJobFinished()));
        disconnect(m_volumeReaderJob, SIGNAL(progress(int)), m_workInProgressWidget, SLOT(updateProgress(int)));
    }
    m_volumeReaderJob = NULL;
}

void Q2DViewer::loadVolumeAsynchronously(Volume *volume)
{
    this->setViewerStatus(LoadingVolume);

    // TODO Esborrar volumeReader!!
    AsynchronousVolumeReader *volumeReader = new AsynchronousVolumeReader();
    m_volumeReaderJob = volumeReader->read(volume);
    connect(m_volumeReaderJob, SIGNAL(done(ThreadWeaver::Job*)), SLOT(volumeReaderJobFinished()));
    connect(m_volumeReaderJob, SIGNAL(progress(int)), m_workInProgressWidget, SLOT(updateProgress(int)));

    // TODO: De moment no tenim cap més remei que especificar un volume fals. La resta del viewer (i els que en depenen) s'esperen
    // tenir un volum carregat després de cridar a setInput.
    // També tenim el problema de que perquè surti al menú de botó dret com a seleccionat, cal posar-li el mateix id.
    Volume *dummyVolume = this->getDummyVolumeFromVolume(volume);
    dummyVolume->setIdentifier(volume->getIdentifier());
    this->setNewVolume(dummyVolume, false);
}

void Q2DViewer::volumeReaderJobFinished()
{
    if (m_volumeReaderJob->success())
    {
        setNewVolumeAndExecuteCommand(m_volumeReaderJob->getVolume());
    }
    else
    {
        this->setViewerStatus(LoadingError);
        m_workInProgressWidget->showError(m_volumeReaderJob->getLastErrorMessageToUser());
    }
}

void Q2DViewer::setNewVolumeAndExecuteCommand(Volume *volume)
{
    try
    {
        this->setNewVolume(volume);
        this->executeInputFinishedCommand();
    }
    catch (...)
    {
        // Si tenim algun problema durant el rendering mostrem l'error i reiniciem l'estat del viewer
        this->setViewerStatus(LoadingError);
        m_workInProgressWidget->showError(tr("There's not enough memory for the rendering process. Try to close all the opened %1 windows, restart "
            "the application and try again. If the problem persists, adding more RAM memory or switching to a 64 bit operating system may solve the problem.")
            .arg(ApplicationNameString));
        
        // TODO Cal esborrar oldRenderWindow per evitar memory leaks. Ara mateix si fem Delete() ens peta.
        vtkRenderWindow *oldRenderWindow = getRenderWindow();
        vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
        renderWindow->SetInteractor(getInteractor());
        renderWindow->AddRenderer(getRenderer());
        oldRenderWindow->RemoveRenderer(getRenderer());
        oldRenderWindow->SetInteractor(NULL);
        //Canviem la RenderWindow
        m_vtkWidget->SetRenderWindow(renderWindow);
        // Forcem 2x buffer
        getRenderWindow()->DoubleBufferOn();
        
        m_windowToImageFilter->SetInput(getRenderWindow());

        m_windowLevelLUTMapper->Delete();
        m_windowLevelLUTMapper = vtkImageMapToWindowLevelColors2::New();

        Volume *dummyVolume = this->getDummyVolumeFromVolume(m_mainVolume);
        dummyVolume->setIdentifier(m_mainVolume->getIdentifier());
        this->setNewVolume(dummyVolume, false);
    }
}

bool Q2DViewer::isVolumeLoadingAsynchronously()
{
    return !m_volumeReaderJob.isNull() && !m_volumeReaderJob->isFinished();
}

Volume* Q2DViewer::getDummyVolumeFromVolume(Volume *volume)
{
    // TODO: Estem perdent memòria durant la vida del 2dviewer, caldria esborrar el dummy d'abans
    Volume *newVolume = new Volume(this);
    newVolume->setObjectName(DummyVolumeObjectName);
    newVolume->setImages(volume->getImages());
    newVolume->convertToNeutralVolume();

    return newVolume;
}

void Q2DViewer::setNewVolume(Volume *volume, bool setViewerStatusToVisualizingVolume)
{
    // Cal que primer posem l'estatus en VisualizingVolume per tal de que el QVTKWidget pugui obtenir el tamany que li correspon
    // si no, ens podem trobar que encara no s'hagi mostrat i tingui tamanys no definits fent que la imatge no es mostri completa #1434
    // TODO: Caldria que fitImageIntoViewport() fos indepdent de si s'està visualitzant o no el QVTKWidget
    if (setViewerStatusToVisualizingVolume)
    {
        this->setViewerStatus(VisualizingVolume);
    }

    // Al fer un nou input, les distàncies que guardava el drawer no tenen sentit, pertant s'esborren
    if (m_mainVolume)
    {
        m_drawer->removeAllPrimitives();
    }

    // HACK
    // Desactivem el rendering per tal de millorar l'eficiència del setInput ja que altrament es renderitza múltiples vegades
    enableRendering(false);

    // TODO Caldria fer neteja? bloquejar? Per tal que quedi en negre mentres es carrega el nou volum?
    m_mainVolume = volume;
    m_currentSlice = 0;
    m_currentPhase = 0;
    m_lastView = Q2DViewer::Axial;
    m_alignPosition = Q2DViewer::AlignCenter;

    // Inicialització del thickSlab
    m_slabThickness = 1;
    m_firstSlabSlice = 0;
    m_lastSlabSlice = 0;
    m_thickSlabActive = false;

    // Aquí corretgim el fet que no s'hagi adquirit la imatge en un espai ortogonal
    // No s'aplica perquè afectaria al cursor3D entre d'altres
//     ImagePlane * currentPlane = new ImagePlane();
//     currentPlane->fillFromImage(m_mainVolume->getImage(0,0));
//     double currentPlaneRowVector[3], currentPlaneColumnVector[3];
//     currentPlane->getRowDirectionVector(currentPlaneRowVector);
//     currentPlane->getColumnDirectionVector(currentPlaneColumnVector);
//     vtkMatrix4x4 *projectionMatrix = vtkMatrix4x4::New();
//     projectionMatrix->Identity();
//     int row;
//     for (row = 0; row < 3; row++)
//     {
//         projectionMatrix->SetElement(row,0, (currentPlaneRowVector[row]));
//         projectionMatrix->SetElement(row,1, (currentPlaneColumnVector[row]));
//     }
//
//     m_imageActor->SetUserMatrix(projectionMatrix);
//     delete currentPlane;

    m_numberOfPhases = m_mainVolume->getNumberOfPhases();
    m_maxSliceValue = this->getMaximumSlice();

    // Això es fa per destruir el blender en cas que ja hi hagi algun input i es vulgui canviar
    if (m_blender != 0)
    {
        m_blender->Delete();
        m_blender = 0;
    }
    // Obtenim valors de gris i aquestes coses
    // Aquí es crea tot el pipeline del visualitzador
    this->buildWindowLevelPipeline();

    // Preparem el thickSlab
    // TODO Cada cop que fem setInput resetejem els valors per defecte?
    m_thickSlabProjectionFilter->SetInput(m_mainVolume->getVtkData());
    m_thickSlabProjectionFilter->SetProjectionDimension(m_lastView);
    m_thickSlabProjectionFilter->SetAccumulatorType((AccumulatorFactory::AccumulatorType) m_slabProjectionMode);
    m_thickSlabProjectionFilter->SetFirstSlice(m_firstSlabSlice * m_numberOfPhases + m_currentPhase);
    m_thickSlabProjectionFilter->SetNumberOfSlicesToProject(m_slabThickness);
    m_thickSlabProjectionFilter->SetStep(m_numberOfPhases);

    // TODO BUG Si no fem aquesta crida ens peta al canviar d'input entre un que fos més gran que l'anterior
    updateDisplayExtent();
    resetViewToAxial();

    updatePatientAnnotationInformation();
    this->enableAnnotation(m_enabledAnnotations);

    // Actualitzem la informació de window level
    this->updateWindowLevelData();
    loadImageOverlays(volume);
    loadDisplayShutters(volume);
    // HACK
    // S'activa el rendering de nou per tal de que es renderitzi l'escena
    enableRendering(true);

    // Indiquem el canvi de volum
    emit volumeChanged(m_mainVolume);
}

void Q2DViewer::loadImageOverlays(Volume *volume)
{
    if (!volume)
    {
        return;
    }

    if (volume->objectName() == DummyVolumeObjectName)
    {
        return;
    }

    int numberOfSlices = volume->getNumberOfSlicesPerPhase();
    int numberOfPhases = volume->getNumberOfPhases();
    for (int sliceIndex = 0; sliceIndex < numberOfSlices; ++sliceIndex)
    {
        for (int phaseIndex = 0; phaseIndex < numberOfPhases; ++ phaseIndex)
        {
            Image *image = volume->getImage(sliceIndex, phaseIndex);
            if (!image)
            {
                ERROR_LOG(QString("Error inesperat intentant accedir a la imatge amb índexs: %1(slice), %2(phase) del volum actual")
                    .arg(sliceIndex).arg(phaseIndex));
                DEBUG_LOG(QString("Error inesperat intentant accedir a la imatge amb índexs: %1(slice), %2(phase) del volum actual")
                    .arg(sliceIndex).arg(phaseIndex));
            }
            else
            {
                if (image->hasOverlays())
                {
                    DrawerBitmap *drawerBitmap = imageOverlayToDrawerBitmap(image->getMergedOverlay(), sliceIndex);
                    getDrawer()->draw(drawerBitmap, Q2DViewer::Axial, sliceIndex);
                    getDrawer()->addToGroup(drawerBitmap, OverlaysDrawerGroup);
                }
            }
        }
    }
}

DrawerBitmap* Q2DViewer::imageOverlayToDrawerBitmap(const ImageOverlay &imageOverlay, int slice)
{
    DrawerBitmap *drawerBitmap = new DrawerBitmap;
    // Inicialment tots seran no visibles, llavors segons en la llesca en que ens trobem aquests es veuran o no segons decideixi el Drawer
    drawerBitmap->setVisibility(false);
    // La primitiva no es podrà esborrar amb les tools
    drawerBitmap->setErasable(false);

    double volumeSpacing[3];    
    m_mainVolume->getSpacing(volumeSpacing);
    drawerBitmap->setSpacing(volumeSpacing);
    
    double volumeOrigin[3];
    m_mainVolume->getOrigin(volumeOrigin);

    double bitmapOrigin[3];

    bitmapOrigin[0] = volumeOrigin[0] + imageOverlay.getXOrigin() * volumeSpacing[0];
    bitmapOrigin[1] = volumeOrigin[1] + imageOverlay.getYOrigin() * volumeSpacing[1];
    bitmapOrigin[2] = volumeOrigin[2] + slice * volumeSpacing[2];
    drawerBitmap->setOrigin(bitmapOrigin);
    
    drawerBitmap->setData(imageOverlay.getColumns(), imageOverlay.getRows(), imageOverlay.getData());
    
    return drawerBitmap;
}

void Q2DViewer::loadDisplayShutters(Volume *volume)
{
    if (!volume)
    {
        return;
    }

    if (volume->objectName() == DummyVolumeObjectName)
    {
        return;
    }

    int numberOfSlices = volume->getNumberOfSlicesPerPhase();
    int numberOfPhases = volume->getNumberOfPhases();
    for (int sliceIndex = 0; sliceIndex < numberOfSlices; ++sliceIndex)
    {
        for (int phaseIndex = 0; phaseIndex < numberOfPhases; ++phaseIndex)
        {
            Image *image = volume->getImage(sliceIndex, phaseIndex);
            if (!image)
            {
                ERROR_LOG(QString("Error inesperat intentant accedir a la imatge amb índexs: %1(slice), %2(phase) del volum actual")
                    .arg(sliceIndex).arg(phaseIndex));
                DEBUG_LOG(QString("Error inesperat intentant accedir a la imatge amb índexs: %1(slice), %2(phase) del volum actual")
                    .arg(sliceIndex).arg(phaseIndex));
            }
            else
            {
                if (image->hasDisplayShutters())
                {
                    DrawerBitmap *drawerBitmap = displayShutterToDrawerBitmap(DisplayShutter::intersection(image->getDisplayShutters()), sliceIndex); // Qt Painter Method
                    getDrawer()->draw(drawerBitmap, Q2DViewer::Axial, sliceIndex);
                    getDrawer()->addToGroup(drawerBitmap, DisplayShuttersDrawerGroup);
                }
            }
        }
    }
}

DrawerBitmap* Q2DViewer::displayShutterToDrawerBitmap(const DisplayShutter &shutter, int slice)
{
    DrawerBitmap *drawerBitmap = new DrawerBitmap;
    // Inicialment tots seran no visibles, llavors segons en la llesca en que ens trobem aquests es veuran o no segons decideixi el Drawer
    drawerBitmap->setVisibility(false);
    // La primitiva no es podrà esborrar amb les tools
    drawerBitmap->setErasable(false);
    // El color del bitmap vindrà donat pel valor de presentació del shutter
    drawerBitmap->setForegroundColor(shutter.getShutterValueAsQColor());
    
    double volumeSpacing[3];
    m_mainVolume->getSpacing(volumeSpacing);
    drawerBitmap->setSpacing(volumeSpacing);
    
    double volumeOrigin[3];
    m_mainVolume->getOrigin(volumeOrigin);
    
    double bitmapOrigin[3];
    bitmapOrigin[0] = volumeOrigin[0];
    bitmapOrigin[0] = volumeOrigin[1];
    bitmapOrigin[2] = volumeOrigin[2] + volumeSpacing[2] * slice;
    drawerBitmap->setOrigin(bitmapOrigin);

    // Creem la màscara del shutter a través d'una QImage
    int volumeDimensions[3];
    m_mainVolume->getDimensions(volumeDimensions);
    
    QImage shutterImage(volumeDimensions[0], volumeDimensions[1], QImage::Format_RGB32);
    shutterImage.fill(Qt::black);
    
    QPainter shutterPainter(&shutterImage);
    shutterPainter.setPen(Qt::white);
    shutterPainter.setBrush(Qt::white);
    shutterPainter.drawPolygon(shutter.getAsQPolygon());
    shutterImage.invertPixels();
    // Màscara feta!
    
    // Convertim la imatge en el format de buffer que s'espera drawer bitmap
    unsigned char *data = new unsigned char[volumeDimensions[0] * volumeDimensions[1]];
    for (int i = 0; i < volumeDimensions[1]; ++i)
    {
        QRgb *currentPixel = reinterpret_cast<QRgb*>(shutterImage.scanLine(i));
        for (int j = 0; j < volumeDimensions[0]; ++j)
        {
            data[j + i * volumeDimensions[0]] = qGray(*currentPixel);
            ++currentPixel;
        }
    }
    drawerBitmap->setData(volumeDimensions[0], volumeDimensions[1], data);
    
    return drawerBitmap;
}

void Q2DViewer::setOverlayInput(Volume *volume)
{
    m_overlayVolume = volume;
    if (m_overlapMethod == Blend)
    {
        if (!m_blender)
        {
            m_blender = vtkImageBlend::New();
            m_blender->SetInput(0, m_mainVolume->getVtkData());
        }
        m_blender->SetInput(1, m_overlayVolume->getVtkData());
        m_blender->SetOpacity(1, 1.0 - m_overlayOpacity);
    }
    updateOverlay();
    emit overlayChanged();
}

Volume* Q2DViewer::getOverlayInput()
{
    return m_overlayVolume;
}

void Q2DViewer::updateOverlay()
{
    switch (m_overlapMethod)
    {
        case None:
            // Actualitzem el pipeline
            m_windowLevelLUTMapper->RemoveAllInputs();
            m_windowLevelLUTMapper->SetInput(m_mainVolume->getVtkData());
            // TODO aquest procediment és possible que sigui insuficient,
            // caldria unficar el pipeline en un mateix mètode
            break;

        case Blend:
            // TODO Revisar la manera de donar-li l'input d'un blending al visualitzador
            // Aquest procediment podria ser insuficent de cares a com estigui construit el pipeline
            m_blender->Modified();
            m_windowLevelLUTMapper->SetInputConnection(m_blender->GetOutputPort());
            break;
    }

    emit overlayModified();
}

void Q2DViewer::setOverlayOpacity(double opacity)
{
    m_overlayOpacity = opacity;
}

void Q2DViewer::resetView(CameraOrientationType view)
{
    // Important, cal desactivar el thickslab abans de fer m_lastView = view, sinó falla amb l'update extent
    enableThickSlab(false);
    m_lastView = view;
    updateAnnotationsInformation(Q2DViewer::WindowInformationAnnotation);
    
    // Reiniciem valors per defecte de la càmera
    m_rotateFactor = 0;
    m_applyFlip = false;
    m_isImageFlipped = false;
    m_alignPosition = Q2DViewer::AlignCenter;
    
    resetCamera();
    
    if (m_mainVolume)
    {
        // En comptes de fer servir sempre this->getMaximumSlice(), actualitzem
        // Aquest valor quan cal, és a dir, al posar input i al canviar de vista
        // estalviant-nos crides i crides
        m_maxSliceValue = this->getMaximumSlice();

        enableRendering(false);
        // TODO Solució inmediata per afrontar el ticket #355, pero s'hauria de fer d'una manera mes elegant i consistent
        // TODO Potser la solució més elegant sigui fer servir Q2DViewer::setImageOrientation() en comptes de fer-ho segons 
        // el valor de patient position, ja que en sagital i coronal, sempre voldrem que la orientació sigui d'una forma determinada
        QString position = m_mainVolume->getImage(0)->getParentSeries()->getPatientPosition();
        if (position == "FFP" || position == "HFP")
        {
            if (m_lastView == Sagital)
            {
                rotateClockWise(2);
            }
            else if (m_lastView == Coronal)
            {
                verticalFlip();
            }
        }    
        
        // Ara adaptem els actors a la nova configuració de la càmara perquè siguin visibles
        
        // TODO Això s'hauria d'encapsular en un mètode tipu "resetDisplayExtent()"
        m_currentSlice = 0; // HACK! Necessari perquè s'actualitzi la llesca correctament
        updateDisplayExtent();
        getRenderer()->ResetCamera();
        // Fins aquí seria el mètode "resetDisplayExtent()"
        
        // Ajustem la imatge al viewport
        fitImageIntoViewport();
        
        // Calculem la llesca que cal mostrar segons la vista escollida
        int initialSliceIndex = 0;
        if (m_lastView == Sagital || m_lastView == Coronal)
        {
            initialSliceIndex = m_maxSliceValue/2;
        }
        enableRendering(true);
        m_currentSlice = -1; // HACK! Necessari perquè s'actualitzi la llesca correctament
        setSlice(initialSliceIndex);
    }
    
    // Thick Slab, li indiquem la direcció de projecció actual
    m_thickSlabProjectionFilter->SetProjectionDimension(m_lastView);
    emit viewChanged(m_lastView);
}

void Q2DViewer::resetViewToAxial()
{
    resetView(Q2DViewer::Axial);
}

void Q2DViewer::resetViewToCoronal()
{
    resetView(Q2DViewer::Coronal);
}

void Q2DViewer::resetViewToSagital()
{
    resetView(Q2DViewer::Sagital);
}

void Q2DViewer::updateCamera()
{
    if (m_mainVolume)
    {
        vtkCamera *camera = getActiveCamera();
        Q_ASSERT(camera);

        double roll = 0.0;
        switch (m_lastView)
        {
            case Axial:
                if (m_isImageFlipped)
                {
                    roll = m_rotateFactor * 90. + 180.;
                }
                else
                {
                    roll = -m_rotateFactor * 90. + 180.;
                }
                break;

            case Sagital:
                if (m_isImageFlipped)
                {
                    roll = m_rotateFactor * 90. - 90.;
                }
                else
                {
                    roll = -m_rotateFactor * 90. - 90.;
                }
                break;

            case Coronal:
                if (m_isImageFlipped)
                {
                    roll = m_rotateFactor * 90.;
                }
                else
                {
                    roll = -m_rotateFactor * 90.;
                }
                break;
        }
        camera->SetRoll(roll);

        if (m_applyFlip)
        {
            // Alternativa 1)
            // TODO Així movem la càmera, però faltaria que la imatge no es mogués de lloc
            // potser implementant a la nostra manera el metode Azimuth i prenent com a centre
            // el centre de la imatge. Una altra possibilitat es contrarestar el desplaçament de la
            // camera en l'eix en que s'ha produit
            camera->Azimuth(180);
            switch (this->m_lastView)
            {
                // HACK Aquest hack esta relacionat amb els de getCurrentDisplayedImageOrientationLabels()
                // és un petit truc perque la imatge quedi orientada correctament. Caldria
                // veure si en el refactoring podem fer-ho d'una forma millor
                case Sagital:
                case Coronal:
                    rotate(-2);
                    break;

                default:
                    break;
            }

            this->getRenderer()->ResetCameraClippingRange();
            m_applyFlip = false;
            m_isImageFlipped = !m_isImageFlipped;
        }
        emit cameraChanged();
        updatePatientOrientationAnnotation();
    }
    else
    {
        DEBUG_LOG("Intentant actualitzar rotació de càmera sense haver donat un input abans...");
    }
}

void Q2DViewer::resetCamera()
{
    vtkCamera *camera = getActiveCamera();
    Q_ASSERT(camera);

    double cameraViewUp[3] = { 0.0, 0.0, 0.0 };
    double cameraPosition[3] = { 0.0, 0.0, 0.0 };
    double cameraRoll = 0.0;

    // Ajustem els paràmetres de la càmera segons la vista
    switch (m_lastView)
    {
        case Axial:
            cameraViewUp[1] = -1.0;
            cameraPosition[2] = -1.0;
            cameraRoll = 180.0;
            break;

        case Sagital:
            cameraViewUp[2] = 1.0;
            cameraPosition[0] = 1.0;
            cameraRoll = -90.0;
            break;

        case Coronal:
            cameraViewUp[2] = 1.0;
            cameraPosition[1] = -1.0;
            cameraRoll = 0.0;
            break;
    }

    // Assignem els valors
    camera->SetFocalPoint(0, 0, 0);
    camera->SetViewUp(cameraViewUp);
    camera->SetPosition(cameraPosition);
    camera->SetRoll(cameraRoll);
}

void Q2DViewer::setSlice(int value)
{
    if (this->m_mainVolume && this->m_currentSlice != value)
    {
        this->checkAndUpdateSliceValue(value);
        if (isThickSlabActive())
        {
            m_thickSlabProjectionFilter->SetFirstSlice(m_firstSlabSlice * m_numberOfPhases + m_currentPhase);
            // TODO Cal actualitzar aquest valor?
            m_thickSlabProjectionFilter->SetNumberOfSlicesToProject(m_slabThickness);
            // Si hi ha el thickslab activat, eliminem totes les roi's. És la decisió ràpida que s'ha près.
            this->getDrawer()->removeAllPrimitives();
        }
        this->updateDisplayExtent();
        updateDefaultPreset();
        updateSliceAnnotationInformation();
        updatePatientOrientationAnnotation();
        emit sliceChanged(m_currentSlice);
        render();
    }
}

void Q2DViewer::setPhase(int value)
{
    // Comprovació de rang
    if (m_mainVolume)
    {
        if (value < 0)
        {
            value = m_numberOfPhases - 1;
        }
        else if (value > m_numberOfPhases - 1)
        {
            value = 0;
        }

        m_currentPhase = value;
        if (isThickSlabActive())
        {
            m_thickSlabProjectionFilter->SetFirstSlice(m_firstSlabSlice * m_numberOfPhases + m_currentPhase);
        }
        this->updateDisplayExtent();
        updateDefaultPreset();
        updateSliceAnnotationInformation();
        emit phaseChanged(m_currentPhase);
        this->render();
    }
}

void Q2DViewer::setOverlapMethod(OverlapMethod method)
{
    m_overlapMethod = method;
}

void Q2DViewer::setOverlapMethodToNone()
{
    setOverlapMethod(Q2DViewer::None);
    m_windowLevelLUTMapper->RemoveAllInputs();
    m_windowLevelLUTMapper->SetInput(m_mainVolume->getVtkData());
}

void Q2DViewer::setOverlapMethodToBlend()
{
    setOverlapMethod(Q2DViewer::Blend);
}

void Q2DViewer::resizeEvent(QResizeEvent *resize)
{
    Q_UNUSED(resize);
    if (m_mainVolume)
    {
        switch (m_alignPosition)
        {
            case AlignRight:
                alignRight();
                break;

            case AlignLeft:
                alignLeft();
                break;

            case AlignCenter:
                fitImageIntoViewport();
                break;
        }
    }
}

void Q2DViewer::setWindowLevel(double window, double level)
{
    if (m_mainVolume)
    {
        if ((m_windowLevelLUTMapper->GetWindow() != window) || (m_windowLevelLUTMapper->GetLevel() != level))
        {
            m_windowLevelLUTMapper->SetWindow(window);
            m_windowLevelLUTMapper->SetLevel(level);
            updateAnnotationsInformation(Q2DViewer::WindowInformationAnnotation);
            this->render();
            emit windowLevelChanged(window, level);
        }
    }
    else
    {
        DEBUG_LOG("::setWindowLevel() : No tenim input ");
    }
}

void Q2DViewer::setTransferFunction(TransferFunction *transferFunction)
{
    m_transferFunction = transferFunction;
    // Apliquem la funció de transferència sobre el window level mapper
    m_windowLevelLUTMapper->SetLookupTable(m_transferFunction->vtkColorTransferFunction());
}

void Q2DViewer::getCurrentWindowLevel(double wl[2])
{
    if (m_mainVolume)
    {
        wl[0] = m_windowLevelLUTMapper->GetWindow();
        wl[1] = m_windowLevelLUTMapper->GetLevel();
    }
    else
    {
        DEBUG_LOG("::getCurrentWindowLevel() : No tenim input ");
    }
}

double Q2DViewer::getCurrentColorWindow()
{
    if (m_mainVolume)
    {
        return m_windowLevelLUTMapper->GetWindow();
    }
    else
    {
        DEBUG_LOG("::getCurrentColorWindow() : No tenim input ");
        return 0;
    }
}

double Q2DViewer::getCurrentColorLevel()
{
    if (m_mainVolume)
    {
        return m_windowLevelLUTMapper->GetLevel();
    }
    else
    {
        DEBUG_LOG("::getCurrentColorLevel() : No tenim input ");
        return 0;
    }
}

int Q2DViewer::getCurrentSlice() const
{
    return m_currentSlice;
}

int Q2DViewer::getCurrentPhase() const
{
    return m_currentPhase;
}

Image* Q2DViewer::getCurrentDisplayedImage() const
{
    return m_mainVolume->getImage(m_currentSlice, m_currentPhase);
}

ImagePlane* Q2DViewer::getCurrentImagePlane(bool vtkReconstructionHack)
{
    return this->getImagePlane(m_currentSlice, m_currentPhase, vtkReconstructionHack);
}

ImagePlane* Q2DViewer::getImagePlane(int sliceNumber, int phaseNumber, bool vtkReconstructionHack)
{
    ImagePlane *imagePlane = 0;
    if (m_mainVolume)
    {
        int *dimensions = m_mainVolume->getDimensions();
        double *spacing = m_mainVolume->getSpacing();
        const double *origin = m_mainVolume->getOrigin();
        switch (m_lastView)
        {
            // XY
            case Axial:
            {
                Image *image = m_mainVolume->getImage(sliceNumber, phaseNumber);
                if (image)
                {
                    imagePlane = new ImagePlane();
                    imagePlane->fillFromImage(image);
                }
            }
                break;

            // YZ TODO Encara no esta comprovat que aquest pla sigui correcte
            case Sagital:
            {
                Image *image = m_mainVolume->getImage(0);
                if (image)
                {
                    QVector3D sagittalRowVector = image->getImageOrientationPatient().getColumnVector();
                    QVector3D sagittalColumnVector;
                    if (vtkReconstructionHack)
                    {
                        // Retornem un fals pla, respecte el món real, però que s'ajusta més al món vtk
                        sagittalColumnVector = image->getImageOrientationPatient().getNormalVector();
                    }
                    else
                    {
                        // Això serà lo normal, retornar la autèntica direcció del pla
                        double sagittalColumnArray[3];
                        m_mainVolume->getStackDirection(sagittalColumnArray, 0);
                        sagittalColumnVector.setX(sagittalColumnArray[0]);
                        sagittalColumnVector.setY(sagittalColumnArray[1]);
                        sagittalColumnVector.setZ(sagittalColumnArray[2]);
                    }

                    imagePlane = new ImagePlane();
                    imagePlane->setImageOrientation(ImageOrientation(sagittalRowVector, sagittalColumnVector));                    
                    imagePlane->setSpacing(spacing[1], spacing[2]);
                    imagePlane->setThickness(spacing[0]);
                    imagePlane->setRows(dimensions[2]);
                    imagePlane->setColumns(dimensions[1]);

                    QVector3D sagittalNormalVector = image->getImageOrientationPatient().getRowVector();
                    // TODO Falta esbrinar si l'origen que estem donant es bo o no
                    imagePlane->setOrigin(origin[0] + sliceNumber * sagittalNormalVector.x() * spacing[0],
                                          origin[1] + sliceNumber * sagittalNormalVector.y() * spacing[0],
                                          origin[2] + sliceNumber * sagittalNormalVector.z() * spacing[0]);
                }
            }
                break;

            // XZ TODO Encara no esta comprovat que aquest pla sigui correcte
            case Coronal:
            {
                Image *image = m_mainVolume->getImage(0);
                if (image)
                {
                    QVector3D coronalRowVector = image->getImageOrientationPatient().getRowVector();
                    QVector3D coronalColumnVector;
                    if (vtkReconstructionHack)
                    {
                        // Retornem un fals pla, respecte el món real, però que s'ajusta més al món vtk
                        coronalColumnVector = image->getImageOrientationPatient().getNormalVector();
                    }
                    else
                    {
                        double coronalColumnArray[3];
                        m_mainVolume->getStackDirection(coronalColumnArray, 0);
                        coronalColumnVector.setX(coronalColumnArray[0]);
                        coronalColumnVector.setY(coronalColumnArray[1]);
                        coronalColumnVector.setZ(coronalColumnArray[2]);
                    }

                    imagePlane = new ImagePlane();
                    imagePlane->setImageOrientation(ImageOrientation(coronalRowVector, coronalColumnVector));
                    imagePlane->setSpacing(spacing[0], spacing[2]);
                    imagePlane->setThickness(spacing[1]);
                    imagePlane->setRows(dimensions[2]);
                    imagePlane->setColumns(dimensions[0]);

                    QVector3D coronalNormalVector = image->getImageOrientationPatient().getColumnVector();
                    // TODO Falta esbrinar si l'origen que estem donant es bo o no
                    imagePlane->setOrigin(origin[0] + coronalNormalVector.x() * sliceNumber * spacing[1],
                                          origin[1] + coronalNormalVector.y() * sliceNumber * spacing[1],
                                          origin[2] + coronalNormalVector.z() * sliceNumber * spacing[1]);
                }
            }
                break;
        }
    }
    return imagePlane;
}

void Q2DViewer::projectDICOMPointToCurrentDisplayedImage(const double pointToProject[3], double projectedPoint[3], bool vtkReconstructionHack)
{
    // AQUÍ SUMEM L'origen TAL CUAL + L'ERROR DE DESPLAÇAMENT VTK
    //
    // La projecció es fa de la següent manera:
    // Primer es fa una  una projecció convencional del punt sobre el pla actual (DICOM)
    // Com que el mapeig de coordenades VTK va a la seva bola, necessitem corretgir el desplaçament
    // introduit per VTK respecte a les coordenades "reals" de DICOM
    // Aquest desplaçament consistirà en tornar a sumar l'origen del primer pla del volum
    // en principi, fer-ho amb l'origen de m_mainVolume també seria correcte

    ImagePlane *currentPlane = this->getCurrentImagePlane(vtkReconstructionHack);
    if (currentPlane)
    {
        // Recollim les dades del pla actual sobre el qual volem projectar el punt de l'altre pla
        double currentPlaneRowVector[3], currentPlaneColumnVector[3], currentPlaneNormalVector[3], currentPlaneOrigin[3];
        currentPlane->getRowDirectionVector(currentPlaneRowVector);
        currentPlane->getColumnDirectionVector(currentPlaneColumnVector);
        currentPlane->getNormalVector(currentPlaneNormalVector);
        currentPlane->getOrigin(currentPlaneOrigin);
        delete currentPlane;

        // A partir d'aquestes dades creem la matriu de projecció,
        // que projectarà el punt donat sobre el pla actual
        vtkMatrix4x4 *projectionMatrix = vtkMatrix4x4::New();
        projectionMatrix->Identity();
        for (int column = 0; column < 3; column++)
        {
            projectionMatrix->SetElement(0, column, currentPlaneRowVector[column]);
            projectionMatrix->SetElement(1, column, currentPlaneColumnVector[column]);
            projectionMatrix->SetElement(2, column, currentPlaneNormalVector[column]);
        }

        // Un cop tenim la matriu podem fer la projeccio
        // necessitem el punt en coordenades homogenies
        double homogeneousPointToProject[4], homogeneousProjectedPoint[4];
        for (int i = 0; i < 3; i++)
        {
            // Desplacem el punt a l'origen del pla
            homogeneousPointToProject[i] = pointToProject[i] - currentPlaneOrigin[i];
        }
        homogeneousPointToProject[3] = 1.0;

        // Projectem el punt amb la matriu
        projectionMatrix->MultiplyPoint(homogeneousPointToProject, homogeneousProjectedPoint);

        //
        // CORRECIÓ VTK!
        //
        // A partir d'aquí cal corretgir l'error introduit pel mapeig que fan les vtk
        // cal sumar l'origen de la primera imatge, o el que seria el mateix, l'origen de m_mainVolume
        //
        // TODO provar si amb l'origen de m_mainVolume també funciona bé
        Image *firstImage = m_mainVolume->getImage(0);
        const double *ori = firstImage->getImagePositionPatient();

        // Segons si hem fet una reconstrucció ortogonal haurem de fer
        // alguns canvis sobre la projecció
        switch (m_lastView)
        {
            case Axial:
                for (int i = 0; i < 3; i++)
                {
                    projectedPoint[i] = homogeneousProjectedPoint[i] + ori[i];
                }
                break;

            case Sagital:
                {
                    if (vtkReconstructionHack)
                    {
                        // HACK Serveix de parxe pels casos de crani que no van bé.
                        // TODO Encara està per acabar, és una primera aproximació
                        projectionMatrix->SetElement(0, 0, 0);
                        projectionMatrix->SetElement(0, 1, 1);
                        projectionMatrix->SetElement(0, 2, 0);
                        // Projectem el punt amb la matriu
                        projectionMatrix->MultiplyPoint(homogeneousPointToProject, homogeneousProjectedPoint);
                    }

                    projectedPoint[1] = homogeneousProjectedPoint[0] + ori[1];
                    projectedPoint[2] = homogeneousProjectedPoint[1] + ori[2];
                    projectedPoint[0] = homogeneousProjectedPoint[2] + ori[0];
                }
                break;

            case Coronal:
                projectedPoint[0] = homogeneousProjectedPoint[0] + ori[0];
                projectedPoint[2] = homogeneousProjectedPoint[1] + ori[2];
                projectedPoint[1] = homogeneousProjectedPoint[2] + ori[1];
                break;
        }

        projectionMatrix->Delete();
    }
    else
    {
        DEBUG_LOG("No hi ha cap pla actual valid");
    }
}

Drawer* Q2DViewer::getDrawer() const
{
    return m_drawer;
}

bool Q2DViewer::getCurrentCursorImageCoordinate(double xyz[3])
{
    bool inside = false;
    if (!m_mainVolume)
    {
        return inside;
    }

    int position[2];
    this->getEventPosition(position);
    if (m_imagePointPicker->PickProp(position[0], position[1], getRenderer()))
    {
        inside = true;
        // Calculem el pixel trobat
        m_imagePointPicker->GetPickPosition(xyz);
        // Calculem la profunditat correcta ja que si tenim altres actors pel mig poden interferir en la mesura
        // TODO Una altre solució possible és tenir renderers separats i en el que fem el pick només tenir-hi l'image actor
        double bounds[6];
        m_imageActor->GetDisplayBounds(bounds);
        int zIndex = getZIndexForView(m_lastView);
        xyz[zIndex] = bounds[zIndex * 2];
    }
    else
    {
        DEBUG_LOG("Outside");
    }
    return inside;
}

Q2DViewer::CameraOrientationType Q2DViewer::getView() const
{
    return m_lastView;
}

void Q2DViewer::setSeedPosition(double pos[3])
{
    emit seedPositionChanged(pos[0], pos[1], pos[2]);
}

void Q2DViewer::updateAnnotationsInformation(AnnotationFlags annotation)
{
    if (!m_mainVolume)
    {
        return;
    }

    // Informació que es mostra per cada viewport
    if (annotation.testFlag(Q2DViewer::WindowInformationAnnotation))
    {
        // Informació de la finestra
        if (m_enabledAnnotations.testFlag(Q2DViewer::WindowInformationAnnotation))
        {
            m_upperLeftText = tr("%1 x %2\nWW: %5 WL: %6")
                .arg(m_mainVolume->getDimensions()[Q2DViewer::getXIndexForView(getView())])
                .arg(m_mainVolume->getDimensions()[Q2DViewer::getYIndexForView(getView())])
                .arg(MathTools::roundToNearestInteger(m_windowLevelLUTMapper->GetWindow()))
                .arg(MathTools::roundToNearestInteger(m_windowLevelLUTMapper->GetLevel()));
        }
        else
        {
            m_upperLeftText = "";
        }
        m_cornerAnnotations->SetText(2, qPrintable(m_upperLeftText));
    }

    if (annotation.testFlag(Q2DViewer::SliceAnnotation))
    {
        this->updateSliceAnnotationInformation();
    }
}

void Q2DViewer::updatePatientAnnotationInformation()
{
    if (m_mainVolume)
    {
        // TODO De moment només agafem la primera imatge perquè assumim que totes pertanyen a la mateixa sèrie
        Image *image = m_mainVolume->getImage(0);
        Series *series = image->getParentSeries();
        Study *study = series->getParentStudy();
        Patient *patient = study->getParentPatient();

        // Informació fixa
        QString seriesTime = series->getTimeAsString();
        if (seriesTime.isEmpty())
        {
            seriesTime = "--:--";
        }

        m_upperRightText = tr("%1\n%2\n%3 %4 %5\nAcc:%6\n%7\n%8").arg(series->getInstitutionName()).arg(patient->getFullName())
                         .arg(study->getPatientAge()).arg(patient->getSex()).arg(patient->getID()).arg(study->getAccessionNumber())
                         .arg(study->getDateAsString()).arg(seriesTime);

        if (series->getModality() == "MG")
        {
            m_lowerRightText.clear();
        }
        else
        {
            // Si protocol i descripció coincideixen posarem el contingut de protocol
            // Si són diferents, els fusionarem
            QString protocolName, description;
            protocolName = series->getProtocolName();
            description = series->getDescription();
            m_lowerRightText = protocolName;
            if (description != protocolName)
            {
                m_lowerRightText += "\n" + description;
            }
        }

        m_cornerAnnotations->SetText(3, qPrintable(m_upperRightText));
        m_cornerAnnotations->SetText(1, qPrintable(m_lowerRightText.trimmed()));
    }
    else
    {
        DEBUG_LOG("No hi ha un volum vàlid. No es poden inicialitzar les annotacions de texte d'informació de pacient");
    }
}

void Q2DViewer::updateSliceAnnotationInformation()
{
    Q_ASSERT(m_cornerAnnotations);
    Q_ASSERT(m_mainVolume);
    // TODO De moment assumim que totes les imatges seran de la mateixa modalitat.
    // Per evitar problemes amb el tractament de multiframe (que deixem per més endavant)
    // agafem directament la primera imatge, però cal solucionar aquest aspecte adequadament.
    Image *image = m_mainVolume->getImage(0);
    if (image->getParentSeries()->getModality() == "MG")
    {
        // Hi ha estudis que són de la modalitat MG que no s'han d'orientar. S'han afegit unes excepcions per poder-los controlar.
        bool found = false;
        QListIterator<QString> iterator(m_mammographyAutoOrientationExceptions);
        while (!found && iterator.hasNext())
        {
            found = image->getParentSeries()->getParentStudy()->getDescription().contains(iterator.next(), Qt::CaseInsensitive);
        }

        if (!found)
        {
            m_enabledAnnotations = m_enabledAnnotations & ~Q2DViewer::SliceAnnotation;

            // En la modalitat de mamografia s'ha de mostar informació especifica de la imatge que s'està mostrant.
            // Per tant si estem a la vista original agafem la imatge actual, altrament no mostrem cap informació.
            if (m_lastView == Q2DViewer::Axial)
            {
                image = m_mainVolume->getImage(m_currentSlice);
            }
            else
            {
                image = 0;
            }

            if (image)
            {
                QString projection = image->getViewCodeMeaning();
                // PS 3.16 - 2008, Page 408, Context ID 4014, View for mammography
                // TODO Tenir-ho carregat en arxius, maps, etc..
                // TODO Fer servir millor els codis [Code Value (0008,0100)] en compte dels "code meanings" podria resultar més segur
                if (projection == "medio-lateral")
                {
                    projection = "ML";
                }
                else if (projection == "medio-lateral oblique")
                {
                    projection = "MLO";
                }
                else if (projection == "latero-medial")
                {
                    projection = "LM";
                }
                else if (projection == "latero-medial oblique")
                {
                    projection = "LMO";
                }
                else if (projection == "cranio-caudal")
                {
                    projection = "CC";
                }
                else if (projection == "caudo-cranial (from below)")
                {
                    projection = "FB";
                }
                else if (projection == "superolateral to inferomedial oblique")
                {
                    projection = "SIO";
                }
                else if (projection == "exaggerated cranio-caudal")
                {
                    projection = "XCC";
                }
                else if (projection == "cranio-caudal exaggerated laterally")
                {
                    projection = "XCCL";
                }
                else if (projection == "cranio-caudal exaggerated medially")
                {
                    projection = "XCCM";
                }

                // S'han de seguir les recomanacions IHE de presentació d'imatges de Mammografia
                // IHE Techincal Framework Vol. 2 revision 8.0, apartat 4.16.4.2.2.1.1.2 Image Orientation and Justification
                PatientOrientation desiredOrientation;
                QString laterality = image->getImageLaterality();
                if (projection == "CC" || projection == "XCC" || projection == "XCCL" || projection == "XCCM" || projection == "FB")
                {
                    if (laterality == PatientOrientation::LeftLabel)
                    {
                        desiredOrientation.setLabels(PatientOrientation::AnteriorLabel, PatientOrientation::RightLabel);
                    }
                    else if (laterality == PatientOrientation::RightLabel)
                    {
                        desiredOrientation.setLabels(PatientOrientation::PosteriorLabel, PatientOrientation::LeftLabel);
                    }
                }
                else if (projection == "MLO" || projection == "ML" || projection == "LM" || projection == "LMO" || projection == "SIO")
                {
                    if (laterality == PatientOrientation::LeftLabel)
                    {
                        desiredOrientation.setLabels(PatientOrientation::AnteriorLabel, PatientOrientation::FeetLabel);
                    }
                    else if (laterality == PatientOrientation::RightLabel)
                    {
                        desiredOrientation.setLabels(PatientOrientation::PosteriorLabel, PatientOrientation::FeetLabel);
                    }
                }
                else
                {
                    DEBUG_LOG("Projecció no tractada! :: " + projection);
                }

                m_lowerRightText = laterality + " " + projection;
                // Apliquem la orientació que volem
                setImageOrientation(desiredOrientation);
            }
            else
            {
                m_lowerRightText.clear();
            }

            m_cornerAnnotations->SetText(1, qPrintable(m_lowerRightText.trimmed()));
        }
    }

    int value = m_currentSlice * m_numberOfPhases + m_currentPhase;
    if (m_numberOfPhases > 1)
    {
        this->updateSliceAnnotation((value/m_numberOfPhases) + 1, m_maxSliceValue + 1, m_currentPhase + 1, m_numberOfPhases);
    }
    else
    {
        this->updateSliceAnnotation(value + 1, m_maxSliceValue + 1);
    }
    // Si aquestes anotacions estan activades, llavors li afegim la informació de la hora de la sèrie i la imatge
    if (m_enabledAnnotations.testFlag(Q2DViewer::PatientInformationAnnotation))
    {
        // Si la vista és "AXIAL" (és a dir mostrem la imatge en l'adquisició original)
        // i tenim informació de la hora d'adquisició de la imatge, la incloem en la informació mostrada
        if (m_lastView == Axial)
        {
            Image *currentImage = getCurrentDisplayedImage();
            if (currentImage)
            {
                QString imageTime = "\n" + currentImage->getFormattedImageTime();
                if (imageTime.isEmpty())
                {
                    imageTime = "--:--";
                }
                m_cornerAnnotations->SetText(3, qPrintable(m_upperRightText + imageTime));
            }
            else
            {
                m_cornerAnnotations->SetText(3, qPrintable(m_upperRightText));
            }
        }
        else
        {
            m_cornerAnnotations->SetText(3, qPrintable(m_upperRightText));
        }
    }
}

void Q2DViewer::updateSliceAnnotation(int currentSlice, int maxSlice, int currentPhase, int maxPhase)
{
    Q_ASSERT(m_cornerAnnotations);

    // Si les anotacions estan habilitades
    if (m_enabledAnnotations.testFlag(Q2DViewer::SliceAnnotation))
    {
        QString lowerLeftText;
        // TODO Ara només tenim en compte de posar l'slice location si estem en la vista "original"
        if (m_lastView == Q2DViewer::Axial)
        {
            Image *image = getCurrentDisplayedImage();
            if (image)
            {
                QString location = image->getSliceLocation();
                if (!location.isEmpty())
                {
                    lowerLeftText = tr("Loc: %1").arg(location.toDouble(), 0, 'f', 2);
                    if (isThickSlabActive())
                    {
                        // TODO Necessitaríem funcions de més alt nivell per obtenir la imatge consecutiva d'acord amb els paràmetres
                        // de thicknes, fases, etc
                        Image *secondImage = m_mainVolume->getImage(m_currentSlice + m_slabThickness - 1, m_currentPhase);
                        if (secondImage)
                        {
                            lowerLeftText += tr("-%1").arg(secondImage->getSliceLocation().toDouble(), 0, 'f', 2);
                        }
                    }
                    lowerLeftText += "\n";
                }
            }
        }

        // Si tenim fases
        if (maxPhase > 1)
        {
            if (m_slabThickness > 1)
            {
                // TODO Potser hauríem de tenir una variable "slabRange"
                lowerLeftText += tr("Slice: %1-%2/%3 Phase: %4/%5").arg(currentSlice).arg(currentSlice + m_slabThickness - 1)
                               .arg(maxSlice).arg(currentPhase).arg(maxPhase);
            }
            else
            {
                lowerLeftText += tr("Slice: %1/%2 Phase: %3/%4").arg(currentSlice).arg(maxSlice).arg(currentPhase).arg(maxPhase);
            }
        }
        // Només llesques
        else
        {
            if (m_slabThickness > 1)
            {
                // TODO Potser hauríem de tenir una variable "slabRange"
                lowerLeftText += tr("Slice: %1-%2/%3").arg(currentSlice).arg(currentSlice + m_slabThickness - 1).arg(maxSlice);
            }
            else
            {
                lowerLeftText += tr("Slice: %1/%2").arg(currentSlice).arg(maxSlice);
            }
        }
        // Afegim el thickness de la llesca nomes si es > 0mm
        if (this->getThickness() > 0.0)
        {
            lowerLeftText += tr(" Thickness: %1 mm").arg(this->getThickness(), 0, 'f', 2);
        }

        m_cornerAnnotations->SetText(0, qPrintable(lowerLeftText));
    }
    else
    {
        m_cornerAnnotations->SetText(0, "");
    }
}

void Q2DViewer::updateDisplayExtent()
{
    Q_ASSERT(m_imageActor);

    // Ens assegurem que tenim dades vàlides
    if (!m_mainVolume->getVtkData())
    {
        return;
    }

    // TODO Potser el càlcul de l'índex de l'imatge l'hauria de fer Volume que
    // és qui coneix com es guarda la informació de la imatge, ja que si canviem la manera
    // de guardar les phases, això ja no ens valdria
    int sliceValue = getImageDataZIndexForSliceAndPhase(m_currentSlice, m_currentPhase);

    // A partir de l'extent del volum, la vista i la llesca en la que ens trobem,
    // calculem l'extent que li correspon a l'actor imatge
    int zIndex = getZIndexForView(m_lastView);
    int imageActorExtent[6];
    m_mainVolume->getWholeExtent(imageActorExtent);
    imageActorExtent[zIndex * 2] = imageActorExtent[zIndex * 2 + 1] = sliceValue;
    m_imageActor->SetDisplayExtent(imageActorExtent[0], imageActorExtent[1], imageActorExtent[2], imageActorExtent[3],
                                   imageActorExtent[4], imageActorExtent[5]);

    // TODO Si separem els renderers potser caldria aplicar-ho a cada renderer?
    getRenderer()->ResetCameraClippingRange();
}

int Q2DViewer::getImageDataZIndexForSliceAndPhase(int slice, int phase)
{
    int zIndex;
    if (isThickSlabActive())
    {
        zIndex = slice;
    }
    else
    {
        zIndex = slice * m_numberOfPhases + phase;
    }

    return zIndex;
}

void Q2DViewer::enableAnnotation(AnnotationFlags annotation, bool enable)
{
    if (enable)
    {
        m_enabledAnnotations = m_enabledAnnotations | annotation;
    }
    else
    {
        m_enabledAnnotations = m_enabledAnnotations & ~annotation;
    }

    refreshAnnotations();

    if (m_mainVolume)
    {
        this->render();
    }
}

void Q2DViewer::removeAnnotation(AnnotationFlags annotation)
{
    enableAnnotation(annotation, false);
}

void Q2DViewer::buildWindowLevelPipeline()
{
    double range[2];
    m_mainVolume->getScalarRange(range);
    DEBUG_LOG("*** Grayscale Transform Pipeline Begin ***");
    DEBUG_LOG(QString("Image Information: Bits Allocated: %1, Bits Stored: %2, Pixel Range %3 to %4, SIGNED?Pixel Representation: %5, Photometric interpretation: %6")
                 .arg(m_mainVolume->getImage(0)->getBitsAllocated()).arg(m_mainVolume->getImage(0)->getBitsStored()).arg(range[0]).arg(range[1])
                 .arg(m_mainVolume->getImage(0)->getPixelRepresentation()).arg(m_mainVolume->getImage(0)->getPhotometricInterpretation()));
    // Fins que no implementem Presentation states aquest serà el cas que sempre s'executarà el 100% dels casos
    if (isThickSlabActive())
    {
        DEBUG_LOG("Grayscale pipeline: Source Data -> ThickSlab -> [Window Level] -> Output ");
        m_windowLevelLUTMapper->SetInput(m_thickSlabProjectionFilter->GetOutput());
    }
    else
    {
        DEBUG_LOG("Grayscale pipeline: Source Data -> [Window Level] -> Output ");
        m_windowLevelLUTMapper->SetInput(m_mainVolume->getVtkData());
    }

    m_imageActor->SetInput(m_windowLevelLUTMapper->GetOutput());
}

void Q2DViewer::setSlabProjectionMode(int projectionMode)
{
    m_slabProjectionMode = projectionMode;
    m_thickSlabProjectionFilter->SetAccumulatorType(static_cast<AccumulatorFactory::AccumulatorType>(m_slabProjectionMode));
    updateDisplayExtent();
    this->render();
}

int Q2DViewer::getSlabProjectionMode() const
{
    return m_slabProjectionMode;
}

void Q2DViewer::setSlabThickness(int thickness)
{
    // Primera aproximació per evitar error dades de primitives: a l'activar o desactivar l'slabthickness, esborrem primitives
    if (thickness != m_slabThickness)
    {
        this->getDrawer()->removeAllPrimitives();
    }

    computeRangeAndSlice(thickness);
    // TODO Comprovar aquest pipeline si és millor calcular ara o més tard
    if (m_slabThickness == 1 && isThickSlabActive())
    {
        DEBUG_LOG("Desactivem thick Slab i resetejem pipeline normal");
        m_thickSlabActive = false;
        // Resetejem el pipeline
        buildWindowLevelPipeline();
        updateDisplayExtent();
        updateSliceAnnotationInformation();
        this->render();
    }
    // La comprovacio es per constuir el pipeline nomes el primer cop
    if (m_slabThickness > 1 && !isThickSlabActive())
    {
        DEBUG_LOG("Activem thick Slab i resetejem pipeline amb thickSlab");
        m_thickSlabActive = true;
        // Resetejem el pipeline
        buildWindowLevelPipeline();
    }

    m_lastSlabSlice = m_currentSlice + m_slabThickness - 1;

    if (isThickSlabActive())
    {
        m_thickSlabProjectionFilter->SetFirstSlice(m_firstSlabSlice * m_numberOfPhases + m_currentPhase);
        m_thickSlabProjectionFilter->SetNumberOfSlicesToProject(m_slabThickness);
        updateDisplayExtent();
        updateSliceAnnotationInformation();
        this->render();
    }

    // TODO és del tot correcte que vagi aquí aquesta crida?
    // Tal com està posat se suposa que sempre el valor de thickness ha
    // canviat i podria ser que no, seria més adequat posar-ho a computerangeAndSlice?
    emit slabThicknessChanged(m_slabThickness);
}

int Q2DViewer::getSlabThickness() const
{
    return m_slabThickness;
}

void Q2DViewer::enableThickSlab(bool enable)
{
    if (!enable)
    {
        setSlabThickness(1);
    }
    else
    {
        setSlabThickness(m_slabThickness);
    }
}

bool Q2DViewer::isThickSlabActive() const
{
    return m_thickSlabActive;
}

void Q2DViewer::computeRangeAndSlice(int newSlabThickness)
{
    // Checking del nou valor
    if (newSlabThickness < 1)
    {
        DEBUG_LOG(" valor invàlid de thickness. Ha de ser >= 1 !!!!!");
        return;
    }
    if (newSlabThickness == m_slabThickness)
    {
        DEBUG_LOG(" tenim el mateix slab thickness, no canviem res ");
        return;
    }
    if (newSlabThickness > m_maxSliceValue + 1)
    {
        DEBUG_LOG(" el nou thickness supera el thickness màxim, tot queda igual ");
        // TODO Podríem aplicar newSlabThickness=m_maxSliceValue+1
        return;
    }
    if (newSlabThickness == 1)
    {
        m_slabThickness = 1;
        return;
    }

    int difference = newSlabThickness - m_slabThickness;
    // Si la diferència és positiva, augmentem el thickness
    if (difference > 0)
    {
        // Divisió entera!
        m_firstSlabSlice -= difference / 2;
        m_lastSlabSlice += difference / 2;

        // Si la diferència és senar creix més per un dels límits
        if ((difference % 2) != 0)
        {
            // Si el thickness actual és parell creixem per sota
            if ((m_slabThickness % 2) == 0)
            {
                m_firstSlabSlice--;
            }
            // Sinó creixem per dalt
            else
            {
                m_lastSlabSlice++;
            }
        }
        // Check per si ens passem de rang superior o inferior
        if (m_firstSlabSlice < this->getMinimumSlice())
        {
            // Si ens passem per sota, cal compensar creixent per dalt
            m_lastSlabSlice = this->getMinimumSlice() + newSlabThickness - 1;
            // Queda al límit inferior
            m_firstSlabSlice = this->getMinimumSlice();
        }
        else if (m_lastSlabSlice > m_maxSliceValue)
        {
            // Si ens passem per dalt, cal compensar creixent per sota
            m_firstSlabSlice = m_maxSliceValue - newSlabThickness + 1;
            m_lastSlabSlice = m_maxSliceValue;
        }
    }
    // La diferència és negativa, decreix el thickness
    else
    {
        // La convertim a positiva per conveniència
        difference *= -1;
        m_firstSlabSlice += difference / 2;
        m_lastSlabSlice -= difference / 2;

        // Si la diferència és senar decreix més per un dels límits
        if ((difference % 2) != 0)
        {
            // Si el thickness actual és parell decreixem per amunt
            if ((m_slabThickness%2) == 0)
            {
                m_lastSlabSlice--;
            }
            // Sinó decreixem per avall
            else
            {
                m_firstSlabSlice++;
            }
        }
    }
    // Actualitzem el thickness
    m_slabThickness = newSlabThickness;
    // Actualitzem la llesca
    m_currentSlice = m_firstSlabSlice;
}

void Q2DViewer::checkAndUpdateSliceValue(int value)
{
    if (value < 0)
    {
        m_currentSlice = m_maxSliceValue - m_slabThickness + 1;
    }
    else if (value + m_slabThickness - 1 > m_maxSliceValue)
    {
        m_currentSlice = 0;
    }
    else
    {
        m_currentSlice = value;
    }

    m_firstSlabSlice = m_currentSlice;
    m_lastSlabSlice = m_firstSlabSlice + m_slabThickness;
}

void Q2DViewer::putCoordinateInCurrentImageBounds(double xyz[3])
{
    double bounds[6];
    m_imageActor->GetBounds(bounds);

    int xIndex = getXIndexForView(m_lastView);
    int yIndex = getYIndexForView(m_lastView);

    // Comprovarem que estigui dins dels límits 2D de la imatge
    // La x està per sota del mínim
    if (xyz[xIndex] < bounds[xIndex * 2])
    {
        xyz[xIndex] = bounds[xIndex * 2];
    }
    // La x està per sobre del màxim
    else if (xyz[xIndex] > bounds[xIndex * 2 + 1])
    {
        xyz[xIndex] = bounds[xIndex * 2 + 1];
    }
    // La y està per sota del mínim
    if (xyz[yIndex] < bounds[yIndex * 2])
    {
        xyz[yIndex] = bounds[yIndex * 2];
    }
    // La y està per sobre del màxim
    else if (xyz[yIndex] > bounds[yIndex * 2 + 1])
    {
        xyz[yIndex] = bounds[yIndex * 2 + 1];
    }
}

vtkImageData* Q2DViewer::getCurrentSlabProjection()
{
    return m_thickSlabProjectionFilter->GetOutput();
}

void Q2DViewer::restore()
{
    if (!m_mainVolume)
    {
        return;
    }

    // Si hi ha un volum carregant no fem el restore
    if (this->isVolumeLoadingAsynchronously())
    {
        return;
    }

    this->setInputAsynchronously(m_mainVolume, m_inputFinishedCommand);
}

void Q2DViewer::clearViewer()
{
    m_drawer->clearViewer();
}

void Q2DViewer::invertWindowLevel()
{
    // Passa el window level a negatiu o positiu, per invertir els colors
    double windowLevel[2];
    this->getCurrentWindowLevel(windowLevel);

    // Això és necessari fer-ho així i no amb setWindowLevel perquè si invertim el color de la imatge sense haver modificat abans el window/level
    // i després seleccionem un altre visor, al tornar a aquest visor, es tornaria aplicar el "default" i no el "custom"
    // Es podria arribar a fer d'una altre manera també, atacant directament als filtres del pipeline, tal com es diu al ticket #1275
    getWindowLevelData()->setCustomWindowLevel(-windowLevel[0], windowLevel[1]);
}

void Q2DViewer::alignLeft()
{
    double viewerLeft[4];
    double bounds[6];
    double motionVector[4];

    computeDisplayToWorld(0.0, 0.0, 0.0, viewerLeft);
    m_imageActor->GetBounds(bounds);
    motionVector[0] = 0.0;
    motionVector[1] = 0.0;
    motionVector[2] = 0.0;
    motionVector[3] = 0.0;

    // Càlcul del desplaçament
    switch (m_lastView)
    {
        case Axial:
            // Si la imatge està rotada o flipada, s'agafa l'altre punt
            if (m_isImageFlipped || (m_rotateFactor == 2))
            {
                motionVector[0] = bounds[1] - viewerLeft[0];
            }
            else
            {
                motionVector[0] = bounds[0] - viewerLeft[0];
            }
            break;

        case Sagital:
            motionVector[1] = bounds[2] - viewerLeft[1];
            break;

        case Coronal:
            motionVector[0] = bounds[0] - viewerLeft[0];
            break;
    }

    pan(motionVector);

    m_alignPosition = Q2DViewer::AlignLeft;
}

void Q2DViewer::alignRight()
{
    int *size;
    double viewerRight[4];
    double bounds[6];
    double motionVector[4];

    size = this->getRenderer()->GetSize();
    computeDisplayToWorld((double)size[0], 0.0, 0.0, viewerRight);
    m_imageActor->GetBounds(bounds);
    motionVector[0] = 0.0;
    motionVector[1] = 0.0;
    motionVector[2] = 0.0;
    motionVector[3] = 0.0;

    // Càlcul del desplaçament
    switch (m_lastView)
    {
        case Axial:
            // Si la imatge està rotada o flipada, s'agafa l'altre punt
            if (m_isImageFlipped || (m_rotateFactor == 2))
            {
                motionVector[0] = bounds[0] - viewerRight[0];
            }
            else
            {
                motionVector[0] = bounds[1] - viewerRight[0];
            }
            break;

        case Sagital:
            motionVector[1] = bounds[3] - viewerRight[1];
            break;

        case Coronal:
            motionVector[0] = bounds[1] - viewerRight[0];
            break;
    }

    pan(motionVector);
    m_alignPosition = Q2DViewer::AlignRight;
}

void Q2DViewer::setAlignPosition(AlignPosition alignPosition)
{
    switch (alignPosition)
    {
        case AlignRight:
            alignRight();
            break;

        case AlignLeft:
            alignLeft();
            break;

        case AlignCenter:
            m_alignPosition = Q2DViewer::AlignCenter;
            break;
    }
}

void Q2DViewer::setImageOrientation(const PatientOrientation &desiredPatientOrientation)
{
    m_imageOrientationOperationsMapper->setInitialOrientation(getCurrentDisplayedImagePatientOrientation());
    m_imageOrientationOperationsMapper->setDesiredOrientation(desiredPatientOrientation);

    // TODO ara mateix fet així és ineficient ja que es poden cridar fins a dos cops updateCamera() innecessàriament
    // Caldria refactoritzar els mètodes de rotació i flip per aplicar aquests canvis requerint un únic updateCamera()
    rotateClockWise(m_imageOrientationOperationsMapper->getNumberOfClockwiseTurnsToApply());
    if (m_imageOrientationOperationsMapper->requiresHorizontalFlip())
    {
        horizontalFlip();
    }
}

void Q2DViewer::showImageOverlays(bool enable)
{
    if (enable)
    {
        getDrawer()->enableGroup(OverlaysDrawerGroup);
    }
    else
    {
        getDrawer()->disableGroup(OverlaysDrawerGroup);
    }
}

void Q2DViewer::showDisplayShutters(bool enable)
{
    if (enable)
    {
        getDrawer()->enableGroup(DisplayShuttersDrawerGroup);
    }
    else
    {
        getDrawer()->disableGroup(DisplayShuttersDrawerGroup);
    }
}

void Q2DViewer::rotate(int times)
{
    // Si és zero no cal fer res
    if (times == 0)
    {
        return;
    }

    // Si la imatge està invertida per efecte mirall el sentit de les rotacions serà el contrari
    if (m_isImageFlipped)
    {
        times = -times;
    }

    m_rotateFactor = (m_rotateFactor + times) % 4;
}

void Q2DViewer::fitImageIntoViewport()
{
    // Obtenim els bounds de la imatge que estem visualitzant
    double bounds[6];
    m_imageActor->GetBounds(bounds);

    // Obtenim les coordenades corresponents a dues puntes oposades de la imatge
    double topCorner[3], bottomCorner[3];
    int xIndex, yIndex, zIndex;
    Q2DViewer::getXYZIndexesForView(xIndex, yIndex, zIndex, m_lastView);

    topCorner[xIndex] = bounds[xIndex * 2];
    topCorner[yIndex] = bounds[yIndex * 2];
    topCorner[zIndex] = 0.0;

    bottomCorner[xIndex] = bounds[xIndex * 2 + 1];
    bottomCorner[yIndex] = bounds[yIndex * 2 + 1];
    bottomCorner[zIndex] = 0.0;

    // Apliquem el zoom
    scaleToFit3D(topCorner, bottomCorner);
}

void Q2DViewer::setWindowLevelPreset(const QString &preset)
{
    double window;
    double level;
    int group;

    if (m_windowLevelData->getWindowLevelFromDescription(preset, window, level))
    {
        if (m_windowLevelData->getGroup(preset, group))
        {
            if (group == WindowLevelPresetsToolData::FileDefined)
            {
                m_defaultPresetToApply = m_windowLevelData->getFileDefinedPresetIndex(preset);
                if (m_lastView == Q2DViewer::Axial)
                {
                    updateDefaultPreset();
                }
                else
                {
                    setWindowLevel(window, level);
                }
            }
            else
            {
                m_defaultPresetToApply = -1;
                setWindowLevel(window, level);
            }
        }
    }
}

void Q2DViewer::updateDefaultPreset()
{
    if (m_mainVolume)
    {
        if (m_defaultPresetToApply >= 0 && m_lastView == Q2DViewer::Axial)
        {
            Image *image = getCurrentDisplayedImage();
            if (m_defaultPresetToApply < image->getNumberOfWindowLevels())
            {
                QPair<double, double> windowLevel = image->getWindowLevel(m_defaultPresetToApply);
                if (image->getPhotometricInterpretation() == "MONOCHROME1")
                {
                    setWindowLevel(-windowLevel.first, windowLevel.second);
                }
                else
                {
                    setWindowLevel(windowLevel.first, windowLevel.second);
                }
            }
        }
    }
    else
    {
        DEBUG_LOG(QString("::updateDefaultPreset() : No tenim input"));
    }
}

double Q2DViewer::getCurrentSpacingBetweenSlices()
{
    double currentSpacingBetweenSlices;
    switch (m_lastView)
    {
        case Axial:
            currentSpacingBetweenSlices = m_mainVolume->getSpacing()[2] * m_slabThickness;
            break;

        case Sagital:
            currentSpacingBetweenSlices = m_mainVolume->getSpacing()[0] * m_slabThickness;
            break;

        case Coronal:
            currentSpacingBetweenSlices = m_mainVolume->getSpacing()[1] * m_slabThickness;
            break;
    }

    return currentSpacingBetweenSlices;
}
};  // End namespace udg
