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
#include "filteroutput.h"
#include "blendfilter.h"
#include "imagepipeline.h"
#include "volumereadermanager.h"
#include "qviewercommand.h"
#include "renderqviewercommand.h"
#include "mammographyimagehelper.h"
#include "slicehandler.h"
#include "volumedisplayunit.h"
// Qt
#include <QResizeEvent>
#include <QImage>
// Include's bàsics vtk
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>
#include <vtkPropPicker.h>
#include <QVTKWidget.h>
#include <vtkWindowToImageFilter.h>
// Anotacions
#include <vtkCornerAnnotation.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkProperty2D.h>
#include <vtkProp.h>
#include <vtkImageActor.h>
#include <vtkScalarsToColors.h>
// Projecció de punts
#include <vtkMatrix4x4.h>

namespace udg {

const QString Q2DViewer::OverlaysDrawerGroup("Overlays");
const QString Q2DViewer::DummyVolumeObjectName("Dummy Volume");

Q2DViewer::Q2DViewer(QWidget *parent)
: QViewer(parent), m_overlayVolume(0), m_blender(0), m_imagePointPicker(0), m_cornerAnnotations(0), m_enabledAnnotations(Q2DViewer::AllAnnotation),
  m_overlapMethod(Q2DViewer::Blend), m_rotateFactor(0), m_applyFlip(false), m_isImageFlipped(false), m_slabProjectionMode(AccumulatorFactory::Maximum)
{
    m_volumeReaderManager = new VolumeReaderManager(this);
    m_inputFinishedCommand = NULL;
    
    connect(m_volumeReaderManager, SIGNAL(readingFinished()), SLOT(volumeReaderJobFinished()));
    connect(m_volumeReaderManager, SIGNAL(progress(int)), m_workInProgressWidget, SLOT(updateProgress(int)));

    m_mainVolumeDisplayUnit = new VolumeDisplayUnit();

    // Creem anotacions i actors
    createAnnotations();
    addActors();

    // Creem el picker per obtenir les coordenades de la imatge
    m_imagePointPicker = vtkPropPicker::New();
    m_imagePointPicker->InitializePickList();
    m_imagePointPicker->AddPickList(m_mainVolumeDisplayUnit->getImageActor());
    m_imagePointPicker->PickFromListOn();
    this->getInteractor()->SetPicker(m_imagePointPicker);

    // Creem el drawer, passant-li com a visor l'objecte this
    m_drawer = new Drawer(this);

    m_imageOrientationOperationsMapper = new ImageOrientationOperationsMapper();

    m_alignPosition = Q2DViewer::AlignCenter;

    // Inicialitzem el filtre de shutter
    m_showDisplayShutters = true;
    m_overlaysAreEnabled = true;
}

Q2DViewer::~Q2DViewer()
{
    delete m_mainVolumeDisplayUnit;

    // Fem delete de tots els objectes vtk dels que hem fet un ::New()
    m_patientOrientationTextActor[0]->Delete();
    m_patientOrientationTextActor[1]->Delete();
    m_patientOrientationTextActor[2]->Delete();
    m_patientOrientationTextActor[3]->Delete();
    m_cornerAnnotations->Delete();
    m_imagePointPicker->Delete();

    // Fem delete d'altres objectes vtk en cas que s'hagin hagut de crear
    delete m_blender;

    removeViewerBitmaps();
    
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

    applyImageOrientationChanges();
}

void Q2DViewer::rotateCounterClockWise(int times)
{
    // Almenys ha de ser 1 (-90º)
    if (times <= 0)
    {
        return;
    }

    rotate(-times);

    applyImageOrientationChanges();
}

void Q2DViewer::horizontalFlip()
{
    setFlip(true);

    applyImageOrientationChanges();
}

void Q2DViewer::verticalFlip()
{
    rotate(2);
    setFlip(true);

    applyImageOrientationChanges();
}

PatientOrientation Q2DViewer::getCurrentDisplayedImagePatientOrientation() const
{
    // Si no estem a la vista axial (adquisició original) obtindrem 
    // la orientació a través de la primera imatge
    int index = (this->getCurrentViewPlane() == OrthogonalPlane::XYPlane) ? getCurrentSlice() : 0;

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
    switch (this->getCurrentViewPlane())
    {
        case OrthogonalPlane::XYPlane:
            baseRowLabel = originalOrientation.getRowDirectionLabel();
            baseColumnLabel = originalOrientation.getColumnDirectionLabel();
            break;

        case OrthogonalPlane::YZPlane:
            baseRowLabel = originalOrientation.getColumnDirectionLabel();
             // TODO Tenim la normal "al revés", en realitat hauria de ser el contrari
            baseColumnLabel = PatientOrientation::getOppositeOrientationLabel(originalOrientation.getNormalDirectionLabel());
            break;

        case OrthogonalPlane::XZPlane:
            baseRowLabel = originalOrientation.getRowDirectionLabel();
            // TODO Tenim la normal "al revés", en realitat hauria de ser el contrari
            baseColumnLabel = PatientOrientation::getOppositeOrientationLabel(originalOrientation.getNormalDirectionLabel());
            break;
    }

    // Ara caldrà escollir les etiquetes corresponents en funció de les rotacions i flips
    QString rowLabel;
    QString columnLabel;
    int absoluteRotateFactor = (4 + m_rotateFactor) % 4;
    if (this->getCurrentViewPlane() == OrthogonalPlane::YZPlane || this->getCurrentViewPlane() == OrthogonalPlane::XZPlane)
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

void Q2DViewer::setDefaultOrientation(AnatomicalPlane::AnatomicalPlaneType anatomicalPlane)
{
    if (!m_mainVolume)
    {
        return;
    }

    // We apply the standard orientation for the desired projection unless original acquisition is axial and is the same as the desired one
    // because when the patient is acquired in prono position we don't want to change the acquisition orientation and thus respect the acquired one
    AnatomicalPlane::AnatomicalPlaneType acquisitionPlane = m_mainVolume->getAcquisitionPlane();
    if (acquisitionPlane != AnatomicalPlane::Axial || acquisitionPlane != anatomicalPlane)
    {
        PatientOrientation desiredOrientation = AnatomicalPlane::getDefaultRadiologicalOrienation(anatomicalPlane);
        setImageOrientation(desiredOrientation);
    }
}

void Q2DViewer::updatePreferredImageOrientation()
{
    Image *image = getCurrentDisplayedImage();
    
    if (!image)
    {
        return;
    }
    
    // Hi ha estudis que són de la modalitat MG que no s'han d'orientar. S'han afegit unes excepcions per poder-los controlar.
    MammographyImageHelper mammographyImageHelper;
    if (mammographyImageHelper.isStandardMammographyImage(image))
    {
        PatientOrientation desiredOrientation = mammographyImageHelper.getImageOrientationPresentation(image);

        // Apliquem la orientació que volem
        setImageOrientation(desiredOrientation);
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

double Q2DViewer::getCurrentSliceThickness() const
{
    return m_mainVolumeDisplayUnit->getSliceHandler()->getSliceThickness();
}

int Q2DViewer::getMinimumSlice() const
{
    return m_mainVolumeDisplayUnit->getSliceHandler()->getMinimumSlice();
}

int Q2DViewer::getMaximumSlice() const
{
    return m_mainVolumeDisplayUnit->getSliceHandler()->getMaximumSlice();
}

void Q2DViewer::addActors()
{
    Q_ASSERT(m_cornerAnnotations);
    Q_ASSERT(m_patientOrientationTextActor[0]);
    Q_ASSERT(m_patientOrientationTextActor[1]);
    Q_ASSERT(m_patientOrientationTextActor[2]);
    Q_ASSERT(m_patientOrientationTextActor[3]);

    vtkRenderer *renderer = getRenderer();
    Q_ASSERT(renderer);
    // Anotacions de texte
    renderer->AddViewProp(m_cornerAnnotations);
    renderer->AddViewProp(m_patientOrientationTextActor[0]);
    renderer->AddViewProp(m_patientOrientationTextActor[1]);
    renderer->AddViewProp(m_patientOrientationTextActor[2]);
    renderer->AddViewProp(m_patientOrientationTextActor[3]);
    renderer->AddViewProp(m_mainVolumeDisplayUnit->getImageActor());
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

    m_volumeReaderManager->cancelReading();
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

    m_volumeReaderManager->cancelReading();
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
    delete m_inputFinishedCommand;
    m_inputFinishedCommand = NULL;
}

void Q2DViewer::loadVolumeAsynchronously(Volume *volume)
{
    this->setViewerStatus(LoadingVolume);

    m_volumeReaderManager->readVolume(volume);

    // TODO: De moment no tenim cap més remei que especificar un volume fals. La resta del viewer (i els que en depenen) s'esperen
    // tenir un volum carregat després de cridar a setInput.
    // També tenim el problema de que perquè surti al menú de botó dret com a seleccionat, cal posar-li el mateix id.
    Volume *dummyVolume = this->getDummyVolumeFromVolume(volume);
    dummyVolume->setIdentifier(volume->getIdentifier());
    this->setNewVolume(dummyVolume, false);
}

void Q2DViewer::volumeReaderJobFinished()
{
    if (m_volumeReaderManager->readingSuccess())
    {
        setNewVolumeAndExecuteCommand(m_volumeReaderManager->getVolume());
    }
    else
    {
        this->setViewerStatus(LoadingError);
        m_workInProgressWidget->showError(m_volumeReaderManager->getLastErrorMessageToUser());
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
        m_workInProgressWidget->showError(tr("There's not enough memory for the rendering process. Try to close all the open %1 windows, restart "
            "the application and try again. If the problem persists, adding more RAM memory or switching to a 64-bit operating system may solve the problem.")
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

        Volume *dummyVolume = this->getDummyVolumeFromVolume(m_mainVolume);
        dummyVolume->setIdentifier(m_mainVolume->getIdentifier());
        this->setNewVolume(dummyVolume, false);
    }
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
    // TODO: Caldria que fitRenderingIntoViewport() fos indepdent de si s'està visualitzant o no el QVTKWidget
    if (setViewerStatusToVisualizingVolume)
    {
        this->setViewerStatus(VisualizingVolume);
    }

    // HACK
    // Desactivem el rendering per tal de millorar l'eficiència del setInput ja que altrament es renderitza múltiples vegades
    enableRendering(false);
    
    if (m_mainVolume != volume)
    {
        // Al canviar de volum, eliminem overlays que poguèssim tenir anteriorment
        removeViewerBitmaps();
    }

    if (m_mainVolume)
    {
        // Al fer un nou input, les distàncies que guardava el drawer no tenen sentit, pertant s'esborren
        m_drawer->removeAllPrimitives();

        // Si el volum anterior era un dummy, l'eliminem
        if (m_mainVolume->objectName() == DummyVolumeObjectName)
        {
            delete m_mainVolume;
        }
    }

    // TODO Caldria fer neteja? bloquejar? Per tal que quedi en negre mentres es carrega el nou volum?
    m_mainVolume = volume;
    m_mainVolumeDisplayUnit->setVolume(m_mainVolume);

    this->setCurrentViewPlane(OrthogonalPlane::XYPlane);
    m_alignPosition = Q2DViewer::AlignCenter;

    // Això es fa per destruir el blender en cas que ja hi hagi algun input i es vulgui canviar
    delete m_blender;
    m_blender = 0;

    initializeThickSlab();
    this->updateDisplayShutterMask();

    printVolumeInformation();

    updatePatientAnnotationInformation();
    this->enableAnnotation(m_enabledAnnotations);

    // Actualitzem la informació de window level
    this->updateWindowLevelData();
    loadOverlays(volume);

    // Reset the view to the acquisition plane
    resetViewToAcquisitionPlane();
    // HACK
    // S'activa el rendering de nou per tal de que es renderitzi l'escena
    enableRendering(true);

    // Indiquem el canvi de volum
    emit volumeChanged(m_mainVolume);
}

void Q2DViewer::removeViewerBitmaps()
{
    // Eliminem els bitmaps que teníem fins ara
    foreach (DrawerBitmap *bitmap, m_viewerBitmaps)
    {
        bitmap->decreaseReferenceCount();
        delete bitmap;
    }
    m_viewerBitmaps.clear();
}

void Q2DViewer::loadOverlays(Volume *volume)
{
    if (!volume)
    {
        return;
    }

    if (volume->objectName() == DummyVolumeObjectName)
    {
        return;
    }

    double volumeSpacing[3];
    volume->getSpacing(volumeSpacing);
    double volumeOrigin[3];
    volume->getOrigin(volumeOrigin);
    
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
                    // Calculem l'origen del bitmap corresponent a aquesta imatge
                    double imageOrigin[3];
                    imageOrigin[0] = volumeOrigin[0];
                    imageOrigin[1] = volumeOrigin[1];
                    imageOrigin[2] = volumeOrigin[2] + sliceIndex * volumeSpacing[2];
                    // Creem els bitmaps
                    foreach(const ImageOverlay &overlay, image->getOverlaysSplit())
                    {
                        DrawerBitmap *overlayBitmap = overlay.getAsDrawerBitmap(imageOrigin, volumeSpacing);
                        // Inicialment no serà, segons la llesca en que ens trobem el Drawer decidirà sobre la seva visibilitat
                        overlayBitmap->setVisibility(false);
                        // La primitiva no es podrà esborrar amb les tools
                        overlayBitmap->setErasable(false);
                        overlayBitmap->increaseReferenceCount();
                        getDrawer()->draw(overlayBitmap, OrthogonalPlane::XYPlane, sliceIndex);
                        getDrawer()->addToGroup(overlayBitmap, OverlaysDrawerGroup);
                        m_viewerBitmaps << overlayBitmap;
                    }
                }
            }
        }
    }

    if (!m_overlaysAreEnabled)
    {
        showImageOverlays(m_overlaysAreEnabled);
    }
}

void Q2DViewer::setOverlayInput(Volume *volume)
{
    m_overlayVolume = volume;
    if (m_overlapMethod == Blend)
    {
        if (!m_blender)
        {
            m_blender = new BlendFilter();
            m_blender->setBase(m_mainVolume->getVtkData());
        }
        m_blender->setOverlay(m_overlayVolume->getVtkData());
        m_blender->setOverlayOpacity(1.0 - m_overlayOpacity);
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
            m_mainVolumeDisplayUnit->getImagePipeline()->setInput(m_mainVolume->getVtkData());
            // TODO aquest procediment és possible que sigui insuficient,
            // caldria unficar el pipeline en un mateix mètode
            break;

        case Blend:
            // TODO Revisar la manera de donar-li l'input d'un blending al visualitzador
            // Aquest procediment podria ser insuficent de cares a com estigui construit el pipeline
            m_blender->update();
            m_mainVolumeDisplayUnit->getImagePipeline()->setInput(m_blender->getOutput());
            break;
    }

    emit overlayModified();
}

void Q2DViewer::setOverlayOpacity(double opacity)
{
    m_overlayOpacity = opacity;
}

void Q2DViewer::resetView(const OrthogonalPlane &view)
{
    // First we compute which slab thickness should be applied after the view has been reseted
    int desiredSlabSlices = 1;
    if (isThickSlabActive())
    {
        // In case thick slab is enabled, we should keep the slice thickness, 
        // so the proper number of slices of the thick slab should be computed for the new view
        double currentSlabThickness = getCurrentSliceThickness();
        int viewIndex = view.getZIndexForView();
        double zSpacingAfterReset = m_mainVolume->getSpacing()[viewIndex];
        desiredSlabSlices = qRound(currentSlabThickness / zSpacingAfterReset);
    }
    
    // Important, cal desactivar el thickslab abans de fer m_currentViewPlane = view, sinó falla amb l'update extent
    disableThickSlab();
    this->setCurrentViewPlane(view);
    updateAnnotationsInformation(Q2DViewer::WindowInformationAnnotation);
    
    // Reiniciem valors per defecte de la càmera
    m_rotateFactor = 0;
    setFlip(false);
    m_isImageFlipped = false;
    m_alignPosition = Q2DViewer::AlignCenter;
    
    resetCamera();
    
    if (m_mainVolume)
    {
        m_mainVolumeDisplayUnit->setViewPlane(view);

        // Ara adaptem els actors a la nova configuració de la càmara perquè siguin visibles
        // TODO Això s'hauria d'encapsular en un mètode tipu "resetDisplayExtent()"
        // HACK! Necessari perquè s'actualitzi la llesca correctament
        m_mainVolumeDisplayUnit->getSliceHandler()->setSlice(0);
        updateDisplayExtent();
        getRenderer()->ResetCamera();
        // Fins aquí seria el mètode "resetDisplayExtent()"
        
        // Calculem la llesca que cal mostrar segons la vista escollida
        int initialSliceIndex = 0;
        if (this->getCurrentViewPlane() == OrthogonalPlane::YZPlane || this->getCurrentViewPlane() == OrthogonalPlane::XZPlane)
        {
            initialSliceIndex = m_mainVolumeDisplayUnit->getSliceHandler()->getMaximumSlice() / 2;
        }
        // HACK! Necessari perquè s'actualitzi la llesca correctament
        m_mainVolumeDisplayUnit->getSliceHandler()->setSlice(-1);
        setSlice(initialSliceIndex);
    }
    
    // Thick Slab, li indiquem la direcció de projecció actual
    m_mainVolumeDisplayUnit->getImagePipeline()->setProjectionAxis(this->getCurrentViewPlane());
    setSlabThickness(desiredSlabSlices);

    emit viewChanged(this->getCurrentViewPlane());
}

void Q2DViewer::updateCamera()
{
    if (m_mainVolume)
    {
        vtkCamera *camera = getActiveCamera();
        Q_ASSERT(camera);

        double roll = 0.0;
        switch (this->getCurrentViewPlane())
        {
            case OrthogonalPlane::XYPlane:
                if (m_isImageFlipped)
                {
                    roll = m_rotateFactor * 90. + 180.;
                }
                else
                {
                    roll = -m_rotateFactor * 90. + 180.;
                }
                break;

            case OrthogonalPlane::YZPlane:
                if (m_isImageFlipped)
                {
                    roll = m_rotateFactor * 90. - 90.;
                }
                else
                {
                    roll = -m_rotateFactor * 90. - 90.;
                }
                break;

            case OrthogonalPlane::XZPlane:
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
            switch (this->getCurrentViewPlane())
            {
                // HACK Aquest hack esta relacionat amb els de getCurrentDisplayedImageOrientationLabels()
                // és un petit truc perque la imatge quedi orientada correctament. Caldria
                // veure si en el refactoring podem fer-ho d'una forma millor
                case OrthogonalPlane::YZPlane:
                case OrthogonalPlane::XZPlane:
                    rotate(-2);
                    break;

                default:
                    break;
            }

            this->getRenderer()->ResetCameraClippingRange();
            setFlip(false);
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

    // Ajustem els paràmetres de la càmera segons la vista
    double cameraRoll = 0.0;
    switch (this->getCurrentViewPlane())
    {
        case OrthogonalPlane::XYPlane:
            cameraRoll = 180.0;
            break;

        case OrthogonalPlane::YZPlane:
            cameraRoll = -90.0;
            break;

        case OrthogonalPlane::XZPlane:
            cameraRoll = 0.0;
            break;
    }

    camera->SetRoll(cameraRoll);
    setCameraViewPlane(this->getCurrentViewPlane());
}

void Q2DViewer::setSlice(int value)
{
    if (this->m_mainVolume)
    {
        m_mainVolumeDisplayUnit->getSliceHandler()->setSlice(value);
        m_mainVolumeDisplayUnit->getImagePipeline()->setSlice(m_mainVolume->getImageIndex(getCurrentSlice(), getCurrentPhase()));
        if (isThickSlabActive())
        {
            // Si hi ha el thickslab activat, eliminem totes les roi's. És la decisió ràpida que s'ha près.
            this->getDrawer()->removeAllPrimitives();
        }
        else
        {
            updateDisplayShutterMask();
        }
        
        this->updateDisplayExtent();
        updateCurrentImageDefaultPresets();
        updateSliceAnnotationInformation();
        updatePreferredImageOrientation();
        updatePatientOrientationAnnotation();
        emit sliceChanged(getCurrentSlice());
        render();
    }
}

void Q2DViewer::setPhase(int value)
{
    // Comprovació de rang
    if (m_mainVolume)
    {
        m_mainVolumeDisplayUnit->getSliceHandler()->setPhase(value);
        m_mainVolumeDisplayUnit->getImagePipeline()->setSlice(m_mainVolume->getImageIndex(getCurrentSlice(), getCurrentPhase()));
        this->updateDisplayExtent();
        updateCurrentImageDefaultPresets();
        updateSliceAnnotationInformation();
        updatePreferredImageOrientation();
        emit phaseChanged(getCurrentPhase());
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
    m_mainVolumeDisplayUnit->getImagePipeline()->setInput(m_mainVolume->getVtkData());
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
                fitRenderingIntoViewport();
                break;
        }
    }
}

void Q2DViewer::setWindowLevel(double window, double level)
{
    if (m_mainVolume)
    {
        if (m_mainVolumeDisplayUnit->getImagePipeline()->setWindowLevel(window,level))
        {
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
    m_mainVolumeDisplayUnit->getImagePipeline()->setTransferFunction(m_transferFunction);
}

void Q2DViewer::getCurrentWindowLevel(double wl[2])
{
    if (m_mainVolume)
    {
        m_mainVolumeDisplayUnit->getImagePipeline()->getCurrentWindowLevel(wl);
    }
    else
    {
        DEBUG_LOG("::getCurrentWindowLevel() : No tenim input ");
    }
}

int Q2DViewer::getCurrentSlice() const
{
    return m_mainVolumeDisplayUnit->getSliceHandler()->getCurrentSlice();
}

int Q2DViewer::getCurrentPhase() const
{
    return m_mainVolumeDisplayUnit->getSliceHandler()->getCurrentPhase();
}

Image* Q2DViewer::getCurrentDisplayedImage() const
{
    if (this->getCurrentViewPlane() == OrthogonalPlane::XYPlane)
    {
        return m_mainVolume->getImage(getCurrentSlice(), getCurrentPhase());
    }
    else
    {
        return 0;
    }
}

ImagePlane* Q2DViewer::getCurrentImagePlane(bool vtkReconstructionHack)
{
    if (!m_mainVolume)
    {
        return 0;
    }
    
    return m_mainVolume->getImagePlane(getCurrentSlice(), this->getCurrentViewPlane(), vtkReconstructionHack);
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
        switch (this->getCurrentViewPlane())
        {
            case OrthogonalPlane::XYPlane:
                for (int i = 0; i < 3; i++)
                {
                    projectedPoint[i] = homogeneousProjectedPoint[i] + ori[i];
                }
                break;

            case OrthogonalPlane::YZPlane:
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

            case OrthogonalPlane::XZPlane:
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

    QPoint position = this->getEventPosition();
    if (m_imagePointPicker->Pick(position.x(), position.y(), 0.0, getRenderer()))
    {
        inside = true;
        // Calculem el pixel trobat
        m_imagePointPicker->GetPickPosition(xyz);
        // Calculem la profunditat correcta. S'ha de tenir en compte que en el cas que tinguem fases
        // vtk no n'és conscient (cada fase es desplaça en la profunditat z com si fos una imatge més)
        // i si no fèssim aquest càlcul, estaríem donant una coordenada Z incorrecta
        int zIndex = this->getCurrentViewPlane().getZIndexForView();
        double zSpacing = m_mainVolume->getSpacing()[zIndex];
        double zOrigin = m_mainVolume->getOrigin()[zIndex];
        xyz[zIndex] =  zOrigin + zSpacing * getCurrentSlice();
    }
    else
    {
        DEBUG_LOG("Outside");
    }
    return inside;
}

const OrthogonalPlane& Q2DViewer::getView() const
{
    return this->getCurrentViewPlane();
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
            double windowLevel[2];
            m_mainVolumeDisplayUnit->getImagePipeline()->getCurrentWindowLevel(windowLevel);
            m_upperLeftText = tr("%1 x %2\nWW: %5 WL: %6")
                .arg(m_mainVolume->getDimensions()[getView().getXIndexForView()])
                .arg(m_mainVolume->getDimensions()[getView().getYIndexForView()])
                .arg(MathTools::roundToNearestInteger(windowLevel[0]))
                .arg(MathTools::roundToNearestInteger(windowLevel[1]));
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

        m_upperRightText = series->getInstitutionName() + "\n";
        m_upperRightText += patient->getFullName() + "\n";
        m_upperRightText += QString("%1 %2 %3\n").arg(study->getPatientAge()).arg(patient->getSex()).arg(patient->getID());
        if (!study->getAccessionNumber().isEmpty())
        {
            m_upperRightText += tr("Acc: %1\n").arg(study->getAccessionNumber());
        }
        else
        {
            m_upperRightText += "\n";
        }
        m_upperRightText += study->getDate().toString(Qt::ISODate) + "\n";
        m_upperRightText += seriesTime;

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
    
    Image *image = getCurrentDisplayedImage();
    
    MammographyImageHelper mammographyImageHelper;
    if (mammographyImageHelper.isStandardMammographyImage(image))
    {
        // Specific mammography annotations should be displayed
        m_enabledAnnotations = m_enabledAnnotations & ~Q2DViewer::SliceAnnotation;
        
        QString laterality = image->getImageLaterality();
        QString projection = mammographyImageHelper.getMammographyProjectionLabel(image);

        m_lowerRightText = laterality + " " + projection;
        
        m_cornerAnnotations->SetText(1, qPrintable(m_lowerRightText.trimmed()));
    }
    else
    {
        updateLateralityAnnotationInformation();
    }

    int value = m_mainVolume->getImageIndex(getCurrentSlice(), getCurrentPhase());
    if (m_mainVolumeDisplayUnit->getSliceHandler()->getNumberOfPhases() > 1)
    {
        this->updateSliceAnnotation((value / m_mainVolumeDisplayUnit->getSliceHandler()->getNumberOfPhases()) + 1,
                                    m_mainVolumeDisplayUnit->getSliceHandler()->getMaximumSlice() + 1, getCurrentPhase() + 1,
                                    m_mainVolumeDisplayUnit->getSliceHandler()->getNumberOfPhases());
    }
    else
    {
        this->updateSliceAnnotation(value + 1, m_mainVolumeDisplayUnit->getSliceHandler()->getMaximumSlice() + 1);
    }
    
    updatePatientInformationAnnotation();
}

void Q2DViewer::updateLateralityAnnotationInformation()
{
    QChar laterality = getCurrentDisplayedImageLaterality();
    if (!laterality.isNull() && !laterality.isSpace())
    {
        QString lateralityAnnotation = "Lat: " + QString(laterality);
            
        if (m_lowerRightText.trimmed().isEmpty())
        {
            m_cornerAnnotations->SetText(1, qPrintable(lateralityAnnotation));
        }
        else
        {
            m_cornerAnnotations->SetText(1, qPrintable(lateralityAnnotation + "\n" + m_lowerRightText.trimmed()));
        }
    }
    else
    {
        m_cornerAnnotations->SetText(1, qPrintable(m_lowerRightText.trimmed()));
    }
}

void Q2DViewer::updatePatientInformationAnnotation()
{
    // Si aquestes anotacions estan activades, llavors li afegim la informació de la hora de la sèrie i la imatge
    if (m_enabledAnnotations.testFlag(Q2DViewer::PatientInformationAnnotation))
    {
        // Si la vista és "AXIAL" (és a dir mostrem la imatge en l'adquisició original)
        // i tenim informació de la hora d'adquisició de la imatge, la incloem en la informació mostrada
        if (this->getCurrentViewPlane() == OrthogonalPlane::XYPlane)
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

QChar Q2DViewer::getCurrentDisplayedImageLaterality() const
{
    QChar laterality;
    bool searchSeriesLateralityOnly = false;
    Image *currentImage = getCurrentDisplayedImage();
    if (!currentImage)
    {
        currentImage = m_mainVolume->getImage(0);
        searchSeriesLateralityOnly = true;
    }
    
    if (currentImage)
    {
        if (!searchSeriesLateralityOnly)
        {
            laterality = currentImage->getImageLaterality();
        }

        if (laterality.isNull() || laterality.isSpace())
        {
            laterality = currentImage->getParentSeries()->getLaterality();
        }
    }
    
    return laterality;
}

void Q2DViewer::updateSliceAnnotation(int currentSlice, int maxSlice, int currentPhase, int maxPhase)
{
    Q_ASSERT(m_cornerAnnotations);

    // Si les anotacions estan habilitades
    if (m_enabledAnnotations.testFlag(Q2DViewer::SliceAnnotation))
    {
        QString lowerLeftText;
        // TODO Ara només tenim en compte de posar l'slice location si estem en la vista "original"
        if (this->getCurrentViewPlane() == OrthogonalPlane::XYPlane)
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
                        Image *secondImage = m_mainVolume->getImage(m_mainVolumeDisplayUnit->getSliceHandler()->getLastSlabSlice(), getCurrentPhase());
                        if (secondImage)
                        {
                            lowerLeftText += tr("-%1").arg(secondImage->getSliceLocation().toDouble(), 0, 'f', 2);
                        }
                    }
                    lowerLeftText += "\n";
                }
            }
        }

        if (m_mainVolumeDisplayUnit->getSliceHandler()->getSlabThickness() > 1)
        {
            // TODO Potser hauríem de tenir una variable "slabRange"
            lowerLeftText += tr("Slice: %1-%2/%3").arg(currentSlice).arg(m_mainVolumeDisplayUnit->getSliceHandler()->getLastSlabSlice() + 1).arg(maxSlice);
        }
        else
        {
            lowerLeftText += tr("Slice: %1/%2").arg(currentSlice).arg(maxSlice);
        }
        
        // Si tenim fases
        if (maxPhase > 1)
        {
            lowerLeftText += tr(" Phase: %1/%2").arg(currentPhase).arg(maxPhase);
        }
        
        // Afegim el thickness de la llesca nomes si es > 0mm
        if (this->getCurrentSliceThickness() > 0.0)
        {
            lowerLeftText += tr(" Thickness: %1 mm").arg(this->getCurrentSliceThickness(), 0, 'f', 2);
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
    // Ens assegurem que tenim dades vàlides
    if (!m_mainVolume->isPixelDataLoaded())
    {
        return;
    }

    int sliceValue = m_mainVolume->getImageIndex(getCurrentSlice(), getCurrentPhase());

    // A partir de l'extent del volum, la vista i la llesca en la que ens trobem,
    // calculem l'extent que li correspon a l'actor imatge
    int zIndex = this->getCurrentViewPlane().getZIndexForView();
    int imageActorExtent[6];
    m_mainVolume->getWholeExtent(imageActorExtent);
    imageActorExtent[zIndex * 2] = imageActorExtent[zIndex * 2 + 1] = sliceValue;
    m_mainVolumeDisplayUnit->getImageActor()->SetDisplayExtent(imageActorExtent[0], imageActorExtent[1], imageActorExtent[2], imageActorExtent[3],
                                                               imageActorExtent[4], imageActorExtent[5]);

    // TODO Si separem els renderers potser caldria aplicar-ho a cada renderer?
    getRenderer()->ResetCameraClippingRange();
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

void Q2DViewer::printVolumeInformation()
{
    double range[2];
    m_mainVolume->getScalarRange(range);
    DEBUG_LOG("*** Grayscale Transform Pipeline Begin ***");
    DEBUG_LOG(QString("Image Information: Bits Allocated: %1, Bits Stored: %2, Pixel Range %3 to %4, SIGNED?Pixel Representation: %5, Photometric interpretation: %6")
                 .arg(m_mainVolume->getImage(0)->getBitsAllocated()).arg(m_mainVolume->getImage(0)->getBitsStored()).arg(range[0]).arg(range[1])
                 .arg(m_mainVolume->getImage(0)->getPixelRepresentation()).arg(m_mainVolume->getImage(0)->getPhotometricInterpretation()));
    // Fins que no implementem Presentation states aquest serà el cas que sempre s'executarà el 100% dels casos
}

void Q2DViewer::setSlabProjectionMode(int projectionMode)
{
    m_slabProjectionMode = projectionMode;
    m_mainVolumeDisplayUnit->getImagePipeline()->setSlabProjectionMode(static_cast<AccumulatorFactory::AccumulatorType>(m_slabProjectionMode));
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
    if (thickness != m_mainVolumeDisplayUnit->getSliceHandler()->getSlabThickness())
    {
        this->getDrawer()->removeAllPrimitives();
    }

    m_mainVolumeDisplayUnit->getSliceHandler()->setSlabThickness(thickness);

    m_mainVolumeDisplayUnit->getImagePipeline()->setSlice(m_mainVolume->getImageIndex(getCurrentSlice(), getCurrentPhase()));
    m_mainVolumeDisplayUnit->getImagePipeline()->setSlabThickness(m_mainVolumeDisplayUnit->getSliceHandler()->getSlabThickness());
    updateDisplayExtent();
    updateSliceAnnotationInformation();
    this->render();

    // TODO és del tot correcte que vagi aquí aquesta crida?
    // Tal com està posat se suposa que sempre el valor de thickness ha
    // canviat i podria ser que no, seria més adequat posar-ho a computerangeAndSlice?
    emit slabThicknessChanged(m_mainVolumeDisplayUnit->getSliceHandler()->getSlabThickness());
}

int Q2DViewer::getSlabThickness() const
{
    return m_mainVolumeDisplayUnit->getSliceHandler()->getSlabThickness();
}

void Q2DViewer::disableThickSlab()
{
    setSlabThickness(1);
}

bool Q2DViewer::isThickSlabActive() const
{
    return m_mainVolumeDisplayUnit->getSliceHandler()->getSlabThickness() > 1;
}

void Q2DViewer::initializeThickSlab()
{
    m_mainVolumeDisplayUnit->getSliceHandler()->setSlabThickness(1);
    m_mainVolumeDisplayUnit->getSliceHandler()->setSlice(0);
    
    m_mainVolumeDisplayUnit->getImagePipeline()->setInput(m_mainVolume->getVtkData());
    m_mainVolumeDisplayUnit->getImagePipeline()->setProjectionAxis(this->getCurrentViewPlane());
    m_mainVolumeDisplayUnit->getImagePipeline()->setSlabProjectionMode((AccumulatorFactory::AccumulatorType) m_slabProjectionMode);
    m_mainVolumeDisplayUnit->getImagePipeline()->setSlice(m_mainVolume->getImageIndex(getCurrentSlice(), getCurrentPhase()));
    m_mainVolumeDisplayUnit->getImagePipeline()->setSlabThickness(m_mainVolumeDisplayUnit->getSliceHandler()->getSlabThickness());
    m_mainVolumeDisplayUnit->getImagePipeline()->setSlabStride(m_mainVolumeDisplayUnit->getSliceHandler()->getNumberOfPhases());
}

void Q2DViewer::putCoordinateInCurrentImageBounds(double xyz[3])
{
    double bounds[6];
    m_mainVolumeDisplayUnit->getImageActor()->GetBounds(bounds);

    int xIndex = this->getCurrentViewPlane().getXIndexForView();
    int yIndex = this->getCurrentViewPlane().getYIndexForView();

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
    return m_mainVolumeDisplayUnit->getImagePipeline()->getSlabProjectionOutput();
}

void Q2DViewer::restore()
{
    if (!m_mainVolume)
    {
        return;
    }

    // Si hi ha un volum carregant no fem el restore
    if (m_volumeReaderManager->isReading())
    {
        return;
    }

    // HACK
    // Desactivem el rendering per tal de millorar l'eficiència de tornar a executar el pipeline,
    // ja que altrament es renderitza múltiples vegades i provoca efectes indesitjats com el flickering
    enableRendering(false);
    
    // Esborrem les anotacions
    if (m_mainVolume)
    {
        m_drawer->removeAllPrimitives();
    }

    // TODO The view is reseted to the acquisition plane but could be more appropiate having a
    // defined command to place the image properly by default depending on the input if no one is defined 
    // Take into account this call disables thickslab
    resetViewToAcquisitionPlane();
    updateWindowLevelData();
    
    // HACK Restaurem el rendering
    enableRendering(true);
    // Apliquem el command
    executeInputFinishedCommand();
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
    setAlignPosition(AlignLeft);
}

void Q2DViewer::alignRight()
{
    setAlignPosition(AlignRight);
}

void Q2DViewer::setAlignPosition(AlignPosition alignPosition)
{
    m_alignPosition = alignPosition;
    
    if (alignPosition == AlignCenter)
    {
        // No cal fer res més
        return;
    }

    // Cas que sigui AlignRight o AlignLeft
    double bounds[6];
    m_mainVolumeDisplayUnit->getImageActor()->GetBounds(bounds);
    double motionVector[4] = { 0.0, 0.0, 0.0, 0.0 };
    
    double alignmentPoint[3];
    if (alignPosition == AlignLeft)
    {
        computeDisplayToWorld(0.0, 0.0, 0.0, alignmentPoint);
    }
    else if (alignPosition == AlignRight)
    {
        computeDisplayToWorld((double)this->getRenderer()->GetSize()[0], 0.0, 0.0, alignmentPoint);
    }
    
    // Càlcul del desplaçament
    int boundIndex = 0;
    switch (this->getCurrentViewPlane())
    {
        case OrthogonalPlane::XYPlane:
            // Si es dóna el cas que o bé està rotada 180º o bé està voltejada, cal agafar l'altre extrem
            // L'operació realitzada és un XOR (!=)
            if (m_isImageFlipped != (m_rotateFactor == 2))
            {
                if (alignPosition == AlignLeft)
                {
                    boundIndex = 1;
                }
            }
            else
            {
                if (alignPosition == AlignRight)
                {
                    boundIndex = 1;
                }
            }

            motionVector[0] = bounds[boundIndex] - alignmentPoint[0];
            
            break;

        case OrthogonalPlane::YZPlane:
            if (alignPosition == AlignLeft)
            {
                boundIndex = 2;
            }
            else if (alignPosition == AlignRight)
            {
                boundIndex = 3;
            }
            
            motionVector[1] = bounds[boundIndex] - alignmentPoint[1];
            
            break;

        case OrthogonalPlane::XZPlane:
            if (alignPosition == AlignRight)
            {
                boundIndex = 1;
            }
            
            motionVector[0] = bounds[boundIndex] - alignmentPoint[0];
            
            break;
    }

    pan(motionVector);
}

void Q2DViewer::setImageOrientation(const PatientOrientation &desiredPatientOrientation)
{
    m_imageOrientationOperationsMapper->setInitialOrientation(getCurrentDisplayedImagePatientOrientation());
    m_imageOrientationOperationsMapper->setDesiredOrientation(desiredPatientOrientation);

    int turns = m_imageOrientationOperationsMapper->getNumberOfClockwiseTurnsToApply();
    bool flip = m_imageOrientationOperationsMapper->requiresHorizontalFlip();

    // Update the obtained orientation parameters
    rotate(turns);
    setFlip(flip);

    // Then, only update the camera and apply render if there have been changes on the orientation parameters
    if (flip || turns > 0)
    {
        applyImageOrientationChanges();
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

    m_overlaysAreEnabled = enable;
}

void Q2DViewer::showDisplayShutters(bool enable)
{
    m_showDisplayShutters = enable;
    updateDisplayShutterMask();
    render();
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

void Q2DViewer::setFlip(bool flip)
{
    m_applyFlip = flip;
}

void Q2DViewer::applyImageOrientationChanges()
{
    updateCamera();
    render();
    
    emit imageOrientationChanged(getCurrentDisplayedImagePatientOrientation());
}

void Q2DViewer::getCurrentRenderedItemBounds(double bounds[6])
{
    m_mainVolumeDisplayUnit->getImageActor()->GetBounds(bounds);
}

void Q2DViewer::updateCurrentImageDefaultPresets()
{
    if (this->getCurrentViewPlane() == OrthogonalPlane::XYPlane)
    {
        Image *image = getCurrentDisplayedImage();
        for (int i = 0; i < image->getNumberOfWindowLevels(); ++i)
        {
            WindowLevel windowLevel = getDefaultWindowLevelForPresentation(image, i);
            m_windowLevelData->updatePreset(windowLevel);
        }
    }
}

double Q2DViewer::getCurrentSpacingBetweenSlices()
{
    int zIndex = this->getCurrentViewPlane().getZIndexForView();
    
    return m_mainVolume->getSpacing()[zIndex];
}

vtkImageActor* Q2DViewer::getVtkImageActor() const
{
    return m_mainVolumeDisplayUnit->getImageActor();
}

Q2DViewer* Q2DViewer::castFromQViewer(QViewer *viewer)
{
    if (!viewer)
    {
        DEBUG_LOG(QString("Cannot cast a null pointer"));
        return 0;
    }

    Q2DViewer *viewer2D = qobject_cast<Q2DViewer*>(viewer);
    if (!viewer2D)
    {
        DEBUG_LOG(QString("Q2DViewer casting failed! Provided viewer may not be a Q2DViewer object: %1").arg(viewer->metaObject()->className()));
    }

    return viewer2D;
}

bool Q2DViewer::canShowDisplayShutter() const
{
    return m_mainVolume
        && m_mainVolume->objectName() != DummyVolumeObjectName
        && !isThickSlabActive()
        && this->getCurrentViewPlane() == OrthogonalPlane::XYPlane
        && getCurrentDisplayedImage()
        && getCurrentDisplayedImage()->getDisplayShutterForDisplayAsVtkImageData(m_mainVolume->getImageIndex(getCurrentSlice(), getCurrentPhase()));
}

void Q2DViewer::updateDisplayShutterMask()
{
    vtkImageData *shutterData = NULL;
    if (m_showDisplayShutters && this->canShowDisplayShutter())
    {
        Image *image = getCurrentDisplayedImage();

        if (image)
        {
            shutterData = image->getDisplayShutterForDisplayAsVtkImageData(m_mainVolume->getImageIndex(getCurrentSlice(), getCurrentPhase()));
        }
    }
    m_mainVolumeDisplayUnit->getImagePipeline()->setShutterData(shutterData);
}

const OrthogonalPlane& Q2DViewer::getCurrentViewPlane() const
{
    return m_mainVolumeDisplayUnit->getViewPlane();
}

void Q2DViewer::setCurrentViewPlane(const OrthogonalPlane &viewPlane)
{
    QViewer::setCurrentViewPlane(viewPlane);
    m_mainVolumeDisplayUnit->setViewPlane(viewPlane);
}

};  // End namespace udg
