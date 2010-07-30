/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
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
// Thickslab
#include "vtkProjectionImageFilter.h"
// Qt
#include <QResizeEvent>
// Include's bàsics vtk
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>
#include <vtkPropPicker.h>
// Composició d'imatges
#include <vtkImageBlend.h>
// Anotacions
#include <vtkCornerAnnotation.h>
#include <vtkAxisActor2D.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkProperty2D.h>
#include <vtkProp.h>
#include <vtkScalarBarActor.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkImageActor.h>
// Grayscale pipeline
#include <vtkImageMapToWindowLevelColors2.h> // Permet aplicar window/level amb imatges a color
#include <vtkScalarsToColors.h>
#include <vtkColorTransferFunction.h>
// Projecció de punts
#include <vtkMatrix4x4.h>

namespace udg {

Q2DViewer::Q2DViewer(QWidget *parent)
: QViewer(parent), m_lastView(Q2DViewer::Axial), m_currentSlice(0), m_currentPhase(0), m_overlayVolume(0), m_blender(0), m_imagePointPicker(0), m_cornerAnnotations(0), m_enabledAnnotations(Q2DViewer::AllAnnotation), m_overlapMethod(Q2DViewer::Blend), m_sideRuler(0), m_bottomRuler(0), m_scalarBar(0), m_rotateFactor(0), m_numberOfPhases(1), m_maxSliceValue(0), m_applyFlip(false), m_isImageFlipped(false), m_slabThickness(1), m_firstSlabSlice(0), m_lastSlabSlice(0), m_thickSlabActive(false), m_slabProjectionMode(AccumulatorFactory::Maximum)
{
    m_imageSizeInformation[0] = 0;
    m_imageSizeInformation[1] = 0;

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
    connect(this, SIGNAL(cameraChanged()), SLOT(updateRulers()));

    m_imageOrientationOperationsMapper = new ImageOrientationOperationsMapper();

    m_alignPosition = Q2DViewer::AlignCenter;
}

Q2DViewer::~Q2DViewer()
{
    // Fem delete de tots els objectes vtk dels que hem fet un ::New()
    m_scalarBar->Delete();
    m_patientOrientationTextActor[0]->Delete();
    m_patientOrientationTextActor[1]->Delete();
    m_patientOrientationTextActor[2]->Delete();
    m_patientOrientationTextActor[3]->Delete();
    m_sideRuler->Delete();
    m_bottomRuler->Delete();
    m_cornerAnnotations->Delete();
    m_imagePointPicker->Delete();
    m_imageActor->Delete();
    m_anchoredRulerCoordinates->Delete();
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
}

void Q2DViewer::createAnnotations()
{
    // Contenidor d'anotacions
    m_cornerAnnotations = vtkCornerAnnotation::New();
    m_cornerAnnotations->GetTextProperty()->SetFontFamilyToArial();
    m_cornerAnnotations->GetTextProperty()->ShadowOn();
    m_cornerAnnotations->GetTextProperty()->SetShadow(1);

    // Escala de colors
    createScalarBar();
    // Anotacions de l'orientació del pacient
    createOrientationAnnotations();
    // Marcadors d'escala
    createRulers();
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
        m_patientOrientationTextActor[i]->GetTextProperty()->SetShadow(1);

        m_patientOrientationTextActor[i]->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
        m_patientOrientationTextActor[i]->GetPosition2Coordinate()->SetCoordinateSystemToNormalizedViewport();
    }
    // Ara posem la informació concreta de cadascuna de les referència d'orientació. 0-4 en sentit anti-horari, començant per 0 = esquerra de la pantalla
    m_patientOrientationTextActor[0]->GetTextProperty()->SetJustificationToLeft();
    m_patientOrientationTextActor[0]->SetPosition(0.01 , 0.5);

    m_patientOrientationTextActor[1]->GetTextProperty()->SetJustificationToCentered();
    m_patientOrientationTextActor[1]->SetPosition(0.5 , 0.01);

    m_patientOrientationTextActor[2]->GetTextProperty()->SetJustificationToRight();
    m_patientOrientationTextActor[2]->SetPosition(0.99 , 0.5);

    m_patientOrientationTextActor[3]->GetTextProperty()->SetJustificationToCentered();
    m_patientOrientationTextActor[3]->GetTextProperty()->SetVerticalJustificationToTop();
    m_patientOrientationTextActor[3]->SetPosition(0.5 , 0.99);
}

void Q2DViewer::createRulers()
{
    // Ruler lateral
    m_sideRuler = vtkAxisActor2D::New();
    m_sideRuler->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_sideRuler->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
    m_sideRuler->AxisVisibilityOn();
    m_sideRuler->TickVisibilityOn();
    m_sideRuler->LabelVisibilityOn();
    m_sideRuler->AdjustLabelsOff();
    m_sideRuler->SetLabelFormat("%.2f");
    m_sideRuler->SetLabelFactor(0.35);
    m_sideRuler->GetLabelTextProperty()->ItalicOff();
    m_sideRuler->GetLabelTextProperty()->BoldOff();
    m_sideRuler->GetLabelTextProperty()->ShadowOff();
    m_sideRuler->GetLabelTextProperty()->SetColor(0 , 0.7 , 0);
    m_sideRuler->GetLabelTextProperty()->SetFontFamilyToArial();
    m_sideRuler->TitleVisibilityOff();
    m_sideRuler->SetTickLength(10);
    m_sideRuler->GetProperty()->SetColor(0 , 1 , 0);
    m_sideRuler->VisibilityOff(); // Per defecte, fins que no hi hagi input son invisibles

    // Ruler inferior
    m_bottomRuler = vtkAxisActor2D::New();
    m_bottomRuler->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_bottomRuler->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
    m_bottomRuler->AxisVisibilityOn();
    m_bottomRuler->TickVisibilityOn();
    m_bottomRuler->LabelVisibilityOn();
    m_bottomRuler->AdjustLabelsOff();
    m_bottomRuler->SetLabelFormat("%.2f");
    m_bottomRuler->SetLabelFactor(0.35);
    m_bottomRuler->GetLabelTextProperty()->ItalicOff();
    m_bottomRuler->GetLabelTextProperty()->BoldOff();
    m_bottomRuler->GetLabelTextProperty()->ShadowOff();
    m_bottomRuler->GetLabelTextProperty()->SetColor(0 , 0.7 , 0);
    m_bottomRuler->GetLabelTextProperty()->SetFontFamilyToArial();
    m_bottomRuler->TitleVisibilityOff();
    m_bottomRuler->SetTickLength(10);
    m_bottomRuler->GetProperty()->SetColor(0 , 1 , 0);
    m_bottomRuler->VisibilityOff(); // Per defecte, fins que no hi hagi input son invisibles

    // Coordenades fixes per ancorar els rulers al lateral i a la part inferior
    m_anchoredRulerCoordinates = vtkCoordinate::New();
    m_anchoredRulerCoordinates->SetCoordinateSystemToView();
    m_anchoredRulerCoordinates->SetValue(-0.95 , -0.9 , -0.95);
}

void Q2DViewer::updateRulers()
{
    double *anchoredCoordinates = m_anchoredRulerCoordinates->GetComputedWorldValue(this->getRenderer());

    switch (m_lastView)
    {
        case Axial:
            m_sideRuler->GetPositionCoordinate()->SetValue(anchoredCoordinates[0] , m_rulerExtent[3] , 0.0);
            m_sideRuler->GetPosition2Coordinate()->SetValue(anchoredCoordinates[0] , m_rulerExtent[2] , 0.0);
            m_sideRuler->SetRange(m_rulerExtent[3] , m_rulerExtent[2]);

            m_bottomRuler->GetPositionCoordinate()->SetValue(m_rulerExtent[1] , anchoredCoordinates[1]  , 0.0);
            m_bottomRuler->GetPosition2Coordinate()->SetValue(m_rulerExtent[0] , anchoredCoordinates[1] , 0.0);
            m_bottomRuler->SetRange(m_rulerExtent[1] , m_rulerExtent[0]);
            break;

        case Sagital:
            m_sideRuler->GetPositionCoordinate()->SetValue(0.0 , anchoredCoordinates[1] , m_rulerExtent[4]);
            m_sideRuler->GetPosition2Coordinate()->SetValue(0.0 , anchoredCoordinates[1] , m_rulerExtent[5]);
            m_sideRuler->SetRange(m_rulerExtent[4] , m_rulerExtent[5]);

            m_bottomRuler->GetPositionCoordinate()->SetValue(0.0 , m_rulerExtent[3] , anchoredCoordinates[2]);
            m_bottomRuler->GetPosition2Coordinate()->SetValue(0.0 , m_rulerExtent[2] , anchoredCoordinates[2]);
            m_bottomRuler->SetRange(m_rulerExtent[3] , m_rulerExtent[2]);
            break;

        case Coronal:
            m_sideRuler->GetPositionCoordinate()->SetValue(anchoredCoordinates[0] , 0.0 , m_rulerExtent[4]);
            m_sideRuler->GetPosition2Coordinate()->SetValue(anchoredCoordinates[0] , 0.0 , m_rulerExtent[5]);
            m_sideRuler->SetRange(m_rulerExtent[4] , m_rulerExtent[5]);

            m_bottomRuler->GetPositionCoordinate()->SetValue(m_rulerExtent[1] , 0.0 , anchoredCoordinates[2]);
            m_bottomRuler->GetPosition2Coordinate()->SetValue(m_rulerExtent[0] , 0.0 , anchoredCoordinates[2]);
            m_bottomRuler->SetRange(m_rulerExtent[1] , m_rulerExtent[0]);
            break;
    }
}

void Q2DViewer::createScalarBar()
{
    m_scalarBar = vtkScalarBarActor::New();
    m_scalarBar->SetOrientationToVertical();
    m_scalarBar->GetPositionCoordinate()->SetCoordinateSystemToView();
    m_scalarBar->SetPosition(0.8 , -0.8);
    m_scalarBar->SetWidth(0.08);
    m_scalarBar->SetHeight(0.6);
    m_scalarBar->SetLabelFormat(" %.f  ");
    m_scalarBar->SetNumberOfLabels(3);
    m_scalarBar->GetLabelTextProperty()->ItalicOff();
    m_scalarBar->GetLabelTextProperty()->BoldOff();
    m_scalarBar->GetLabelTextProperty()->SetJustificationToRight();
    m_scalarBar->GetLabelTextProperty()->SetFontFamilyToArial();
    m_scalarBar->GetLabelTextProperty()->ShadowOff();
    m_scalarBar->VisibilityOff(); // Inicialment serà invisible fins que no hi hagi input

    // Li configurem la lookup table
    vtkWindowLevelLookupTable *lookup = vtkWindowLevelLookupTable::New();
    lookup->SetWindow(m_windowLevelLUTMapper->GetWindow());
    lookup->SetLevel(m_windowLevelLUTMapper->GetLevel());
    lookup->Build();
    m_scalarBar->SetLookupTable(lookup);
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
}

void Q2DViewer::horizontalFlip()
{
    m_applyFlip = true;
    updateCamera();
}

void Q2DViewer::verticalFlip()
{
    rotate(2);
    horizontalFlip();
}

QVector<QString> Q2DViewer::getCurrentDisplayedImageOrientationLabels() const
{
    int index = (m_lastView == Axial) ? m_currentSlice : 0;
    // Això es fa per si tenim un mhd que realment només té un arxiu (imatge) però té més llesques
    // TODO Caldria millorar l'accés a les imatges a partir del volum, per no haver de fer aquestes filigranes
    // és a dir, al preguntar a Volume, getImage(index) ell ens retorna la imatge que toca i ja comprova rangs si cal
    // i no ens retorna la llista d'imatges a saco
    index = (index >= m_mainVolume->getImages().size()) ? 0 : index;
    
    QString orientation;
    Image *image = m_mainVolume->getImage(index);
    if (image)
    {
        orientation = image->getPatientOrientation();
    }
    // Tenim les orientacions originals de la imatge en una llista
    QStringList list = orientation.split("\\");

    bool ok = false;
    switch (list.size())
    {
        case 2:
            // Afegim un element neutre perque la resta segueixi funcionant be
            ok = true;
            list << "";
            break;
    
        case 3:
            ok = true;
            break;
    }

    QVector<QString> labelsVector(4);
    // Ara caldrà posar, en funció de les rotacions, flips i vista, les etiquetes en l'ordre adequat
    if (ok)
    {
        int index = 4+m_rotateFactor;
        // 0:Esquerra, 1:A dalt, 2:Dreta, 3:Abaix
        if (m_lastView == Axial)
        {
            labelsVector[(0 + index) % 4] = this->getOppositeOrientationLabel(list.at(0)); // Esquerra
            labelsVector[(2 + index) % 4] = list.at(0); // Dreta
            labelsVector[(1 + index) % 4] = this->getOppositeOrientationLabel(list.at(1)); // A dalt
            labelsVector[(3 + index) % 4] = list.at(1); // A baix
        }
        else if (m_lastView == Sagital)
        {
            // HACK FLIP De moment necessitem fer aquest truc. Durant el refactoring caldria
            // veure si es pot fer d'una manera millor
            if (m_isImageFlipped)
            {
                index += 2;
            }

            labelsVector[(0 + index) % 4] =  this->getOppositeOrientationLabel(list.at(1)); // Esquerra
            labelsVector[(2 + index) % 4] =  list.at(1); // Dreta
            labelsVector[(1 + index) % 4] =  list.at(2); // A dalt
            labelsVector[(3 + index) % 4] =  this->getOppositeOrientationLabel(list.at(2)); // A baix
        }
        else if (m_lastView == Coronal)
        {
            // HACK FLIP De moment necessitem fer aquest truc. Durant el refactoring caldria
            // veure si es pot fer d'una manera millor
            if (m_isImageFlipped)
            {
                index += 2;
            }

            labelsVector[(0 + index) % 4] = this->getOppositeOrientationLabel(list.at(0)); // Esquerra
            labelsVector[(2 + index) % 4] = list.at(0); // Dreta
            labelsVector[(1 + index) % 4] = list.at(2); // A dalt
            labelsVector[(3 + index) % 4] = this->getOppositeOrientationLabel(list.at(2)); // A baix
        }
        if (m_isImageFlipped)
        {
            qSwap(labelsVector[0], labelsVector[2]);
        }
    }
    else
    {
        DEBUG_LOG("La imatge actual no conté etiqueta d'orientació vàlida: " + orientation);
    }
    return labelsVector;
}

QString Q2DViewer::getCurrentPlaneProjectionLabel() const
{
    QVector<QString> labels = this->getCurrentDisplayedImageOrientationLabels();
    return Image::getProjectionLabelFromPlaneOrientation(labels[0] + "\\" + labels[1]);
}

void Q2DViewer::getXYZIndexesForView(int &x, int &y, int &z, int view)
{
    x = Q2DViewer::getXIndexForView(view);
    y = Q2DViewer::getYIndexForView(view);
    z = Q2DViewer::getZIndexForView(view);
}

int Q2DViewer::getXIndexForView(int view)
{
    int x;
    switch (view)
    {
        case Q2DViewer::Axial:
            x = 0;
            break;

        case Q2DViewer::Sagital:
            x = 1;
            break;

        case Q2DViewer::Coronal:
            x = 0;
            break;
    }
    return x;
}

int Q2DViewer::getYIndexForView(int view)
{
    int y;
    switch (view)
    {
        case Q2DViewer::Axial:
            y = 1;
            break;

        case Q2DViewer::Sagital:
            y = 2;
            break;

        case Q2DViewer::Coronal:
            y = 2;
            break;
    }
    return y;
}

int Q2DViewer::getZIndexForView(int view)
{
    int z;
    switch (view)
    {
        case Q2DViewer::Axial:
            z = 2;
            break;

        case Q2DViewer::Sagital:
            z = 0;
            break;

        case Q2DViewer::Coronal:
            z = 1;
            break;
    }
    return z;
}

void Q2DViewer::mapOrientationStringToAnnotation()
{
    // Obtenim els labels que estem veient en aquest moment
    QVector<QString> labels = this->getCurrentDisplayedImageOrientationLabels();

    // Text actor -> 0:Esquerra, 1:Abaix , 2:Dreta, 3:A dalt
    // Labels     -> 0:Esquerra, 1:A dalt, 2:Dreta, 3:Abaix
    m_patientOrientationTextActor[0]->SetInput(qPrintable(labels[0]));
    m_patientOrientationTextActor[1]->SetInput(qPrintable(labels[3]));
    m_patientOrientationTextActor[2]->SetInput(qPrintable(labels[2]));
    m_patientOrientationTextActor[3]->SetInput(qPrintable(labels[1]));
}

void Q2DViewer::refreshAnnotations()
{
    if (!m_mainVolume)
    {
        return;
    }

    if (m_enabledAnnotations & Q2DViewer::PatientInformationAnnotation)
    {
        m_cornerAnnotations->SetText(3, qPrintable(m_upperRightText));
        m_cornerAnnotations->SetText(1, qPrintable(m_lowerRightText.trimmed()));
    }
    else
    {
        m_cornerAnnotations->SetText(3, "");
        m_cornerAnnotations->SetText(1, "");
    }

    if (m_enabledAnnotations & Q2DViewer::RulersAnnotation)
    {
        m_sideRuler->VisibilityOn();
        m_bottomRuler->VisibilityOn();
    }
    else
    {
        m_sideRuler->VisibilityOff();
        m_bottomRuler->VisibilityOff();
    }

    if (m_enabledAnnotations & Q2DViewer::PatientOrientationAnnotation)
    {
        for (int j = 0; j < 4; j++)
            m_patientOrientationTextActor[j]->VisibilityOn();
    }
    else
    {
        for (int j = 0; j < 4; j++)
            m_patientOrientationTextActor[j]->VisibilityOff();
    }

    if (m_enabledAnnotations & Q2DViewer::ScalarBarAnnotation)
    {
        m_scalarBar->VisibilityOn();
    }
    else
    {
        m_scalarBar->VisibilityOff();
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
                        thickness = image->getSliceThickness();
                    else
                        thickness = m_mainVolume->getSpacing()[2];
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
        if (m_numberOfPhases == 1) // Si és un volum 3D normal...
        {
            int *extent = m_mainVolume->getWholeExtent();
            min = extent[m_lastView * 2];
            max = extent[m_lastView * 2 + 1];
        }
        else // Si tenim 4D
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
        this->getSliceRange(range[0],range[1]);
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
    this->getSliceRange(min,trash);
    return min;
}

int Q2DViewer::getMaximumSlice()
{
    int max, trash;
    this->getSliceRange(trash,max);
    return max;
}

void Q2DViewer::addActors()
{
    Q_ASSERT(m_cornerAnnotations);
    Q_ASSERT(m_patientOrientationTextActor[0]);
    Q_ASSERT(m_patientOrientationTextActor[1]);
    Q_ASSERT(m_patientOrientationTextActor[2]);
    Q_ASSERT(m_patientOrientationTextActor[3]);
    Q_ASSERT(m_sideRuler);
    Q_ASSERT(m_bottomRuler);
    Q_ASSERT(m_scalarBar);
    Q_ASSERT(m_imageActor);

    vtkRenderer *renderer = getRenderer();
    Q_ASSERT(renderer);
    // Anotacions de texte
    renderer->AddViewProp(m_cornerAnnotations);
    renderer->AddViewProp(m_patientOrientationTextActor[0]);
    renderer->AddViewProp(m_patientOrientationTextActor[1]);
    renderer->AddViewProp(m_patientOrientationTextActor[2]);
    renderer->AddViewProp(m_patientOrientationTextActor[3]);
    renderer->AddViewProp(m_sideRuler);
    renderer->AddViewProp(m_bottomRuler);
    renderer->AddViewProp(m_scalarBar);
    renderer->AddViewProp(m_imageActor);
    // TODO Colocar això en un lloc mes adient
    vtkCamera *camera = getActiveCamera();
    Q_ASSERT(camera);
    camera->ParallelProjectionOn();
}

QString Q2DViewer::getOppositeOrientationLabel(const QString &label)
{
    int i = 0;
    QString oppositeLabel;
    while (i < label.size())
    {
        if (label.at(i) == 'L')
        {
            oppositeLabel += "R";
        }
        else if (label.at(i) == 'R')
        {
            oppositeLabel += "L";
        }
        else if (label.at(i) == 'A')
        {
            oppositeLabel += "P";
        }
        else if (label.at(i) == 'P')
        {
            oppositeLabel += "A";
        }
        else if (label.at(i) == 'H')
        {
            oppositeLabel += "F";
        }
        else if (label.at(i) == 'F')
        {
            oppositeLabel += "H";
        }
        else
        {
            oppositeLabel += "?";
        }
        i++;
    }
    return oppositeLabel;
}

void Q2DViewer::setInput(Volume *volume)
{
    if (!volume)
    {
        return;
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

    int extent[6];
    double origin[3], spacing[3];
    m_mainVolume->getOrigin(origin);
    m_mainVolume->getSpacing(spacing);
    m_mainVolume->getWholeExtent(extent);
    m_rulerExtent[0] = origin[0];
    m_rulerExtent[1] = origin[0] + extent[1]*spacing[0];
    m_rulerExtent[2] = origin[1];
    m_rulerExtent[3] = origin[1] + extent[3]*spacing[1];
    m_rulerExtent[4] = origin[2];
    m_rulerExtent[5] = origin[2] + extent[5]*spacing[2];

    m_numberOfPhases = m_mainVolume->getNumberOfPhases();
    m_maxSliceValue = this->getMaximumSlice();

    // Això es fa per destruir el blender en cas que ja hi hagi algun input i es vulgui canviar
    if (m_blender !=0)
    {
        m_blender->Delete();
        m_blender = 0;
    }
    // Obtenim valors de gris i aquestes coses
    // Aquí es crea tot el pipeline del visualitzador
    this->applyGrayscalePipeline();

    // Preparem el thickSlab 
    // TODO Cada cop que fem setInput resetejem els valors per defecte?
    m_thickSlabProjectionFilter->SetInput(m_mainVolume->getVtkData());
    m_thickSlabProjectionFilter->SetProjectionDimension(m_lastView);
    m_thickSlabProjectionFilter->SetAccumulatorType((AccumulatorFactory::AccumulatorType) m_slabProjectionMode);
    m_thickSlabProjectionFilter->SetFirstSlice(m_firstSlabSlice * m_numberOfPhases + m_currentPhase);
    m_thickSlabProjectionFilter->SetNumberOfSlicesToProject(m_slabThickness);
    m_thickSlabProjectionFilter->SetStep(m_numberOfPhases);

    updateDisplayExtent(); // TODO BUG Si no fem aquesta crida ens peta al canviar d'input entre un que fos més gran que l'anterior
    resetViewToAxial();

    updatePatientAnnotationInformation();
    this->enableAnnotation(m_enabledAnnotations);

    // Actualitzem la informació de window level
    this->updateWindowLevelData();
    // HACK
    // S'activa el rendering de nou per tal de que es renderitzi l'escena
    enableRendering(true);
    // Indiquem el canvi de volum
    emit volumeChanged(m_mainVolume);
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
    m_alignPosition = Q2DViewer::AlignCenter;
    resetCamera();
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
                    roll = m_rotateFactor*90. + 180.;
                }
                else
                {
                    roll = -m_rotateFactor*90. + 180.;
                }
                break;

            case Sagital:
                if (m_isImageFlipped)
                {
                    roll = m_rotateFactor*90. -90.;
                }
                else
                {
                    roll = -m_rotateFactor*90. - 90.;
                }
                break;

            case Coronal:
                if (m_isImageFlipped)
                {
                    roll = m_rotateFactor*90.;
                }
                else
                {
                    roll = -m_rotateFactor*90.;
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
                // HACK Aquest hack esta relacionat amb els de mapOrientationStringToAnnotation()
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
        mapOrientationStringToAnnotation();
        this->render();
    }
    else
    {
        DEBUG_LOG("Intentant actualitzar rotació de càmera sense haver donat un input abans...");
    }
}

void Q2DViewer::resetCamera()
{
    if (m_mainVolume)
    {
        // En comptes de fer servir sempre this->getMaximumSlice(), actualitzem
        // Aquest valor quan cal, és a dir, al posar input i al canviar de vista
        // estalviant-nos crides i crides
        m_maxSliceValue = this->getMaximumSlice();

        // Reiniciem valors per defecte de la càmera
        m_rotateFactor = 0;
        m_applyFlip = false;
        m_isImageFlipped = false;

        vtkCamera *camera = getActiveCamera();
        Q_ASSERT(camera);

        int initialSliceIndex = 0;
        double cameraViewUp[3] = {0.0,0.0,0.0};
        double cameraPosition[3] = {0.0,0.0,0.0};
        double cameraRoll = 0.0;
        double cameraAzimuth = 0.0;
        QString position;

        switch (m_lastView)
        {
            case Axial:
                // Paràmetres de la càmera
                cameraViewUp[1] = -1.0;
                cameraPosition[2] = -1.0;
                cameraRoll = 180.0;
                // Indiquem quina és la llesca inicial
                initialSliceIndex = 0;

                // Obtenim la mida de la matriu de la imatge (files i columnes)
                m_imageSizeInformation[0] = m_mainVolume->getDimensions()[0];
                m_imageSizeInformation[1] = m_mainVolume->getDimensions()[1];
                break;

            case Sagital:
                // Paràmetres de la càmera
                cameraViewUp[2] = 1.0;
                cameraPosition[0] = 1.0;
                // TODO Solució inmediata per afrontar el ticket #355, pero s'hauria de fer d'una manera mes elegant i consistent
                position = m_mainVolume->getImage(0)->getParentSeries()->getPatientPosition();
                if (position == "FFP" || position == "HFP")
                {
                    cameraRoll = 90.0;
                    m_rotateFactor = 2;
                }
                else
                {
                    cameraRoll = -90.0;
                }
                
                // Indiquem quina és la llesca inicial
                initialSliceIndex = m_maxSliceValue/2;

                // Obtenim la mida de la matriu de la imatge (files i columnes)
                m_imageSizeInformation[0] = m_mainVolume->getDimensions()[1];
                m_imageSizeInformation[1] = m_mainVolume->getDimensions()[2];
                break;

            case Coronal:
                // Paràmetres de la càmera
                cameraViewUp[2] = 1.0;
                cameraPosition[1] = -1.0;
                // TODO Solució inmediata per afrontar el ticket #355, pero s'hauria de fer d'una manera mes elegant i consistent
                position = m_mainVolume->getImage(0)->getParentSeries()->getPatientPosition();
                if (position == "FFP" || position == "HFP")
                {
                    cameraRoll = 180.0;
                    cameraAzimuth = 180.0;
                    m_isImageFlipped = true;
                }
                else
                {
                    cameraRoll = 0.0;
                }

                // Indiquem quina és la llesca inicial
                initialSliceIndex = m_maxSliceValue/2;

                // Obtenim la mida de la matriu de la imatge (files i columnes)
                m_imageSizeInformation[0] = m_mainVolume->getDimensions()[0];
                m_imageSizeInformation[1] = m_mainVolume->getDimensions()[2];
                break;
        }

        // Ajustem la càmera
        camera->SetFocalPoint(0,0,0);
        camera->SetViewUp(cameraViewUp);
        camera->SetPosition(cameraPosition);
        camera->SetRoll(cameraRoll);
        if (cameraAzimuth != 0.0)
        {
            camera->Azimuth(cameraAzimuth);
        }

        // Posicionem la imatge 
        // TODO No ho fem amb setSlice() perquè introdueix flickering degut a 
        // l'emit sliceChanged() que provoca un render() a través del Drawer. 
        // Cal veure com evitar aquesta duplicació de codi de setSlice() perquè tot segueixi funcionant igual
        checkAndUpdateSliceValue(initialSliceIndex);
        updateDisplayExtent();
        getRenderer()->ResetCamera(); // Aquesta línia és de més a més (adicional al codi de setSlice()!)
        updateAnnotationsInformation(Q2DViewer::SliceAnnotation | Q2DViewer::WindowInformationAnnotation);
        mapOrientationStringToAnnotation();
        
        // Ajustem la imatge al viewport
        fitImageIntoViewport();

        // Hem d'indicar l'slice changed al final per evitar el flickering que abans comentàvem
        emit sliceChanged(m_currentSlice);
        render();
    }
    else
    {
        DEBUG_LOG("Intentant canviar de vista sense haver donat un input abans...");
    }
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
        // TODO per cada canvi de llesca potser també caldria
        // comprovar si el ww/wwl és diferent i aplicar el que toqui (#478)
        updateSliceAnnotationInformation();
        mapOrientationStringToAnnotation();
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
            value = 0;
        }
        else if (value > m_numberOfPhases - 1)
        {
            value = m_numberOfPhases - 1;
        }

        m_currentPhase = value;
        if (isThickSlabActive())
        {
            m_thickSlabProjectionFilter->SetFirstSlice(m_firstSlabSlice * m_numberOfPhases + m_currentPhase);
        }
        this->updateDisplayExtent();
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

void Q2DViewer::setWindowLevel(double window , double level)
{
    if (m_mainVolume)
    {
        m_windowLevelLUTMapper->SetWindow(window);
        m_windowLevelLUTMapper->SetLevel(level);
        updateAnnotationsInformation(Q2DViewer::WindowInformationAnnotation);
        // Actualitzem la Scalar bar si aquesta és visible
        if (m_enabledAnnotations & Q2DViewer::ScalarBarAnnotation)
        {
            vtkWindowLevelLookupTable::SafeDownCast(m_scalarBar->GetLookupTable())->SetWindow(m_windowLevelLUTMapper->GetWindow());
            vtkWindowLevelLookupTable::SafeDownCast(m_scalarBar->GetLookupTable())->SetLevel(m_windowLevelLUTMapper->GetLevel());
        }
        this->render();
        emit windowLevelChanged(window , level);
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
    m_windowLevelLUTMapper->SetLookupTable(m_transferFunction->getColorTransferFunction());
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

ImagePlane* Q2DViewer::getImagePlane(int sliceNumber , int phaseNumber, bool vtkReconstructionHack)
{
    ImagePlane *imagePlane = 0;
    if (m_mainVolume)
    {
        int *dimensions = m_mainVolume->getDimensions();
        double *spacing = m_mainVolume->getSpacing();
        const double *origin  = m_mainVolume->getOrigin();
        switch (m_lastView)
        {
            case Axial: // XY
                {
                    Image *image = m_mainVolume->getImage(sliceNumber, phaseNumber);
                    if (image)
                    {
                        imagePlane = new ImagePlane();
                        imagePlane->fillFromImage(image);
                    }
                }
                break;

            case Sagital: // YZ TODO Encara no esta comprovat que aquest pla sigui correcte
                {
                    Image *image = m_mainVolume->getImage(0);
                    if (image)
                    {
                        imagePlane = new ImagePlane();
                        const double *directionCosines = image->getImageOrientationPatient();

                        if (vtkReconstructionHack)
                        {
                            // Retornem un fals pla, respecte el món real, però que s'ajusta més al món vtk
                            imagePlane->setRowDirectionVector(directionCosines[3], directionCosines[4], directionCosines[5]);
                            imagePlane->setColumnDirectionVector(directionCosines[6], directionCosines[7], directionCosines[8]);
                        }
                        else
                        {
                            // Això serà lo normal, retornar la autèntica direcció del pla
                            double columnVector[3];
                            m_mainVolume->getStackDirection(columnVector, 0);

                            imagePlane->setRowDirectionVector(directionCosines[3], directionCosines[4], directionCosines[5]);
                            imagePlane->setColumnDirectionVector(columnVector[0], columnVector[1], columnVector[2]);
                        }

                        imagePlane->setSpacing(spacing[1], spacing[2]);
                        imagePlane->setThickness(spacing[0]);
                        imagePlane->setRows(dimensions[2]);
                        imagePlane->setColumns(dimensions[1]);

                        // TODO Falta esbrinar si l'origen que estem donant es bo o no
                        imagePlane->setOrigin(origin[0] + sliceNumber*directionCosines[0]*spacing[0], origin[1] + sliceNumber*directionCosines[1]*spacing[0], origin[2] + sliceNumber*directionCosines[2]*spacing[0]);
                    }
                }
                break;

            case Coronal: // XZ TODO Encara no esta comprovat que aquest pla sigui correcte
                {
                    Image *image = m_mainVolume->getImage(0);
                    if (image)
                    {
                        imagePlane = new ImagePlane();
                        const double *directionCosines = image->getImageOrientationPatient();

                        if (vtkReconstructionHack)
                        {
                            // Retornem un fals pla, respecte el món real, però que s'ajusta més al món vtk
                            imagePlane->setRowDirectionVector(directionCosines[0], directionCosines[1], directionCosines[2]);
                            imagePlane->setColumnDirectionVector(directionCosines[6], directionCosines[7], directionCosines[8]);
                        }
                        else
                        {
                            double columnVector[3];
                            m_mainVolume->getStackDirection(columnVector, 0);

                            imagePlane->setRowDirectionVector(directionCosines[0], directionCosines[1], directionCosines[2]);
                            imagePlane->setColumnDirectionVector(columnVector[0], columnVector[1], columnVector[2]);
                        }

                        imagePlane->setSpacing(spacing[0], spacing[2]);
                        imagePlane->setThickness(spacing[1]);
                        imagePlane->setRows(dimensions[2]);
                        imagePlane->setColumns(dimensions[0]);

                        // TODO Falta esbrinar si l'origen que estem donant es bo o no
                        imagePlane->setOrigin(origin[0] + directionCosines[3]*sliceNumber*spacing[1], origin[1] + directionCosines[4]*sliceNumber*spacing[1], origin[2] + directionCosines[5]*sliceNumber*spacing[1]);
                    }
                }
                break;
        }
    }
    return imagePlane;
}

void Q2DViewer::projectDICOMPointToCurrentDisplayedImage(const double pointToProject[3], double projectedPoint[3], bool vtkReconstructionHack)
{
    //
    // AQUÍ SUMEM L'origen TAL CUAL + L'ERROR DE DESPLAÇAMENT VTK
    //
    // La projecció es fa de la següent manera:
    // Primer es fa una  una projecció convencional del punt sobre el pla actual (DICOM)
    // Com que el mapeig de coordenades VTK va a la seva bola, necessitem corretgir el desplaçament
    // introduit per VTK respecte a les coordenades "reals" de DICOM
    // Aquest desplaçament consistirà en tornar a sumar l'origen del primer pla del volum
    // en principi, fer-ho amb l'origen de m_mainVolume també seria correcte
    //
    ImagePlane *currentPlane = this->getCurrentImagePlane(vtkReconstructionHack);
    if (currentPlane)
    {
        // Recollim les dades del pla actual sobre el qual volem projectar el punt de l'altre pla
        double currentPlaneRowVector[3], currentPlaneColumnVector[3], currentPlaneNormalVector[3], currentPlaneOrigin[3];
        currentPlane->getRowDirectionVector(currentPlaneRowVector);
        currentPlane->getColumnDirectionVector(currentPlaneColumnVector);
        currentPlane->getNormalVector(currentPlaneNormalVector);
        currentPlane->getOrigin(currentPlaneOrigin);

        // A partir d'aquestes dades creem la matriu de projecció,
        // que projectarà el punt donat sobre el pla actual
        vtkMatrix4x4 *projectionMatrix = vtkMatrix4x4::New();
        projectionMatrix->Identity();
        for (int column = 0; column < 3; column++)
        {
            projectionMatrix->SetElement(0,column,currentPlaneRowVector[column]);
            projectionMatrix->SetElement(1,column,currentPlaneColumnVector[column]);
            projectionMatrix->SetElement(2,column,currentPlaneNormalVector[column]);
        }

        // Un cop tenim la matriu podem fer la projeccio
        // necessitem el punt en coordenades homogenies
        double homogeneousPointToProject[4], homogeneousProjectedPoint[4];
        for (int i=0; i<3; i++)
            homogeneousPointToProject[i] = pointToProject[i] - currentPlaneOrigin[i]; // desplacem el punt a l'origen del pla
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
                        projectionMatrix->SetElement(0,0,0);
                        projectionMatrix->SetElement(0,1,1);
                        projectionMatrix->SetElement(0,2,0);
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
    }
    else
        DEBUG_LOG("No hi ha cap pla actual valid");
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
    if (annotation & Q2DViewer::WindowInformationAnnotation)
    {
        // Informació de la finestra
        if (m_enabledAnnotations & Q2DViewer::WindowInformationAnnotation)
        {
            m_upperLeftText = tr("%1 x %2\nWW: %5 WL: %6")
                .arg(m_imageSizeInformation[0])
                .arg(m_imageSizeInformation[1])
                .arg(MathTools::roundToNearestInteger(m_windowLevelLUTMapper->GetWindow()))
                .arg(MathTools::roundToNearestInteger(m_windowLevelLUTMapper->GetLevel()));
        }
        else
        {
            m_upperLeftText = "";
        }
        m_cornerAnnotations->SetText(2 , qPrintable(m_upperLeftText));
    }

    if (annotation & Q2DViewer::SliceAnnotation)
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

        m_upperRightText = tr("%1\n%2\n%3 %4 %5\nAcc:%6\n%7\n%8").arg(series->getInstitutionName()).arg(patient->getFullName()).arg(study->getPatientAge()).arg(patient->getSex()).arg(patient->getID()).arg(study->getAccessionNumber()).arg(study->getDateAsString()).arg(seriesTime);

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
    // TODO de moment assumim que totes les imatges seran de la mateixa modalitat.
    // Per evitar problemes amb el tractament de multiframe (que deixem per més endavant)
    // agafem directament la primera imatge, però cal solucionar aquest aspecte adequadament.
    Image *image = m_mainVolume->getImage(0);
    if (image->getParentSeries()->getModality() == "MG")
    {
        m_enabledAnnotations =  m_enabledAnnotations & ~Q2DViewer::SliceAnnotation;

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
            /// PS 3.16 - 2008, Page 408, Context ID 4014, View for mammography
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
            QString desiredOrientation;
            QString laterality = image->getImageLaterality();
            if (projection == "CC" || projection == "XCC" || projection == "XCCL" || projection == "XCCM" || projection == "FB")
            {
                if (laterality == "L")
                {
                    desiredOrientation = "A\\R";
                }
                else if (laterality == "R")
                {
                    desiredOrientation = "P\\L";
                }
            }
            else if (projection == "MLO" || projection == "ML" || projection == "LM" || projection == "LMO" || projection == "SIO")
            {
                if (laterality == "L")
                {
                    desiredOrientation = "A\\F";
                }
                else if (laterality == "R")
                {
                    desiredOrientation = "P\\F";
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

    int value = m_currentSlice*m_numberOfPhases + m_currentPhase;
    if (m_numberOfPhases > 1)
    {
        this->updateSliceAnnotation((value/m_numberOfPhases) + 1, m_maxSliceValue + 1, m_currentPhase + 1, m_numberOfPhases);
    }
    else
    {
        this->updateSliceAnnotation(value+1, m_maxSliceValue+1);
    }
    // Si aquestes anotacions estan activades, llavors li afegim la informació de la hora de la sèrie i la imatge
    if (m_enabledAnnotations & Q2DViewer::PatientInformationAnnotation)
    {
        // Si la vista és "AXIAL" (és a dir mostrem la imatge en l'adquisició original)
        // i tenim informació de la hora d'adquisició de la imatge, la incloem en la informació mostrada        
        if (m_lastView == Axial)
        {
            Image *currentImage = getCurrentDisplayedImage();
            if (currentImage)
            {
                QString imageTime = "\n"+currentImage->getFormattedImageTime();
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

    if (m_enabledAnnotations & Q2DViewer::SliceAnnotation) // Si les anotacions estan habilitades
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
                        Image *secondImage = m_mainVolume->getImage(m_currentSlice + m_slabThickness-1, m_currentPhase);
                        if (secondImage)
                        {
                            lowerLeftText += tr("-%1").arg(secondImage->getSliceLocation().toDouble(), 0, 'f', 2);
                        }
                    }
                    lowerLeftText += "\n";
                }
            }
        }

        if (maxPhase > 1) // Tenim fases
        {
            if (m_slabThickness > 1)
            {
                // TODO Potser hauríem de tenir una variable "slabRange"
                lowerLeftText += tr("Slice: %1-%2/%3 Phase: %4/%5").arg(currentSlice).arg(currentSlice + m_slabThickness-1).arg(maxSlice).arg(currentPhase).arg(maxPhase);
            }
            else
            {
                lowerLeftText += tr("Slice: %1/%2 Phase: %3/%4").arg(currentSlice).arg(maxSlice).arg(currentPhase).arg(maxPhase);
            }
        }
        else // Només llesques
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

        m_cornerAnnotations->SetText(0 , qPrintable(lowerLeftText));
    }
    else
    {
        m_cornerAnnotations->SetText(0 , "");
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
    // Thick slab
    int sliceValue;
    if (isThickSlabActive())
    {
        // En comptes de currentSlice podria ser m_firstSlabSlice, que és equivalent
        sliceValue = m_currentSlice; // Podria ser 0, dependent de l'extent de sortida del filtre
    }
    else
        sliceValue = m_currentSlice * m_numberOfPhases + m_currentPhase;

    // A partir de l'extent del volum, la vista i la llesca en la que ens trobem, 
    // calculem l'extent que li correspon a l'actor imatge
    int zIndex = getZIndexForView(m_lastView);
    int imageActorExtent[6];
    m_mainVolume->getWholeExtent(imageActorExtent);
    imageActorExtent[zIndex * 2] = imageActorExtent[zIndex * 2 + 1] = sliceValue;
    m_imageActor->SetDisplayExtent(imageActorExtent[0], imageActorExtent[1], imageActorExtent[2], imageActorExtent[3], imageActorExtent[4], imageActorExtent[5]);

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
        m_enabledAnnotations =  m_enabledAnnotations & ~annotation;
    }

    refreshAnnotations();
    this->render();
}

void Q2DViewer::removeAnnotation(AnnotationFlags annotation)
{
    enableAnnotation(annotation, false);
}

void Q2DViewer::applyGrayscalePipeline()
{
    DEBUG_LOG("*** Grayscale Transform Pipeline Begin ***");
    DEBUG_LOG(QString("Image Information: Bits Allocated: %1, Bits Stored: %2, Pixel Range %3 to %4, SIGNED?Pixel Representation: %5, Photometric interpretation: %6").arg(m_mainVolume->getImage(0)->getBitsAllocated()).arg(m_mainVolume->getImage(0)->getBitsStored()).arg(m_mainVolume->getVtkData()->GetScalarRange()[0]).arg(m_mainVolume->getVtkData()->GetScalarRange()[1]).arg(m_mainVolume->getImage(0)->getPixelRepresentation()).arg(m_mainVolume->getImage(0)->getPhotometricInterpretation()));
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
    if (m_slabThickness == 1  && isThickSlabActive())
    {
        DEBUG_LOG("Desactivem thick Slab i resetejem pipeline normal");
        m_thickSlabActive = false;
        // Resetejem el pipeline
        applyGrayscalePipeline();
        updateDisplayExtent();
        updateSliceAnnotationInformation();
        this->render();
    }
    if (m_slabThickness > 1 && !isThickSlabActive()) // La comprovacio es per constuir el pipeline nomes el primer cop
    {
        DEBUG_LOG("Activem thick Slab i resetejem pipeline amb thickSlab");
        m_thickSlabActive = true;
        // Resetejem el pipeline
        applyGrayscalePipeline();
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
        setSlabThickness(1);
    else
        setSlabThickness(m_slabThickness);
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
        m_firstSlabSlice -= difference / 2; // Divisió entera!
        m_lastSlabSlice += difference / 2;

        // Si la diferència és senar creix més per un dels límits
        if ((difference % 2) != 0)
        {
            // Si el thickness actual és parell creixem per sota
            if ((m_slabThickness % 2) == 0)
            {
                m_firstSlabSlice--;
            }
            else // Sinó creixem per dalt
            {
                m_lastSlabSlice++;
            }
        }
        // Check per si ens passem de rang superior o inferior
        if (m_firstSlabSlice < this->getMinimumSlice())
        {
            // Si ens passem per sota, cal compensar creixent per dalt
            m_lastSlabSlice = this->getMinimumSlice() + newSlabThickness - 1;
            m_firstSlabSlice = this->getMinimumSlice(); // Queda al límit inferior
        }
        else if (m_lastSlabSlice > m_maxSliceValue)
        {
            // Si ens passem per dalt, cal compensar creixent per sota
            m_firstSlabSlice = m_maxSliceValue - newSlabThickness + 1;
            m_lastSlabSlice = m_maxSliceValue;
        }
    }
    else // La diferència és negativa, decreix el thickness
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
            else // Sinó decreixem per avall
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
        m_currentSlice = 0;
    }
    else if (value + m_slabThickness - 1 > m_maxSliceValue)
    {
        m_currentSlice = m_maxSliceValue - m_slabThickness + 1;
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
    if (xyz[xIndex] < bounds[xIndex * 2]) // La x està per sota del mínim
    {
        xyz[xIndex] = bounds[xIndex * 2];
    }
    else if (xyz[xIndex] > bounds[xIndex * 2 + 1]) // La x està per sobre del màxim
    {
        xyz[xIndex] = bounds[xIndex * 2 + 1];
    }

    if (xyz[yIndex] < bounds[yIndex * 2]) // La y està per sota del mínim
    {
        xyz[yIndex] = bounds[yIndex * 2];
    }
    else if (xyz[yIndex] > bounds[yIndex * 2 + 1]) // La y està per sobre del màxim
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

    // S'esborren les anotacions
    if (m_mainVolume)
    {
        m_drawer->removeAllPrimitives();
    }

    // HACK
    // Desactivem el rendering per tal de millorar l'eficiència de tornar a executar el pipeline, 
    // ja que altrament es renderitza múltiples vegades i provoca efectes indesitjats com el flickering
    enableRendering(false);

    this->applyGrayscalePipeline();
    this->resetView(m_lastView);
    this->resetWindowLevelToDefault();
    this->updateWindowLevelData();

    // Activem el refresh i refresquem
    enableRendering(true);
    this->setAlignPosition(m_alignPosition);

    this->render();
}

void Q2DViewer::clearViewer()
{
    m_drawer->clearViewer();
}

void Q2DViewer::invertWindowLevel()
{
    // Passa el window level a negatiu o positiu, per invertir els colors
    double wl[2];
    double window;
    double level;

    this->getCurrentWindowLevel(wl);
    window = wl[0] * -1;
    level = wl[1];

    setWindowLevel(window, level);
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
            if (m_isImageFlipped || (m_rotateFactor == 2)) // Si la imatge està rotada o flipada, s'agafa l'altre punt
            {
                motionVector[0]=bounds[1]-viewerLeft[0];
            }
            else
            {
                motionVector[0]=bounds[0]-viewerLeft[0];
            }
            break;
        
        case Sagital:
            motionVector[1]=bounds[2]-viewerLeft[1];
            break;
        
        case Coronal:
            motionVector[0]=bounds[0]-viewerLeft[0];
            break;
    }

    pan(motionVector);

    // Canviem els rulers de posició
    m_anchoredRulerCoordinates->SetValue(0.95 , -0.9 , -0.95);
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
                motionVector[0] = bounds[0]-viewerRight[0];
            }
            else
            {
                motionVector[0] = bounds[1]-viewerRight[0];
            }
            break;
        
        case Sagital:
            motionVector[1] = bounds[3]-viewerRight[1];
            break;
        
        case Coronal:
            motionVector[0] = bounds[1]-viewerRight[0];
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

void Q2DViewer::setImageOrientation(const QString &orientation)
{
    QVector<QString> labels = getCurrentDisplayedImageOrientationLabels();
    QStringList desiredOrientationList = orientation.split("\\");
    QString desiredTop, desiredLeft;
    if (desiredOrientationList.count() == 2)
    {
        desiredTop = desiredOrientationList.at(0);
        desiredLeft = desiredOrientationList.at(1);
    }
    m_imageOrientationOperationsMapper->setInitialOrientation(labels[2], labels[3]);
    m_imageOrientationOperationsMapper->setDesiredOrientation(desiredTop, desiredLeft);
    
    // TODO ara mateix fet així és ineficient ja que es poden cridar fins a dos cops updateCamera() innecessàriament
    // Caldria refactoritzar els mètodes de rotació i flip per aplicar aquests canvis requerint un únic updateCamera()
    rotateClockWise(m_imageOrientationOperationsMapper->getNumberOfClockwiseTurnsToApply());
    if (m_imageOrientationOperationsMapper->requiresHorizontalFlip())
    {
        horizontalFlip();
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

    m_rotateFactor = (m_rotateFactor+times) % 4;
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

    topCorner[xIndex] = bounds[xIndex*2];
    topCorner[yIndex] = bounds[yIndex*2];
    topCorner[zIndex] = 0.0;

    bottomCorner[xIndex] = bounds[xIndex*2+1];
    bottomCorner[yIndex] = bounds[yIndex*2+1];
    bottomCorner[zIndex] = 0.0;

    // Apliquem el zoom 
    scaleToFit3D(topCorner, bottomCorner);
}

};  // End namespace udg

