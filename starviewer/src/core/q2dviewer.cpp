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
//thickslab
#include "vtkProjectionImageFilter.h"

// include's qt
#include <QResizeEvent>
#include <QSize>
#include <QMenu>
#include <QAction>
#include <QMutableMapIterator>
// Tools
#include "q2dviewertoolmanager.h"

// include's bàsics vtk
#include <QVTKWidget.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkImageViewer2.h>
#include <vtkCamera.h>
#include <vtkMath.h>
// composició d'imatges
#include <vtkImageCheckerboard.h>
#include <vtkImageBlend.h>
#include <vtkImageRectilinearWipe.h>
// anotacions
#include <vtkCornerAnnotation.h>
#include <vtkAxisActor2D.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkProperty2D.h>
#include <vtkProp.h>
#include <vtkScalarBarActor.h>
#include <vtkLookupTable.h>
// desar imatges
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkPNMWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkBMPWriter.h>
#include <vtkMetaImageWriter.h>
// voxel information
#include <vtkPointData.h>
#include <vtkCell.h>
#include <vtkPropPicker.h>
#include <vtkImageActor.h>

// displayed area
#include <vtkImageChangeInformation.h>
#include <vtkImageResample.h>
#include <vtkExtractVOI.h>

// grayscale pipeline
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageShiftScale.h>
#include <vtkWindowLevelLookupTable.h>

// projeccio de punts
#include <vtkMatrix4x4.h>
#include <vtkPlane.h>

namespace udg {

Q2DViewer::Q2DViewer( QWidget *parent )
: QViewer( parent ), m_lastView(Q2DViewer::Axial), m_currentSlice(0), m_currentPhase(0), m_overlayVolume(0), m_blender(0), m_picker(0), m_cornerAnnotations(0), m_enabledAnnotations(Q2DViewer::AllAnnotation), m_overlay( Q2DViewer::CheckerBoard ), m_sideRuler(0), m_bottomRuler(0), m_scalarBar(0), m_oldToolManager(0), m_rotateFactor(0), m_numberOfPhases(1), m_maxSliceValue(0), m_applyFlip(false), m_isImageFlipped(false),m_modalityLUTRescale(0), m_modalityLut(0), m_windowLevelLut(0), m_presentationLut(0), m_enabledOldTools(false), m_slabThickness(1), m_firstSlabSlice(0), m_lastSlabSlice(0), m_thickSlabActive(false), m_slabProjectionMode( AccumulatorFactory::Maximum )
{
    // CheckerBoard
    // el nombre de divisions per defecte, serà de 2, per simplificar
    m_divisions[0] = m_divisions[1] = m_divisions[2] = 2;
    m_imageSizeInformation[0] = 0;
    m_imageSizeInformation[1] = 0;

    // configuració del viewer
    m_imageRenderer = vtkRenderer::New();
    m_imageActor = vtkImageActor::New();
    m_windowLevelLUTMapper = vtkImageMapToWindowLevelColors::New();
    m_interactorStyle = vtkInteractorStyleImage::New();
    m_interactorStyle->AutoAdjustCameraClippingRangeOn();
    // per composar el thickSlab
    m_thickSlabProjectionFilter = vtkProjectionImageFilter::New();

    setupInteraction();
    // anotacions
    createAnnotations();
    addActors();

    //creem el drawer, passant-li com a visor l'objecte this
    m_drawer = new Drawer( this );
    connect( this, SIGNAL(cameraChanged()), SLOT(updateRulers()) );
}

Q2DViewer::~Q2DViewer()
{
    m_scalarBar->Delete();
    m_patientOrientationTextActor[0]->Delete();
    m_patientOrientationTextActor[1]->Delete();
    m_patientOrientationTextActor[2]->Delete();
    m_patientOrientationTextActor[3]->Delete();
    m_sideRuler->Delete();
    m_bottomRuler->Delete();
    m_cornerAnnotations->Delete();
    m_picker->Delete();
    m_vtkQtConnections->Delete();
    // TODO hem hagut de fer eliminar primer el drawer per davant d'altres objectes
    // per solucionar el ticket #539, però això denota que hi ha algun problema de
    // disseny que fa que no sigui prou robust. L'ordre en que s'esborren els objectes
    // no ens hauria d'afectar
    // HACK imposem que s'esborri primer el drawer
    delete m_drawer;
    delete m_oldToolManager;
}

vtkRenderer *Q2DViewer::getRenderer()
{
    return m_imageRenderer;
}

void Q2DViewer::createAnnotations()
{
    // contenidor d'anotacions 
    m_cornerAnnotations = vtkCornerAnnotation::New();
    m_cornerAnnotations->GetTextProperty()->SetFontFamilyToArial();
    m_cornerAnnotations->GetTextProperty()->ShadowOff();
    // escala de colors
    createScalarBar();
    // anotacions de l'orientació del pacient
    createOrientationAnnotations();
    // Marcadors d'escala
    createRulers();
}

void Q2DViewer::createOrientationAnnotations()
{
    // informació de referència de la orientació del pacient
    for( int i = 0; i < 4; i++ )
    {
        m_patientOrientationTextActor[i] = vtkTextActor::New();
        m_patientOrientationTextActor[i]->ScaledTextOff();
        m_patientOrientationTextActor[i]->GetTextProperty()->SetFontSize( 18 );
        m_patientOrientationTextActor[i]->GetTextProperty()->BoldOn();
        m_patientOrientationTextActor[i]->GetTextProperty()->SetFontFamilyToArial();
        m_patientOrientationTextActor[i]->GetTextProperty()->ShadowOff();

        m_patientOrientationTextActor[i]->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
        m_patientOrientationTextActor[i]->GetPosition2Coordinate()->SetCoordinateSystemToNormalizedViewport();
    }
    // ara posem la informació concreta de cadascuna de les referència d'orientació. 0-4 en sentit anti-horari, començant per 0 = esquerra de la pantalla
    m_patientOrientationTextActor[0]->GetTextProperty()->SetJustificationToLeft();
    m_patientOrientationTextActor[0]->SetPosition( 0.01 , 0.5 );

    m_patientOrientationTextActor[1]->GetTextProperty()->SetJustificationToCentered();
    m_patientOrientationTextActor[1]->SetPosition( 0.5 , 0.01 );

    m_patientOrientationTextActor[2]->GetTextProperty()->SetJustificationToRight();
    m_patientOrientationTextActor[2]->SetPosition( 0.99 , 0.5 );

    m_patientOrientationTextActor[3]->GetTextProperty()->SetJustificationToCentered();
    m_patientOrientationTextActor[3]->SetPosition( 0.5 , 0.99 );
}

void Q2DViewer::createRulers()
{
    // ruler lateral
    m_sideRuler = vtkAxisActor2D::New();
    m_sideRuler->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_sideRuler->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
    m_sideRuler->AxisVisibilityOn();
    m_sideRuler->TickVisibilityOn();
    m_sideRuler->LabelVisibilityOn();
    m_sideRuler->AdjustLabelsOff();
    m_sideRuler->SetLabelFormat("%.2f");
    m_sideRuler->SetLabelFactor( 0.35 );
    m_sideRuler->GetLabelTextProperty()->ItalicOff();
    m_sideRuler->GetLabelTextProperty()->BoldOff();
    m_sideRuler->GetLabelTextProperty()->ShadowOff();
    m_sideRuler->GetLabelTextProperty()->SetColor( 0 , 0.7 , 0 );
    m_sideRuler->GetLabelTextProperty()->SetFontFamilyToArial();
    m_sideRuler->TitleVisibilityOff();
    m_sideRuler->SetTickLength( 10 );
    m_sideRuler->GetProperty()->SetColor( 0 , 1 , 0 );
    m_sideRuler->VisibilityOff(); // per defecte, fins que no hi hagi input son invisibles

    // ruler inferior
    m_bottomRuler = vtkAxisActor2D::New();
    m_bottomRuler->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_bottomRuler->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
    m_bottomRuler->AxisVisibilityOn();
    m_bottomRuler->TickVisibilityOn();
    m_bottomRuler->LabelVisibilityOn();
    m_bottomRuler->AdjustLabelsOff();
    m_bottomRuler->SetLabelFormat("%.2f");
    m_bottomRuler->SetLabelFactor( 0.35 );
    m_bottomRuler->GetLabelTextProperty()->ItalicOff();
    m_bottomRuler->GetLabelTextProperty()->BoldOff();
    m_bottomRuler->GetLabelTextProperty()->ShadowOff();
    m_bottomRuler->GetLabelTextProperty()->SetColor( 0 , 0.7 , 0 );
    m_bottomRuler->GetLabelTextProperty()->SetFontFamilyToArial();
    m_bottomRuler->TitleVisibilityOff();
    m_bottomRuler->SetTickLength( 10 );
    m_bottomRuler->GetProperty()->SetColor( 0 , 1 , 0 );
    m_bottomRuler->VisibilityOff(); // per defecte, fins que no hi hagi input son invisibles

    // coordenades fixes per ancorar els rulers al lateral i a la part inferior
    m_anchoredRulerCoordinates = vtkCoordinate::New();
    m_anchoredRulerCoordinates->SetCoordinateSystemToView();
    m_anchoredRulerCoordinates->SetValue( -0.95 , -0.9 , -0.95 );
}

void Q2DViewer::updateRulers()
{
    double *anchoredCoordinates = m_anchoredRulerCoordinates->GetComputedWorldValue( this->getRenderer() );

    switch( m_lastView )
    {
    case Axial:
        m_sideRuler->GetPositionCoordinate()->SetValue( anchoredCoordinates[0] , m_rulerExtent[3] , 0.0 );
        m_sideRuler->GetPosition2Coordinate()->SetValue( anchoredCoordinates[0] , m_rulerExtent[2] , 0.0 );
        m_sideRuler->SetRange( m_rulerExtent[3] , m_rulerExtent[2] );

        m_bottomRuler->GetPositionCoordinate()->SetValue( m_rulerExtent[1] , anchoredCoordinates[1]  , 0.0 );
        m_bottomRuler->GetPosition2Coordinate()->SetValue( m_rulerExtent[0] , anchoredCoordinates[1] , 0.0  );
        m_bottomRuler->SetRange( m_rulerExtent[1] , m_rulerExtent[0] );
    break;

    case Sagital:
        m_sideRuler->GetPositionCoordinate()->SetValue( 0.0 , anchoredCoordinates[1] , m_rulerExtent[4] );
        m_sideRuler->GetPosition2Coordinate()->SetValue( 0.0 , anchoredCoordinates[1] , m_rulerExtent[5] );
        m_sideRuler->SetRange( m_rulerExtent[4] , m_rulerExtent[5] );

        m_bottomRuler->GetPositionCoordinate()->SetValue( 0.0 , m_rulerExtent[3] , anchoredCoordinates[2] );
        m_bottomRuler->GetPosition2Coordinate()->SetValue( 0.0 , m_rulerExtent[2] , anchoredCoordinates[2] );
        m_bottomRuler->SetRange( m_rulerExtent[3] , m_rulerExtent[2] );
    break;

    case Coronal:
        m_sideRuler->GetPositionCoordinate()->SetValue( anchoredCoordinates[0] , 0.0 , m_rulerExtent[4] );
        m_sideRuler->GetPosition2Coordinate()->SetValue( anchoredCoordinates[0] , 0.0 , m_rulerExtent[5] );
        m_sideRuler->SetRange( m_rulerExtent[4] , m_rulerExtent[5] );

        m_bottomRuler->GetPositionCoordinate()->SetValue( m_rulerExtent[1] , 0.0 , anchoredCoordinates[2] );
        m_bottomRuler->GetPosition2Coordinate()->SetValue( m_rulerExtent[0] , 0.0 , anchoredCoordinates[2] );
        m_bottomRuler->SetRange( m_rulerExtent[1] , m_rulerExtent[0] );
    break;
    }
}

void Q2DViewer::setupDefaultPipeline()
{
    if( m_mainVolume )
    {
        m_windowLevelLUTMapper->SetInput( m_mainVolume->getVtkData() );
    }
}

void Q2DViewer::setupThickSlabPipeline()
{
    m_windowLevelLUTMapper->SetInput( m_thickSlabProjectionFilter->GetOutput() );
}

void Q2DViewer::createScalarBar()
{
    m_scalarBar = vtkScalarBarActor::New();
    m_scalarBar->SetOrientationToVertical();
    m_scalarBar->GetPositionCoordinate()->SetCoordinateSystemToView();
    m_scalarBar->SetPosition( 0.8 , -0.8 );
    m_scalarBar->SetWidth( 0.08 );
    m_scalarBar->SetHeight( 0.6 );
    m_scalarBar->SetLabelFormat( " %.f  " );
    m_scalarBar->SetNumberOfLabels( 3 );
    m_scalarBar->GetLabelTextProperty()->ItalicOff();
    m_scalarBar->GetLabelTextProperty()->BoldOff();
    m_scalarBar->GetLabelTextProperty()->SetJustificationToRight();
    m_scalarBar->GetLabelTextProperty()->SetFontFamilyToArial();
    m_scalarBar->GetLabelTextProperty()->ShadowOff();
    m_scalarBar->VisibilityOff(); // inicialment sera invisible fins que no hi hagi input

    // Li configurem la lookup table
    vtkWindowLevelLookupTable *lookup = vtkWindowLevelLookupTable::New();
    lookup->SetWindow( m_windowLevelLUTMapper->GetWindow() );
    lookup->SetLevel( m_windowLevelLUTMapper->GetLevel() );
    lookup->Build();
    m_scalarBar->SetLookupTable( lookup );
}

void Q2DViewer::updateScalarBar()
{
    Q_ASSERT( m_scalarBar );
    vtkWindowLevelLookupTable::SafeDownCast( m_scalarBar->GetLookupTable() )->SetWindow( m_windowLevelLUTMapper->GetWindow() );
    vtkWindowLevelLookupTable::SafeDownCast( m_scalarBar->GetLookupTable() )->SetLevel( m_windowLevelLUTMapper->GetLevel() );
}

void Q2DViewer::rotateClockWise()
{
    if( m_isImageFlipped )
    {
        m_rotateFactor = (m_rotateFactor-1) % 4 ;
    }
    else
    {
        m_rotateFactor = (m_rotateFactor+1) % 4 ;
    }
    updateCamera();
}

void Q2DViewer::rotateCounterClockWise()
{
    if( m_isImageFlipped )
    {
        m_rotateFactor = (m_rotateFactor-3) % 4 ;
    }
    else
    {
        m_rotateFactor = (m_rotateFactor+3) % 4 ;
    }
    updateCamera();
}

void Q2DViewer::setRotationFactor( int factor )
{
    m_rotateFactor = factor;
    updateCamera();
}

void Q2DViewer::horizontalFlip()
{
    m_applyFlip = true;
    updateCamera();
}

void Q2DViewer::verticalFlip()
{
    m_rotateFactor = (m_rotateFactor + 2) % 4 ;
    horizontalFlip();
}

void Q2DViewer::setModalityRescale( double slope, double intercept )
{
    if( !m_modalityLUTRescale )
    {
        m_modalityLUTRescale = vtkImageShiftScale::New();
        m_modalityLUTRescale->SetInput( m_mainVolume->getVtkData() );
    }
    m_modalityLUTRescale->SetShift( slope );
    m_modalityLUTRescale->SetScale( intercept );
}

void Q2DViewer::setModalityLUT( vtkWindowLevelLookupTable *lut )
{
    if( !lut && m_modalityLut )
    {
        m_modalityLut->Delete();
    }
    m_modalityLut = lut;
}

void Q2DViewer::setVOILUT( vtkWindowLevelLookupTable *lut )
{
    if( !lut && m_windowLevelLut )
    {
        m_windowLevelLut->Delete();
    }
    m_windowLevelLut = lut;
}

void Q2DViewer::setPresentationLUT( vtkWindowLevelLookupTable *lut )
{
    if( !lut && m_presentationLut )
    {
        m_presentationLut->Delete();
    }
    m_presentationLut = lut;
}

void Q2DViewer::mapOrientationStringToAnnotation()
{
    //\TODO Cal comprovar que els flips siguin correctes
    int index = (m_lastView == Axial) ? m_currentSlice : 0;
    // això es fa per si tenim un mhd que realment només té un arxiu (imatge) però té més llesques
    // TODO caldria millorar l'accés a les imatges a partir del volum, per no haver de fer aquestes filigranes
    // és a dir, al preguntar a Volume, getImage(index) ell ens retorna la imatge que toca i ja comprova rangs si cal
    // i no ens retorna la llista d'imatges a saco
    index = ( index >= m_mainVolume->getImages().size() ) ? 0 : index;
    QString orientation = m_mainVolume->getImages().at(index)->getPatientOrientation();
    QStringList list = orientation.split(",");

    bool ok = false;
    switch( list.size() )
    {
    case 2:
        // afegim un element neutre perque la resta segueixi funcionant be
        ok = true;
        list << "";
        break;
    case 3:
        ok = true;
        break;
    }

    if( ok )
    {
        int index = 4-m_rotateFactor;
        // 0:Esquerra , 1:Abaix , 2:Dreta , 3:A dalt
        if( m_lastView == Axial )
        {
            m_patientOrientationTextActor[ (0 + index) % 4 ]->SetInput( qPrintable( this->getOppositeOrientationLabel( list.at(0) ) ) );
            m_patientOrientationTextActor[ (2 + index) % 4 ]->SetInput( qPrintable( list.at(0) ) );
            m_patientOrientationTextActor[ (1 + index) % 4 ]->SetInput( qPrintable( list.at(1) ) );
            m_patientOrientationTextActor[ (3 + index) % 4 ]->SetInput( qPrintable( this->getOppositeOrientationLabel( list.at(1) ) ) );
        }
        else if( m_lastView == Sagital )
        {
            // HACK FLIP de moment necessitem fer aquest truc. Durant el refactoring caldria
            // veure si es pot fer d'una manera millor
            if( m_isImageFlipped )
                index -= 2;

            m_patientOrientationTextActor[ (0 + index) % 4 ]->SetInput( qPrintable( this->getOppositeOrientationLabel( list.at(1) ) ) );
            m_patientOrientationTextActor[ (2 + index) % 4 ]->SetInput( qPrintable( list.at(1) ) );
            m_patientOrientationTextActor[ (1 + index) % 4 ]->SetInput( qPrintable( this->getOppositeOrientationLabel( list.at(2) ) ) );
            m_patientOrientationTextActor[ (3 + index) % 4 ]->SetInput( qPrintable( list.at(2) ) );
        }
        else if( m_lastView == Coronal )
        {
            // HACK FLIP de moment necessitem fer aquest truc. Durant el refactoring caldria
            // veure si es pot fer d'una manera millor
            if( m_isImageFlipped )
                index -= 2;

            m_patientOrientationTextActor[ (0 + index) % 4 ]->SetInput( qPrintable( this->getOppositeOrientationLabel( list.at(0) ) ) );
            m_patientOrientationTextActor[ (2 + index) % 4 ]->SetInput( qPrintable( list.at(0) ) );
            m_patientOrientationTextActor[ (1 + index) % 4 ]->SetInput( qPrintable( this->getOppositeOrientationLabel( list.at(2) ) ) );
            m_patientOrientationTextActor[ (3 + index) % 4 ]->SetInput( qPrintable( list.at(2) ) );
        }
        if( m_isImageFlipped )
        {
            // llavors caldrà intercanviar esquerra per dreta
            QString swap( m_patientOrientationTextActor[0]->GetInput() );
            m_patientOrientationTextActor[0]->SetInput( m_patientOrientationTextActor[2]->GetInput() );
            m_patientOrientationTextActor[2]->SetInput( qPrintable( swap ) );
        }
    }
    else
    {
        DEBUG_LOG("L'orientació del pacient conte un nombre incorrecte d'elements:[" + QString::number(list.size()) + "]. No s'aplicaran annotacions de referència sobre la imatge");
    }
}

void Q2DViewer::refreshAnnotations()
{
    if( !m_mainVolume )
        return;

    if( m_enabledAnnotations & Q2DViewer::PatientInformationAnnotation )
	{
        //m_serieInformationAnnotation->VisibilityOn();
		m_cornerAnnotations->SetText( 3, qPrintable( m_upperRightText ) );
        m_cornerAnnotations->SetText( 1, qPrintable( m_lowerRightText.trimmed() ) );
	}
    else
	{
        //m_serieInformationAnnotation->VisibilityOff();
		m_cornerAnnotations->SetText( 3, "" );
        m_cornerAnnotations->SetText( 1, "" );
	}

    if ( m_enabledAnnotations & Q2DViewer::RulersAnnotation )
    {
        m_sideRuler->VisibilityOn();
        m_bottomRuler->VisibilityOn();
    }
    else
    {
        m_sideRuler->VisibilityOff();
        m_bottomRuler->VisibilityOff();
    }

    if ( m_enabledAnnotations & Q2DViewer::PatientOrientationAnnotation )
    {
        for( int j = 0; j < 4; j++ )
            m_patientOrientationTextActor[j]->VisibilityOn();
    }
    else
    {
        for( int j = 0; j < 4; j++ )
            m_patientOrientationTextActor[j]->VisibilityOff();
    }

    if ( m_enabledAnnotations & Q2DViewer::ScalarBarAnnotation )
        m_scalarBar->VisibilityOn();
    else
        m_scalarBar->VisibilityOff();

	updateAnnotationsInformation( Q2DViewer::WindowInformationAnnotation | Q2DViewer::SliceAnnotation );
}

// TODO potser hauria de ser getCurrentSliceThickness ?
double Q2DViewer::getThickness()
{
    double thickness;
    switch( m_lastView )
    {
    case Axial:
    {
        // HACK fins que se solucioni de forma consistent el ticket #492
        if( isThickSlabActive() )
        {
            // si hi ha thickslab, llavors el thickness es basa a partir de la
            // suma de l'espai entre llesques
            // TODO repassar que això sigui del tot correcte
            thickness = m_mainVolume->getSpacing()[2] * m_slabThickness;
        }
        else
        {
            Image *image = getCurrentDisplayedImage();
            if(image)
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
    if( m_mainVolume )
    {
        if( m_numberOfPhases == 1 ) // si és un volum 3D normal...
        {
            m_mainVolume->updateInformation();
            int *extent = m_mainVolume->getWholeExtent();
            min = extent[m_lastView * 2];
            max = extent[m_lastView * 2 + 1];
        }
        else // si tenim 4D
        {
            // TODO assumim que sempre estem en axial!
            min = 0;
            max = m_mainVolume->getSeries()->getNumberOfSlicesPerPhase() - 1;
        }
    }
    else
    {
        min = max = 0;
        DEBUG_LOG("Alerta! Demanem el rang de llesques però no tenim volum! Valors de sortida = 0!");
    }
}

int *Q2DViewer::getSliceRange()
{
    if( m_mainVolume )
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
    Q_ASSERT( m_cornerAnnotations );
    Q_ASSERT( m_patientOrientationTextActor[0] );
    Q_ASSERT( m_patientOrientationTextActor[1] );
    Q_ASSERT( m_patientOrientationTextActor[2] );
    Q_ASSERT( m_patientOrientationTextActor[3] );
    Q_ASSERT( m_sideRuler );
    Q_ASSERT( m_bottomRuler );
    Q_ASSERT( m_scalarBar );
    Q_ASSERT( m_imageActor );

    // anotacions de texte 
    this->getRenderer()->AddViewProp( m_cornerAnnotations );
    this->getRenderer()->AddViewProp( m_patientOrientationTextActor[0] );
    this->getRenderer()->AddViewProp( m_patientOrientationTextActor[1] );
    this->getRenderer()->AddViewProp( m_patientOrientationTextActor[2] );
    this->getRenderer()->AddViewProp( m_patientOrientationTextActor[3] );
    this->getRenderer()->AddViewProp( m_sideRuler );
    this->getRenderer()->AddViewProp( m_bottomRuler );
    this->getRenderer()->AddViewProp( m_scalarBar );
    m_imageRenderer->AddViewProp( m_imageActor );
    // TODO colocar aix`o en un lloc mes adient
    m_imageRenderer->GetActiveCamera()->ParallelProjectionOn();
}

QString Q2DViewer::getOppositeOrientationLabel( const QString &label )
{
    int i = 0;
    QString oppositeLabel;
    while( i < label.size() )
    {
        if( label.at( i ) == 'L' )
            oppositeLabel += "R";
        else if( label.at( i ) == 'R' )
            oppositeLabel += "L";
        else if( label.at( i ) == 'A' )
            oppositeLabel += "P";
        else if( label.at( i ) == 'P' )
            oppositeLabel += "A";
        else if( label.at( i ) == 'S' )
            oppositeLabel += "I";
        else if( label.at( i ) == 'I' )
            oppositeLabel += "S";
        else if( label.at( i ) == 'H' )
            oppositeLabel += "F";
        else if( label.at( i ) == 'F' )
            oppositeLabel += "H";
        else
            oppositeLabel += "?";
        i++;
    }
    return oppositeLabel;
}

void Q2DViewer::setOldTool( QString toolName )
{
    // per poder cridar aquesta funcio caldra haver fet
    // abans un "enableTools" que crei el manager
    // amb l'assert trobarem facilment on cal fer-lo
    Q_ASSERT( m_oldToolManager );
    if( m_oldToolManager->setCurrentTool( toolName ) )
    {
        ///\Todo per implementar
        DEBUG_LOG( QString("OK, hem activat la tool: ") + toolName );
    }
    else
    {
        ///\Todo per implementar
        DEBUG_LOG( QString(":/ no s'ha pogut activar la tool: ") + toolName );
    }
}

OldTool *Q2DViewer::getOldTool( QString toolName )
{
    // per poder cridar aquesta funcio caldra haver fet
    // abans un "enableTools" que crei el manager
    // amb l'assert trobarem facilment on cal fer-lo
    Q_ASSERT( m_oldToolManager );
    return m_oldToolManager->getTool( toolName );
}

QString Q2DViewer::getCurrentOldToolName()
{
    // per poder cridar aquesta funcio caldra haver fet
    // abans un "enableTools" que crei el manager
    // amb l'assert trobarem facilment on cal fer-lo
    Q_ASSERT( m_oldToolManager );
    return m_oldToolManager->getCurrentToolName();
}

void Q2DViewer::setEnableOldTools( bool enable )
{
    if( enable )
        this->enableOldTools();
    else
        this->disableOldTools();
}

void Q2DViewer::enableOldTools()
{
    // Això evita que es faci més d'un connect en cas que es cridi aquesta funció i ja s'hagi fet abans
    if( !m_enabledOldTools )
    {
        // el creem si encara no existeix
        if( !m_oldToolManager )
            m_oldToolManager = new Q2DViewerToolManager( this );

        connect( this , SIGNAL( eventReceived(unsigned long) ) , m_oldToolManager , SLOT( forwardEvent(unsigned long) ) );
        m_enabledOldTools = true;
    }
}

void Q2DViewer::disableOldTools()
{
    if( m_enabledOldTools )
    {
        if( m_oldToolManager )
            disconnect( this , SIGNAL( eventReceived(unsigned long) ) , m_oldToolManager , SLOT( forwardEvent(unsigned long) ) );

        m_enabledOldTools = false;
    }
}

void Q2DViewer::setupInteraction()
{
    Q_ASSERT( m_imageRenderer );
    Q_ASSERT( m_vtkWidget );
    Q_ASSERT( m_interactorStyle );

    this->getRenderWindow()->AddRenderer( m_imageRenderer );
    this->getInteractor()->SetInteractorStyle( m_interactorStyle );
    m_interactorStyle->SetCurrentRenderer( m_imageRenderer );
    m_windowToImageFilter->SetInput( this->getRenderer()->GetRenderWindow() );

    m_picker = vtkPropPicker::New();
    // configurem la interacció de qvtkWidget
    m_vtkWidget->GetRenderWindow()->GetInteractor()->SetPicker( m_picker );

    // \TODO fer això aquí? o fer-ho en el tool manager?
    this->getInteractor()->RemoveObservers( vtkCommand::LeftButtonPressEvent );
    this->getInteractor()->RemoveObservers( vtkCommand::RightButtonPressEvent );
    this->getInteractor()->RemoveObservers( vtkCommand::MouseWheelForwardEvent );
    this->getInteractor()->RemoveObservers( vtkCommand::MouseWheelBackwardEvent );
    this->getInteractor()->RemoveObservers( vtkCommand::MiddleButtonPressEvent );
    this->getInteractor()->RemoveObservers( vtkCommand::CharEvent );

    m_vtkQtConnections = vtkEventQtSlotConnect::New();
    // despatxa qualsevol event-> tools
    m_vtkQtConnections->Connect( m_vtkWidget->GetRenderWindow()->GetInteractor(),
                                 vtkCommand::AnyEvent,
                                 this,
#ifdef VTK_QT_5_0_SUPPORT
                                 SLOT( eventHandler(vtkObject*, unsigned long, void*, vtkCommand*) )
#else
                                 SLOT( eventHandler(vtkObject*, unsigned long, void*, void*, vtkCommand*) )
#endif
                                 );
}

void Q2DViewer::setInput( Volume* volume )
{
    if( !volume )
        return;


    //al fer un nou input, les distàncies que guardava el drawer no tenen sentit, pertant s'esborren
    if( m_mainVolume )
        m_drawer->removeAllPrimitives();

    m_mainVolume = volume;
    m_currentSlice = 0;
    m_currentPhase = 0;
    m_lastView = Q2DViewer::Axial;

    // Inicialització del thickSlab
    m_slabThickness = 1;
    m_firstSlabSlice = 0;
    m_lastSlabSlice = 0;
    m_thickSlabActive = false;
    // obtenim valors de gris i aquestes coses
    // aquí es crea tot el pieline del visualitzador
    this->computeInputGrayscalePipeline();
    this->applyGrayscalePipeline();

    int extent[6];
    double origin[3], spacing[3];
    m_mainVolume->getOrigin( origin );
    m_mainVolume->getSpacing( spacing );
    m_mainVolume->getWholeExtent( extent );
    m_rulerExtent[0] = origin[0];
    m_rulerExtent[1] = origin[0] + extent[1]*spacing[0];
    m_rulerExtent[2] = origin[1];
    m_rulerExtent[3] = origin[1] + extent[3]*spacing[1];
    m_rulerExtent[4] = origin[2];
    m_rulerExtent[5] = origin[2] + extent[5]*spacing[2];

    m_numberOfPhases = m_mainVolume->getNumberOfPhases();
    m_maxSliceValue = this->getMaximumSlice();

    // Això es fa per destruir el blender en cas que ja hi hagi algun input i es vulgui canviar
    if(m_blender!=0)
    {
        m_blender->Delete();
        m_blender = 0;
    }

    // Preparem el thickSlab // TODO cada cop que fem setInput resetejem els valors per defecte??
    m_thickSlabProjectionFilter->SetInput( m_mainVolume->getVtkData() );
    m_thickSlabProjectionFilter->SetProjectionDimension( m_lastView );
    m_thickSlabProjectionFilter->SetAccumulatorType( (AccumulatorFactory::AccumulatorType) m_slabProjectionMode );
    m_thickSlabProjectionFilter->SetFirstSlice( m_firstSlabSlice );
    m_thickSlabProjectionFilter->SetNumberOfSlicesToProject( m_slabThickness );
    m_thickSlabProjectionFilter->SetStep( m_numberOfPhases );

    updateDisplayExtent(); // TODO BUG sino fem aquesta crida ens peta al canviar d'input entre un que fos més gran que l'anterior
    resetViewToAxial();

    updatePatientAnnotationInformation();
    this->enableAnnotation( m_enabledAnnotations );

    // actualitzem la informació de window level
    this->updateWindowLevelData();
    // \TODO això no sabem si serà del tot necessari
    //     m_picker->PickFromListOn();
    //     m_picker->AddPickList( m_imageActor );
}

void Q2DViewer::setOverlayInput( Volume* volume )
{
    m_overlayVolume = volume;

    // \TODO s'hauria d'eliminar aquests objectes en algun punt (veure cas del blend)
    vtkImageCheckerboard *imageCheckerBoard = vtkImageCheckerboard::New();
    vtkImageRectilinearWipe *wipe = vtkImageRectilinearWipe::New();
    // \TODO hauríem d'actualitzar valors que es calculen al setInput de la variable m_overlay!
    switch( m_overlay )
    {
    case None:

        // actualitzem el viewer
        m_windowLevelLUTMapper->RemoveAllInputs();
        m_windowLevelLUTMapper->SetInput( m_mainVolume->getVtkData() );
        // \TODO hauríem d'actualitzar valors que es calculen al setInput!
    break;

    case CheckerBoard:

        imageCheckerBoard->SetInput1( m_mainVolume->getVtkData() );
        imageCheckerBoard->SetInput2( m_overlayVolume->getVtkData() );
        imageCheckerBoard->SetNumberOfDivisions( m_divisions );
        // actualitzem el pipeline
        m_windowLevelLUTMapper->SetInputConnection( imageCheckerBoard->GetOutputPort() );
        //updateDisplayExtent?
        // \TODO hauríem d'actualitzar valors que es calculen al setInput!
    break;

    case Blend:
        if (m_blender==0)
        {
            m_blender = vtkImageBlend::New();
            m_blender->SetInput(0, m_mainVolume->getVtkData());
        }
        m_blender->SetInput(1, m_overlayVolume->getVtkData());
        m_blender->SetOpacity( 1, 1.0 - m_opacityOverlay );
        // \TODO Revisar la manera de donar-li l'input d'un blending al visualitzador
        m_blender->Modified();
        m_windowLevelLUTMapper->SetInputConnection( m_blender->GetOutputPort() );

    break;

    case RectilinearWipe:
        wipe->SetInput( 0 , m_mainVolume->getVtkData() );
        wipe->SetInput( 1 , m_overlayVolume->getVtkData() );
        wipe->SetPosition(20,20);
        wipe->SetWipeToUpperLeft();
        m_windowLevelLUTMapper->SetInput( wipe->GetOutput() );
        // \TODO hauríem d'actualitzar valors que es calculen al setInput!
    break;
    }
}

void Q2DViewer::setOpacityOverlay ( double op )
{
    m_opacityOverlay=op;
}

void Q2DViewer::render()
{
    // si tenim dades
    if( m_mainVolume )
    {
        this->getRenderWindow()->Render();
    }
    else
    {
        DEBUG_LOG( "::render() : No hi ha cap volum per visualitzar" );
    }
}

void Q2DViewer::resetView( CameraOrientationType view )
{
    m_lastView = view;
    // TODO aquest signal el mantenim aquí i no el posem
    // al final del mètode, com semblaria lògic, degut
    // a que cal millorar la interacció amb QThickSlabWidget
    // ara si es posa al final, després de resetCamera, peta
    emit viewChanged( m_lastView );

    // thick Slab, li indiquem la direcció de projecció
    m_thickSlabProjectionFilter->SetProjectionDimension( m_lastView );

    resetCamera();
}

void Q2DViewer::resetViewToAxial()
{
    resetView( Q2DViewer::Axial );
}

void Q2DViewer::resetViewToCoronal()
{
    resetView( Q2DViewer::Coronal );
}

void Q2DViewer::resetViewToSagital()
{
    resetView( Q2DViewer::Sagital );
}

void Q2DViewer::updateCamera()
{
    if( m_mainVolume )
    {
        vtkCamera *camera = this->getRenderer()->GetActiveCamera();
        Q_ASSERT( camera );

        double roll = 0.0;

        switch( m_lastView )
        {
        case Axial:
            if( m_isImageFlipped )
                roll = m_rotateFactor*90. + 180.;
            else
                roll = -m_rotateFactor*90. + 180.;
        break;

        case Sagital:
            if( m_isImageFlipped )
                roll = m_rotateFactor*90. -90.;
            else
                roll = -m_rotateFactor*90. - 90.;
        break;

        case Coronal:
            if( m_isImageFlipped )
                roll = m_rotateFactor*90.;
            else
                roll = -m_rotateFactor*90.;
        break;
        }

        camera->SetRoll( roll );
        emit rotationDegreesChanged( roll );

        if( m_applyFlip )
        {
            // Alternativa 1)
            // TODO així movem la càmera, però faltaria que la imatge no es mogués de lloc
            // potser implementant a la nostra manera el metode Azimuth i prenent com a centre
            // el centre de la imatge. Una altra possibilitat es contrarestar el desplaçament de la
            // camera en l'eix en que s'ha produit
            camera->Azimuth( 180 );
            switch( this->m_lastView )
            {
            // HACK aquest hack esta relacionat amb els de mapOrientationStringToAnnotation()
            // es un petit truc perque la imatge quedi orientada correctament. Caldria
            // veure si en el refactoring podem fer-ho d'una forma millor
            case Sagital:
            case Coronal:
                m_rotateFactor = (m_rotateFactor - 2) % 4;
                break;

            default:
                break;
            }

            this->getRenderer()->ResetCameraClippingRange();
            m_applyFlip = false;
            m_isImageFlipped = ! m_isImageFlipped;
        }

        emit rotationFactorChanged( m_rotateFactor );
        emit cameraChanged();
        mapOrientationStringToAnnotation();
        this->refresh();
    }
    else
    {
        DEBUG_LOG( "Intentant actualitzar rotació de càmera sense haver donat un input abans..." );
    }
}

void Q2DViewer::resetCamera()
{
    if( m_mainVolume )
    {
        // en comptes de fer servir sempre this->getMaximumSlice(), actualitzem
        // aquest valor quan cal, és a dir, al posar input i al canviar de vista
        // estalviant-nos crides i crides
        m_maxSliceValue = this->getMaximumSlice();

        // reiniciem valors per defecte de la càmera
        m_rotateFactor = 0;
        m_applyFlip = false;
        m_isImageFlipped = false;

        vtkCamera *camera = m_imageRenderer->GetActiveCamera();
        Q_ASSERT( camera );

        double bounds[6];
        QString position;
        switch( m_lastView )
        {
        case Axial:
            // ajustem la càmera
            camera->SetFocalPoint(0,0,0);
            camera->SetViewUp(0,-1,0);
            camera->SetPosition(0,0,-1);
            camera->SetRoll( -m_rotateFactor*90. + 180. );

            // posicionem la imatge TODO no ho fem amb setSlice() perquè introdueix flickering
            checkAndUpdateSliceValue(0);
            updateDisplayExtent();
            m_imageRenderer->ResetCamera();

            // ajustem la imatge al viewport
            m_imageActor->GetBounds( bounds );
            scaleToFit3D( bounds[1], bounds[3], 0.0, bounds[0], bounds[2], 0.0 );

            m_imageSizeInformation[0] = m_mainVolume->getDimensions()[0];
            m_imageSizeInformation[1] = m_mainVolume->getDimensions()[1];
        break;

        case Sagital:
            // ajustem la càmera
            camera->SetFocalPoint(0,0,0);
            camera->SetPosition(1,0,0); // -1 if medical ?
            camera->SetViewUp(0,0,1);
            camera->SetRoll( -m_rotateFactor*90. -90. );

            // posicionem la imatge TODO no ho fem amb setSlice() perquè introdueix flickering
            checkAndUpdateSliceValue(m_maxSliceValue/2);
            updateDisplayExtent();
            m_imageRenderer->ResetCamera();

            // ajustem la imatge al viewport
            m_imageActor->GetBounds( bounds );
            scaleToFit3D( 0.0, bounds[2], bounds[5], 0.0, bounds[3], bounds[4] );

            // TODO solucio inmediata per afrontar el ticket #355, pero s'hauria de fer d'una manera mes elegant i consistent
            position = m_mainVolume->getSeries()->getPatientPosition();
            if( position == "FFP" || position == "HFP" )
                m_rotateFactor = (m_rotateFactor+2) % 4 ;

            //\TODO hauria de ser a partir de main_volume o a partir de l'output del viewer
            m_imageSizeInformation[0] = m_mainVolume->getDimensions()[1];
            m_imageSizeInformation[1] = m_mainVolume->getDimensions()[2];

        break;

        case Coronal:
            // ajustem la càmera
            camera->SetFocalPoint(0,0,0);
            camera->SetPosition(0,-1,0); // 1 if medical ?
            camera->SetViewUp(0,0,1);
            camera->SetRoll( -m_rotateFactor*90. );

            // posicionem la imatge TODO no ho fem amb setSlice() perquè introdueix flickering
            checkAndUpdateSliceValue(m_maxSliceValue/2);
            updateDisplayExtent();
            m_imageRenderer->ResetCamera();

            // ajustem la imatge al viewport
            m_imageActor->GetBounds( bounds );
            scaleToFit3D( bounds[1], 0.0, bounds[4], bounds[0], 0.0, bounds[5] );

            // TODO solucio inmediata per afrontar el ticket #355, pero s'hauria de fer d'una manera mes elegant i consistent
            position = m_mainVolume->getSeries()->getPatientPosition();
            if( position == "FFP" || position == "HFP" )
                m_rotateFactor = (m_rotateFactor+2) % 4 ;

            //\TODO hauria de ser a partir de main_volume o a partir de l'output del viewer
            m_imageSizeInformation[0] = m_mainVolume->getDimensions()[0];
            m_imageSizeInformation[1] = m_mainVolume->getDimensions()[2];
        break;
        }
		updateAnnotationsInformation( Q2DViewer::SliceAnnotation | Q2DViewer::WindowInformationAnnotation );
        mapOrientationStringToAnnotation();
        // TODO potser això no és del tot correcte, cal fer més consistent conjuntament amb setSlice
        emit sliceChanged( m_currentSlice );
    }
    else
    {
        DEBUG_LOG( "Intentant canviar de vista sense haver donat un input abans..." );
    }
}

vtkImageMapToWindowLevelColors *Q2DViewer::getWindowLevelMapper() const
{
    return m_windowLevelLUTMapper;
}

void Q2DViewer::setSlice( int value )
{
    if( this->m_mainVolume && this->m_currentSlice != value )
    {
        this->checkAndUpdateSliceValue( value );
        if( isThickSlabActive() )
        {
            m_thickSlabProjectionFilter->SetFirstSlice( m_firstSlabSlice );
            // TODO cal actualitzar aquest valor?
            m_thickSlabProjectionFilter->SetNumberOfSlicesToProject( m_slabThickness );
            //si hi ha el thickslab activat, eliminem totes les roi's. És la decisió ràpida que s'ha près.
            this->getDrawer()->removeAllPrimitives();
        }
        this->updateDisplayExtent();
        // TODO per cada canvi de llesca potser també caldria
        // comprovar si el ww/wwl és diferent i aplicar el que toqui ( #478 )
        updateSliceAnnotationInformation();
        mapOrientationStringToAnnotation();
        emit sliceChanged( m_currentSlice );
    }
}

void Q2DViewer::setPhase( int value )
{
    // comprovació de rang
    if( m_mainVolume )
    {
        if( value < 0 )
            value = 0;
        else if( value > m_numberOfPhases - 1 )
            value = m_numberOfPhases - 1;

        m_currentPhase = value;
        this->updateDisplayExtent();
		updateSliceAnnotationInformation();
        emit phaseChanged( m_currentPhase );
        this->refresh();
    }
}

void Q2DViewer::setOverlay( OverlayType overlay )
{
    m_overlay = overlay;
}

void Q2DViewer::setNoOverlay()
{
    setOverlay( Q2DViewer::None );
    DEBUG_LOG("Passem pel setNoOverlay()");
    m_windowLevelLUTMapper->RemoveAllInputs();
    m_windowLevelLUTMapper->SetInput( m_mainVolume->getVtkData() );
}

void Q2DViewer::setOverlayToBlend()
{
    setOverlay( Q2DViewer::Blend );
}

void Q2DViewer::setOverlayToCheckerBoard()
{
    setOverlay( Q2DViewer::CheckerBoard );
}

void Q2DViewer::setOverlayToRectilinearWipe()
{
    setOverlay( Q2DViewer::RectilinearWipe );
}

void Q2DViewer::resizeEvent( QResizeEvent *vtkNotUsed(resize) )
{
    if( m_mainVolume )
    {
        // l'única info que cal actualitzar és la mida de finestra/viewport
        updateAnnotationsInformation( Q2DViewer::WindowInformationAnnotation );
        updateRulers();
    }
}

void Q2DViewer::setWindowLevel( double window , double level )
{
    if( m_mainVolume )
    {
        m_windowLevelLUTMapper->SetWindow( window );
        m_windowLevelLUTMapper->SetLevel( level );
        updateAnnotationsInformation( Q2DViewer::WindowInformationAnnotation );
        updateScalarBar();
        this->refresh();
        emit windowLevelChanged( window , level );
    }
    else
    {
        DEBUG_LOG( "::setWindowLevel() : No tenim input " );
    }
}

void Q2DViewer::getCurrentWindowLevel( double wl[2] )
{
    if( m_mainVolume )
    {
        wl[0] = m_windowLevelLUTMapper->GetWindow();
        wl[1] = m_windowLevelLUTMapper->GetLevel();
    }
    else
    {
        DEBUG_LOG( "::getCurrentWindowLevel() : No tenim input " );
    }
}

double Q2DViewer::getCurrentColorWindow()
{
    if( m_mainVolume )
    {
        return m_windowLevelLUTMapper->GetWindow();
    }
    else
    {
        DEBUG_LOG( "::getCurrentColorWindow() : No tenim input " );
        return 0;
    }
}

double Q2DViewer::getCurrentColorLevel()
{
    if( m_mainVolume )
    {
        return m_windowLevelLUTMapper->GetLevel();
    }
    else
    {
        DEBUG_LOG( "::getCurrentColorLevel() : No tenim input " );
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

void Q2DViewer::resetWindowLevelToDefault()
{
    // això ens dóna un level/level "maco" per defecte
    // situem el level al mig i donem un window complet de tot el rang
    //\TODO aquí caldria tenir en compte el default del presentation state actual si l'hi ha
    this->setWindowLevel( m_defaultWindow, m_defaultLevel );
}

void Q2DViewer::setModalityRescale( vtkImageShiftScale *rescale )
{
    m_modalityLUTRescale = rescale;
}

vtkImageActor *Q2DViewer::getImageActor()
{
    return m_imageActor;
}

void Q2DViewer::setPixelAspectRatio( double ratio )
{
    if( ratio != 1.0 && ratio > 0.0 )
    {
        m_mainVolume->updateInformation();
        double spacing[3];
        m_mainVolume->getSpacing( spacing );

        vtkImageChangeInformation *change = vtkImageChangeInformation::New();
        change->SetInput( m_imageActor->GetInput() );

        if( ratio > 1.0 )
            change->SetOutputSpacing( spacing[0]*ratio, spacing[1], spacing[2] );
        else
            change->SetOutputSpacing( spacing[0], spacing[1]*ratio, spacing[2] );

        m_imageActor->SetInput( change->GetOutput() );
    }
    else
    {
        DEBUG_LOG( QString("Ratio no aplicable: %1").arg(ratio) );
    }
}

void Q2DViewer::setPresentationPixelSpacing( double x , double y )
{
    m_presentationPixelSpacing[0] = x;
    m_presentationPixelSpacing[1] = y;
}

void Q2DViewer::setTrueSizeMode( bool on )
{
    if( on )
    {
        DEBUG_LOG("Presentation size mode: TRUE SIZE:: NO IMPLEMENTAT ENCARA!");
        // \TODO això encara no funciona
    }
}

void Q2DViewer::setMagnificationFactor( double factor )
{
    // presumiblement un SetScale(factor,factor,1) de l'ImageActor podria ser una solució alternativa
    if( factor > 0.0 && factor != 1.0 )
    {
        m_mainVolume->updateInformation();
        double spacing[3];
        m_mainVolume->getSpacing( spacing );
        vtkImageChangeInformation *change = vtkImageChangeInformation::New();
        change->SetInput( m_imageActor->GetInput() );
        change->SetOutputSpacing( spacing[0]*factor, spacing[1]*factor, spacing[2] );
        vtkImageResample *resample = vtkImageResample::New();
        resample->SetInput( change->GetOutput() );
        resample->SetAxisMagnificationFactor( 0, factor );
        resample->SetAxisMagnificationFactor( 1, factor );
        resample->SetAxisMagnificationFactor( 2, 1.0 );
        resample->SetAxisOutputSpacing( 0, spacing[0]*factor );
        resample->SetAxisOutputSpacing( 1, spacing[1]*factor );
        resample->SetAxisOutputSpacing( 2, 1.0 );
        resample->Update();
        m_imageActor->SetInput( resample->GetOutput() );
    }
    else
    {
        DEBUG_LOG( QString("Factor no aplicable: %1").arg(factor) );
    }
}

Image *Q2DViewer::getCurrentDisplayedImage() const
{
    return m_mainVolume->getImage( m_currentSlice, m_currentPhase );
}

ImagePlane *Q2DViewer::getCurrentImagePlane( bool vtkReconstructionHack )
{
    return this->getImagePlane( m_currentSlice, m_currentPhase, vtkReconstructionHack );
}

ImagePlane *Q2DViewer::getImagePlane( int sliceNumber , int phaseNumber, bool vtkReconstructionHack )
{
    ImagePlane *imagePlane = 0;
    if( m_mainVolume )
    {
        int *dimensions = m_mainVolume->getDimensions();
        double *spacing = m_mainVolume->getSpacing();
        const double *origin  = m_mainVolume->getOrigin();
        switch( m_lastView )
        {
            case Axial: // XY
            {
                Image *image = m_mainVolume->getImage( sliceNumber, phaseNumber );
                if( image )
                {
                    imagePlane = new ImagePlane();
                    imagePlane->fillFromImage( image );
                }
            }
            break;

            case Sagital: // YZ TODO encara no esta comprovat que aquest pla sigui correcte
            {
                Image *image = m_mainVolume->getSeries()->getImages().at(0);
                if( image )
                {
                    imagePlane = new ImagePlane();

                    const double *dirCosines = image->getImageOrientationPatient();

                    if( vtkReconstructionHack )
                    {
                        // retornem un fals pla, respecte el món real, però que s'ajusta més al món vtk
                        imagePlane->setRowDirectionVector( dirCosines[3], dirCosines[4], dirCosines[5] );
                        imagePlane->setColumnDirectionVector( dirCosines[6], dirCosines[7], dirCosines[8] );
                    }
                    else
                    {
                        // això serà lo normal, retornar la autèntica direcció del pla
                        double columnVector[3];
                        m_mainVolume->getStackDirection( columnVector, 0 );

                        imagePlane->setRowDirectionVector( dirCosines[3], dirCosines[4], dirCosines[5] );
                        imagePlane->setColumnDirectionVector( columnVector[0], columnVector[1], columnVector[2] );
                    }

                    imagePlane->setSpacing( spacing[1], spacing[2] );
                    imagePlane->setThickness( spacing[0] );
                    imagePlane->setRows( dimensions[2] );
                    imagePlane->setColumns( dimensions[1] );

                    // TODO falta esbrinar si l'origen que estem donant es bo o no
                    imagePlane->setOrigin( origin[0] + sliceNumber*dirCosines[0]*spacing[0],
                                           origin[1] + sliceNumber*dirCosines[1]*spacing[0],
                                           origin[2] + sliceNumber*dirCosines[2]*spacing[0] );
                }
            }
            break;

            case Coronal: // XZ TODO encara no esta comprovat que aquest pla sigui correcte
            {
                Image *image = m_mainVolume->getSeries()->getImages().at(0);
                if( image )
                {
                    imagePlane = new ImagePlane();
                    const double *dirCosines = image->getImageOrientationPatient();

                    if( vtkReconstructionHack )
                    {
                        // retornem un fals pla, respecte el món real, però que s'ajusta més al món vtk
                        imagePlane->setRowDirectionVector( dirCosines[0], dirCosines[1], dirCosines[2] );
                        imagePlane->setColumnDirectionVector( dirCosines[6], dirCosines[7], dirCosines[8] );
                    }
                    else
                    {
                        double columnVector[3];
                        m_mainVolume->getStackDirection( columnVector, 0 );

                        imagePlane->setRowDirectionVector( dirCosines[0], dirCosines[1], dirCosines[2] );
                        imagePlane->setColumnDirectionVector( columnVector[0], columnVector[1], columnVector[2] );
                    }


                    imagePlane->setSpacing( spacing[0], spacing[2] );
                    imagePlane->setThickness( spacing[1] );
                    imagePlane->setRows( dimensions[2] );
                    imagePlane->setColumns( dimensions[0] );

                    // TODO falta esbrinar si l'origen que estem donant es bo o no
                    imagePlane->setOrigin( origin[0] + dirCosines[3]*sliceNumber*spacing[1],
                                           origin[1] + dirCosines[4]*sliceNumber*spacing[1],
                                           origin[2] + dirCosines[5]*sliceNumber*spacing[1]
                                         );
                }
            }
            break;
        }
    }
    return imagePlane;
}

void Q2DViewer::projectDICOMPointToCurrentDisplayedImage( const double pointToProject[3], double projectedPoint[3], bool vtkReconstructionHack )
{
    //
    // AQUÍ SUMEM L'origen TAL CUAL + L'ERROR DE DESPLAÇAMENT VTK
    //
    // La projecció es fa de la següent manera:
    // Primer es fa una  una projecció convencional del punt sobre el pla actual (DICOM)
    // Com que el mapeig de coordenades VTK va a la seva bola, necessitem corretgir el desplaçament
    // introduit per VTK respecte a les coordenades "reals" de DICOM
    // aquest desplaçament consistirà en tornar a sumar l'origen del primer pla del volum
    // en principi, fer-ho amb l'origen de m_mainVolume també seria correcte
    //
    ImagePlane *currentPlane = this->getCurrentImagePlane(vtkReconstructionHack);
    if( currentPlane )
    {
        // recollim les dades del pla actual sobre el qual volem projectar el punt de l'altre pla
        double currentPlaneRowVector[3], currentPlaneColumnVector[3], currentPlaneNormalVector[3], currentPlaneOrigin[3];
        currentPlane->getRowDirectionVector( currentPlaneRowVector );
        currentPlane->getColumnDirectionVector( currentPlaneColumnVector );
        currentPlane->getNormalVector( currentPlaneNormalVector );
        currentPlane->getOrigin( currentPlaneOrigin );

        // a partir d'aquestes dades creem la matriu de projecció,
        // que projectarà el punt donat sobre el pla actual
        vtkMatrix4x4 *projectionMatrix = vtkMatrix4x4::New();
        projectionMatrix->Identity();
        for( int column = 0; column < 3; column++ )
        {
            projectionMatrix->SetElement(0,column,currentPlaneRowVector[ column ]);
            projectionMatrix->SetElement(1,column,currentPlaneColumnVector[ column ]);
            projectionMatrix->SetElement(2,column,currentPlaneNormalVector[ column ]);
        }

        // un cop tenim la matriu podem fer la projeccio
        // necessitem el punt en coordenades homogenies
        double homogeneousPointToProject[4], homogeneousProjectedPoint[4];
        for( int i=0; i<3; i++ )
            homogeneousPointToProject[i] = pointToProject[i] - currentPlaneOrigin[i]; // desplacem el punt a l'origen del pla
        homogeneousPointToProject[3] = 1.0;

        // projectem el punt amb la matriu
        projectionMatrix->MultiplyPoint( homogeneousPointToProject, homogeneousProjectedPoint );

        //
        // CORRECIÓ VTK!
        //
        // a partir d'aquí cal corretgir l'error introduit pel mapeig que fan les vtk
        // cal sumar l'origen de la primera imatge, o el que seria el mateix, l'origen de m_mainVolume
        //
        // TODO provar si amb l'origen de m_mainVolume també funciona bé
        Image *firstImage = m_mainVolume->getImages().at(0);
        const double *ori = firstImage->getImagePositionPatient();

        // segons si hem fet una reconstrucció ortogonal haurem de fer
        // alguns canvis sobre la projecció
        switch( m_lastView )
        {
            case Axial:
                for( int i = 0; i<3; i++ )
                    projectedPoint[i] = homogeneousProjectedPoint[i] + ori[i];
            break;

            case Sagital:
            {
                if( vtkReconstructionHack )
                {
                    // HACK que serveix de parxe pels casos de crani que no van be. TODO encara està per acabar, és una primera aproximació
                    projectionMatrix->SetElement(0,0,0);
                    projectionMatrix->SetElement(0,1,1);
                    projectionMatrix->SetElement(0,2,0);
                    // projectem el punt amb la matriu
                    projectionMatrix->MultiplyPoint( homogeneousPointToProject, homogeneousProjectedPoint );
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

Drawer *Q2DViewer::getDrawer() const
{
    return m_drawer;
}

bool Q2DViewer::getCurrentCursorPosition( double xyz[3] )
{
    bool found = false;
    if( !m_mainVolume )
        return found;
    // agafem el punt que està apuntant el ratolí en aquell moment \TODO podríem passar-li el 4t parèmatre opcional (vtkPropCollection) per indicar que només agafi de l'ImageActor, però no sembla que suigui necessari realment i que si fa pick d'un altre actor 2D no passa res
    int position[2];
    this->getEventPosition( position );
    m_picker->PickProp( position[0], position[1], m_imageRenderer );
    // calculem el pixel trobat
    m_picker->GetPickPosition( xyz );

    // quan dona una posició de (0,0,0) és que estem fora de l'actor
    if( !( xyz[0] == 0 && xyz[1] == 0 && xyz[2] == 0) )
    {
        double tolerance;
        int subCellId;
        double parametricCoordinates[3], interpolationWeights[8];

        // Use tolerance as a function of size of source data
        tolerance = m_mainVolume->getVtkData()->GetLength();
        tolerance = tolerance ? tolerance*tolerance / 1000.0 : 0.001;

        // HACK per solucionar el problema amb el metode FindAndGetCell que necessita
        // més presició que la que obtenim amb el GetPickPosition
        double *origin = m_mainVolume->getVtkData()->GetOrigin();
        if( abs(xyz[0] - origin[0] ) < 0.00001 ) xyz[0] = origin[0];
        if( abs(xyz[1] - origin[1] ) < 0.00001 ) xyz[1] = origin[1];
        if( abs(xyz[2] - origin[2] ) < 0.00001 ) xyz[2] = origin[2];

        // Find the cell that contains q and get it
        vtkCell *cell = m_mainVolume->getVtkData()->FindAndGetCell( xyz , NULL , -1 , tolerance , subCellId , parametricCoordinates , interpolationWeights );
        if ( cell )
        {
            found = true;
        }
    }
    if( !found )
    {
        xyz[0] = -1;
        xyz[1] = -1;
        xyz[2] = -1;
    }

    return found;
}

double Q2DViewer::getCurrentImageValue()
{
    double xyz[3];
    bool found = false;
    double imageValue;
    if( this->getCurrentCursorPosition(xyz) )
    {
        double tolerance;
        int subCellId;
        double parametricCoordinates[3], interpolationWeights[8];

        vtkPointData *pointData = m_mainVolume->getVtkData()->GetPointData();
        vtkPointData* outPointData = vtkPointData::New();
        outPointData->InterpolateAllocate( pointData , 1 , 1 );

        // Use tolerance as a function of size of source data
        tolerance = m_mainVolume->getVtkData()->GetLength();
        tolerance = tolerance ? tolerance*tolerance / 1000.0 : 0.001;

        // Find the cell that contains q and get it
        vtkCell *cell = m_mainVolume->getVtkData()->FindAndGetCell( xyz , NULL , -1 , tolerance , subCellId , parametricCoordinates , interpolationWeights );
        if ( cell )
        {
            // Interpolate the point data
            outPointData->InterpolatePoint( pointData , 0 , cell->PointIds , interpolationWeights );
            imageValue = outPointData->GetScalars()->GetTuple1(0);
            found = true;
        }
        outPointData->Delete();
    }
    if( !found )
    {
        DEBUG_LOG("No s'ha trobat valor");
        return -1.;
    }
    else
    {
        return imageValue;
    }
}

Q2DViewer::CameraOrientationType Q2DViewer::getView() const
{
    return m_lastView;
}

void Q2DViewer::reset()
{
    //\TODO: completar, encara és incomplert
    resetViewToAxial();
}

void Q2DViewer::setDivisions( int x , int y , int z )
{
    m_divisions[0] = x;
    m_divisions[1] = y;
    m_divisions[2] = z;
}

void Q2DViewer::setDivisions( int data[3] )
{
    m_divisions[0] = data[0];
    m_divisions[1] = data[1];
    m_divisions[2] = data[2];
}

int* Q2DViewer::getDivisions( void )
{
    return m_divisions;
}

void Q2DViewer::getDivisions( int data[3] )
{
    data[0] = m_divisions[0];
    data[1] = m_divisions[1];
    data[2] = m_divisions[2];
}

void Q2DViewer::setSeedPosition( double pos[3] )
{
    m_seedPosition[0] = pos[0];
    m_seedPosition[1] = pos[1];
    m_seedPosition[2] = pos[2];
    emit seedChanged();
}

void Q2DViewer::getSeedPosition( double pos[3] )
{
    pos[0] = m_seedPosition[0];
    pos[1] = m_seedPosition[1];
    pos[2] = m_seedPosition[2];
}

void Q2DViewer::saveCurrent( QString baseName , FileType extension )
{
    m_windowToImageFilter->Update();
    m_windowToImageFilter->Modified();
    vtkImageData *image = m_windowToImageFilter->GetOutput();
    switch( extension )
    {
        case PNG:
        {
            vtkImageWriter *pngWriter = vtkPNGWriter::New();
            pngWriter->SetInput( image );
            pngWriter->SetFilePattern( "%s-%d.png" );
            pngWriter->SetFilePrefix( qPrintable(baseName) );
            pngWriter->Write();

            break;
        }
        case JPEG:
        {
            vtkImageWriter *jpegWriter = vtkJPEGWriter::New();
            jpegWriter->SetInput( image );
            jpegWriter->SetFilePattern( "%s-%d.jpg" );
            jpegWriter->SetFilePrefix( qPrintable(baseName) );
            jpegWriter->Write();

            break;
        }
        // \TODO el format tiff fa petar al desar, mirar si és problema de compatibilitat del sistema o de les pròpies vtk
        case TIFF:
        {
            vtkImageWriter *tiffWriter = vtkTIFFWriter::New();
            tiffWriter->SetInput( image );
            tiffWriter->SetFilePattern( "%s-%d.tif" );
            tiffWriter->SetFilePrefix( qPrintable(baseName) );
            tiffWriter->Write();

            break;
        }
        case PNM:
        {
            vtkImageWriter *pnmWriter = vtkPNMWriter::New();
            pnmWriter->SetInput( image );
            pnmWriter->SetFilePattern( "%s-%d.pnm" );
            pnmWriter->SetFilePrefix( qPrintable(baseName) );
            pnmWriter->Write();

            break;
        }
        case BMP:
        {
            vtkImageWriter *bmpWriter = vtkBMPWriter::New();
            bmpWriter->SetInput( image );
            bmpWriter->SetFilePattern( "%s-%d.bmp" );
            bmpWriter->SetFilePrefix( qPrintable(baseName) );
            bmpWriter->Write();

            break;
        }
        case DICOM:
        {
            break;
        }
        case META:
        {
            vtkMetaImageWriter *metaWriter = vtkMetaImageWriter::New();
            metaWriter->SetInput( m_mainVolume->getVtkData() );
            metaWriter->SetFileName( qPrintable(baseName) );
            metaWriter->Write();

            break;
        }
    }
}

void Q2DViewer::updateAnnotationsInformation( AnnotationFlags annotation )
{
    if( !m_mainVolume )
        return;

    // Informació que es mostra per cada viewport
    if( annotation & Q2DViewer::WindowInformationAnnotation )
    {
        // informació de la finestra
        if( m_enabledAnnotations & Q2DViewer::WindowInformationAnnotation )
        {
			m_upperLeftText = tr("%1 x %2\nWW: %5 WL: %6")
                .arg( m_imageSizeInformation[0] )
                .arg( m_imageSizeInformation[1] )
                .arg( (int)vtkMath::Round( m_windowLevelLUTMapper->GetWindow() ) )
                .arg( (int)vtkMath::Round( m_windowLevelLUTMapper->GetLevel() ) );
        }
        else
            m_upperLeftText = "";
        m_cornerAnnotations->SetText( 2 , qPrintable( m_upperLeftText ) );
    }

    if( annotation & Q2DViewer::SliceAnnotation )
        this->updateSliceAnnotationInformation();
}

void Q2DViewer::updatePatientAnnotationInformation()
{
    if( m_mainVolume )
    {
        // informació fixa
        m_upperRightText = tr("%1\n%2\n%3 %4 %5\nAcc:%6\n%7\n%8")
                    .arg( m_mainVolume->getSeries()->getInstitutionName() )
					.arg( m_mainVolume->getPatient()->getFullName() )
					.arg( m_mainVolume->getStudy()->getPatientAge() )
                    .arg( m_mainVolume->getPatient()->getSex() )
                    .arg( m_mainVolume->getPatient()->getID() )
                    .arg( m_mainVolume->getStudy()->getAccessionNumber() )
                    .arg( m_mainVolume->getStudy()->getDateAsString() )
                    .arg( m_mainVolume->getStudy()->getTimeAsString() ); // TODO seria més correcte mostrar l'hora de la sèrie i inclús de la imatge


        // Si protocol i descripció coincideixen posarem el contingut de protocol
        // Si són diferents, els fusionarem
        QString protocolName, description;
        protocolName = m_mainVolume->getSeries()->getProtocolName();
        description = m_mainVolume->getSeries()->getDescription();
        m_lowerRightText = protocolName;
        if( description != protocolName )
            m_lowerRightText += "\n" + description;

        m_cornerAnnotations->SetText( 3, qPrintable( m_upperRightText ) );
        m_cornerAnnotations->SetText( 1, qPrintable( m_lowerRightText.trimmed() ) );
    }
    else
    {
        DEBUG_LOG("No hi ha un volum vàlid. No es poden inicialitzar les annotacions de texte d'informació de pacient");
    }

}

void Q2DViewer::updateSliceAnnotationInformation()
{
    Q_ASSERT( m_cornerAnnotations );

    int value = m_currentSlice*m_numberOfPhases + m_currentPhase;
    if( m_numberOfPhases > 1 )
    {
        this->updateSliceAnnotation( (value/m_numberOfPhases) + 1, m_maxSliceValue + 1, m_currentPhase + 1, m_numberOfPhases );
    }
    else
    {
        this->updateSliceAnnotation( value+1, m_maxSliceValue+1 );
    }
}

void Q2DViewer::updateSliceAnnotation( int currentSlice, int maxSlice, int currentPhase, int maxPhase )
{
    Q_ASSERT( m_cornerAnnotations );

    if( m_enabledAnnotations & Q2DViewer::SliceAnnotation ) // si les annotacions estan habilitades
    {
        QString lowerLeftText;
        if( maxPhase > 1 ) // tenim fases
        {
            if( m_slabThickness > 1 )
            {
                lowerLeftText = tr("Slice: %1-%2/%3 Phase: %4/%5")
                        .arg( currentSlice )
                        .arg( currentSlice+m_slabThickness-1 )
                        .arg( maxSlice )
                        .arg( currentPhase )
                        .arg( maxPhase );
            }
            else
            {
                lowerLeftText = tr("Slice: %1/%2 Phase: %3/%4")
                        .arg( currentSlice )
                        .arg( maxSlice )
                        .arg( currentPhase )
                        .arg( maxPhase );
            }
        }
        else // només llesques
        {
            if( m_slabThickness > 1 )
            {
                lowerLeftText = tr("Slice: %1-%2/%3")
                        .arg( currentSlice )
                        .arg( currentSlice+m_slabThickness-1 )
                        .arg( maxSlice );
            }
            else
            {
                lowerLeftText = tr("Slice: %1/%2")
                        .arg( currentSlice )
                        .arg( maxSlice );
            }
        }
        //afegim el thickness de la llesca nomes si es > 0mm
        if ( this->getThickness() > 0.0 )
            lowerLeftText += tr(" Thickness: %1 mm").arg( this->getThickness(), 0, 'g', 2 );

        m_cornerAnnotations->SetText( 0 , qPrintable(lowerLeftText) );
    }
    else
    {
        m_cornerAnnotations->SetText( 0 , "" );
    }
}

void Q2DViewer::updateDisplayExtent()
{
    Q_ASSERT( m_imageActor );

    vtkImageData *input = m_mainVolume->getVtkData();
    if( !input )
        return;

    // thick slab
    int sliceValue;
    if( isThickSlabActive() )
    {
        // en comptes de currentSlice podria ser m_firstSlabSlice, que és equivalent
        sliceValue = m_currentSlice; // Podria ser 0, dependent de l'extent de sortida del filtre
    }
    else
        sliceValue = m_currentSlice*m_numberOfPhases + m_currentPhase;

    input->UpdateInformation();
    int *wholeExtent = input->GetWholeExtent();
    switch( m_lastView )
    {
        case Axial:
            m_imageActor->SetDisplayExtent( wholeExtent[0], wholeExtent[1], wholeExtent[2], wholeExtent[3], sliceValue, sliceValue );
            break;

        case Coronal:
            m_imageActor->SetDisplayExtent( wholeExtent[0], wholeExtent[1], sliceValue, sliceValue, wholeExtent[4], wholeExtent[5] );
            break;

        case Sagital:
            m_imageActor->SetDisplayExtent( sliceValue, sliceValue, wholeExtent[2], wholeExtent[3], wholeExtent[4], wholeExtent[5] );
            break;
    }
    // TODO si separem els renderers potser caldria aplicar-ho a cada renderer?
    m_imageRenderer->ResetCameraClippingRange();
}

void Q2DViewer::enableAnnotation( AnnotationFlags annotation, bool enable )
{
    if( enable )
        m_enabledAnnotations = m_enabledAnnotations | annotation;
    else
        m_enabledAnnotations =  m_enabledAnnotations & ~annotation ;

    refreshAnnotations();
    this->refresh();
}

void Q2DViewer::removeAnnotation( AnnotationFlags annotation )
{
    enableAnnotation( annotation, false );
}

void Q2DViewer::computeInputGrayscalePipeline()
{
    if( m_mainVolume )
    {
        // si llegim l'arxiu tal qual, la modality no cal aplicar-la perquè les pròpies gdcm ja ens apliquen la modality lut
        computeModalityLUT();
        computeVOILUT();
    }
}

void Q2DViewer::applyGrayscalePipeline()
{
    DEBUG_LOG( "*** Grayscale Transform Pipeline Begin ***" );
    DEBUG_LOG( QString("Image Information: Bits Allocated: %1, Bits Stored: %2, Pixel Range %3 to %4, SIGNED?Pixel Representation: %5, Photometric interpretation: %6")
    .arg( m_mainVolume->getImages()[m_currentSlice]->getBitsAllocated() )
    .arg( m_mainVolume->getImages()[m_currentSlice]->getBitsStored() )
    .arg( m_mainVolume->getVtkData()->GetScalarRange()[0] )
    .arg( m_mainVolume->getVtkData()->GetScalarRange()[1] )
    .arg( m_mainVolume->getImages().at(0)->getPixelRepresentation() )
    .arg( m_mainVolume->getImages().at(0)->getPhotometricInterpretation() )
                     );
//\TODO Això s'ha d'aplicar enfunció de si tenim presentationm state o no? mirar si s'ha de fer aquí o al presentation state attacher...

//     if( m_mainVolume->getImages().at(0)->getPhotometricInterpretation() =="MONOCHROME1" && m_presentationStateFilename != NULL )
//     {
//         DEBUG_LOG("La imatge és MONOCHROME1: ¿invertim les dades després de la VOI LUT i abans de la presentation LUT?... Si hi ha presentation state això no s'hauria de fer!");
//         if( m_presentationLut )
//         {
//             m_presentationLut->InverseVideoOn();
//             DEBUG_LOG("Inverteixo la PRESENTATION LUT");
//         }
//         else if( m_windowLevelLut )
//         {
//             m_windowLevelLut->InverseVideoOn();
//             DEBUG_LOG("Inverteixo la VOI LUT");
//         }
//         else if( m_modalityLut )
//         {
//             m_modalityLut->InverseVideoOn();
//             DEBUG_LOG("Inverteixo la MODALITY LUT");
//         }
//     }
    // crear el pipeline en funció de tot el que ens hem trobat
    if( m_modalityLut )
    {
        if( m_windowLevelLut )
        {
            if( m_presentationLut ) // modality lut + windowlevel lut + presentation lut
            {
                DEBUG_LOG("Grayscale pipeline: Source Data -> Modality LUT -> Window Level LUT -> Presentation LUT -> Output  :: FIRST TRY OF IMPLEMENTATION!");
                m_windowLevelLUTMapper->SetInput( m_mainVolume->getVtkData() );
                m_windowLevelLUTMapper->SetLookupTable( m_presentationLut );
                m_imageActor->SetInput( m_windowLevelLUTMapper->GetOutput() );
                // es dóna per fet que els paràmetres correctes de window level ja estan calculats, ja sigui per especificació explícita o per assignació automàtica
                m_windowLevelLUTMapper->SetWindow( m_defaultWindow );
                m_windowLevelLUTMapper->SetLevel( m_defaultLevel );
            }
            else // modality lut + windowlevel lut
            {
                DEBUG_LOG("Grayscale pipeline: Source Data -> Modality LUT -> Window Level LUT -> Output  :: NOT IMPLEMENTED YET!");
            }
        }
        else
        {
            if( m_presentationLut ) // modality lut + presentation lut [ + ww/wl ]
            {
                DEBUG_LOG("Grayscale pipeline: Source Data -> Modality LUT -> [Window Level] -> Presentation LUT -> Output  :: NOT IMPLEMENTED YET!");
            }
            else // modality [ + ww/wl ]
            {
                DEBUG_LOG("Grayscale pipeline: Source Data -> Modality LUT -> [Window Level] -> Output ");
                m_windowLevelLUTMapper->SetInput( m_mainVolume->getVtkData() );
                m_windowLevelLUTMapper->SetLookupTable( m_modalityLut );
                m_imageActor->SetInput( m_windowLevelLUTMapper->GetOutput() );
                // es dóna per fet que els paràmetres correctes de window level ja estan calculats, ja sigui per especificació explícita o per assignació automàtica
                m_windowLevelLUTMapper->SetWindow( m_defaultWindow );
                m_windowLevelLUTMapper->SetLevel( m_defaultLevel );
            }
        }
    }
    else
    {
        if( m_windowLevelLut )
        {
            if( m_presentationLut )
            {
                if( m_modalityLUTRescale ) // rescale slope + windowlevel lut + presentation lut
                {
                    DEBUG_LOG("Grayscale pipeline: Source Data -> RescaleSlope -> Window Level LUT -> Presentation LUT -> Output  :: NOT IMPLEMENTED YET!");
                }
                else // windowlevel lut + presentation lut
                {
                    DEBUG_LOG("Grayscale pipeline: Source Data -> Window Level LUT -> Presentation LUT -> Output :: NOT IMPLEMENTED YET!");
                }
            }
            else // windowlevel lut
            {
                if( m_modalityLUTRescale ) // rescale slope + windowlevel lut
                {
                    DEBUG_LOG("Grayscale pipeline: Source Data -> RescaleSlope -> Window Level LUT -> Output ");
                    m_windowLevelLUTMapper->SetInput( m_modalityLUTRescale->GetOutput() );
                }
                else // windowlevel lut
                {
                    DEBUG_LOG("Grayscale pipeline: Source Data -> Window Level LUT -> Output ");
                    m_windowLevelLUTMapper->SetInput( m_mainVolume->getVtkData() );
                }
                m_windowLevelLUTMapper->SetLookupTable( m_windowLevelLut );
                m_imageActor->SetInput( m_windowLevelLUTMapper->GetOutput() );
                // es dóna per fet que el window level queda ajustat per defecte
                m_windowLevelLUTMapper->SetWindow( m_defaultWindow );
                m_windowLevelLUTMapper->SetLevel( m_defaultLevel );
            }
        }
        else
        {
            if( m_presentationLut )
            {
                if( m_modalityLUTRescale ) // rescale slope + [ww/wl +] presentation
                {
                    DEBUG_LOG("Grayscale pipeline: Source Data -> RescaleSlope -> [Window Level] -> Presentation LUT -> Output ");
                    m_modalityLUTRescale->SetInput( m_mainVolume->getVtkData() );
                    m_windowLevelLUTMapper->SetInput( m_modalityLUTRescale->GetOutput() );
                }
                else // [ww/wl +] presentation
                {
                    DEBUG_LOG("Grayscale pipeline: Source Data -> [Window Level] -> Presentation LUT -> Output ");
                    m_windowLevelLUTMapper->SetInput( m_mainVolume->getVtkData() );
                }
                m_windowLevelLUTMapper->SetLookupTable( m_presentationLut );
                // es dóna per fet que els paràmetres correctes de window level ja estan calculats, ja sigui per especificació explícita o per assignació automàtica
                m_windowLevelLUTMapper->SetWindow( m_defaultWindow );
                m_windowLevelLUTMapper->SetLevel( m_defaultLevel );
                m_imageActor->SetInput( m_windowLevelLUTMapper->GetOutput() );
            }
            else
            {
                if( m_modalityLUTRescale ) // rescale slope
                {
                    DEBUG_LOG("Grayscale pipeline: Source Data -> RescaleSlope -> [Window Level] -> Output ");
//                     m_modalityLUTRescale->SetInput( m_mainVolume->getVtkData() );
                    m_windowLevelLUTMapper->SetInput( m_modalityLUTRescale->GetOutput() );
                }
                else // res
                {
                    DEBUG_LOG("Grayscale pipeline: Source Data -> [Window Level] -> Output ");
                    m_windowLevelLUTMapper->SetInput( m_mainVolume->getVtkData() );
                }
                m_windowLevelLUTMapper->SetWindow( m_defaultWindow );
                m_windowLevelLUTMapper->SetLevel( m_defaultLevel );
                m_imageActor->SetInput( m_windowLevelLUTMapper->GetOutput() );
            }
        }
    }
}

void Q2DViewer::setSlabProjectionMode( int projectionMode )
{
    m_slabProjectionMode = projectionMode;
    m_thickSlabProjectionFilter->SetAccumulatorType( static_cast<AccumulatorFactory::AccumulatorType>( m_slabProjectionMode ) );
    updateDisplayExtent();
    this->refresh();
}

int Q2DViewer::getSlabProjectionMode() const
{
    return m_slabProjectionMode;
}

void Q2DViewer::setSlabThickness( int thickness )
{
    //primera aproximació per evitar error dades de primitives: a l'activar o desactivar l'slabthickness, esborrem primitives
    if ( thickness != m_slabThickness )
        this->getDrawer()->removeAllPrimitives();

    computeRangeAndSlice( thickness );
    // TODO comprovar aquest pipeline si és millor calcular ara o més tard
    if( m_slabThickness == 1  && isThickSlabActive() )
    {
        DEBUG_LOG( "desconnectar" );
        setupDefaultPipeline();
        m_thickSlabActive = false;
        updateDisplayExtent();
        this->refresh();
    }
    if ( m_slabThickness > 1 && !isThickSlabActive() ) // la comprovacio es per constuir el pipeline nomes el primer cop
    {
        DEBUG_LOG( "connectar" );
        setupThickSlabPipeline();
        m_thickSlabActive = true;
    }

    m_lastSlabSlice = m_currentSlice + m_slabThickness - 1;

    if( isThickSlabActive() )
    {
        m_thickSlabProjectionFilter->SetFirstSlice( m_firstSlabSlice );
        m_thickSlabProjectionFilter->SetNumberOfSlicesToProject( m_slabThickness );
        updateDisplayExtent();
        updateSliceAnnotationInformation();
        this->refresh();
    }

    // TODO és del tot correcte que vagi aquí aquesta crida?
    // tal com està posat se suposa que sempre el valor de thickness ha
    // canviat i podria ser que no, seria més adequat posar-ho a computerangeAndSlice?
    emit slabThicknessChanged( m_slabThickness );

}

int Q2DViewer::getSlabThickness() const
{
    return m_slabThickness;
}

void Q2DViewer::enableThickSlab( bool enable )
{
    if(!enable)
        setSlabThickness(1);
    else
        setSlabThickness( m_slabThickness );
}

bool Q2DViewer::isThickSlabActive() const
{
    return m_thickSlabActive;
}

void Q2DViewer::computeModalityLUT()
{
    // If the Modality LUT or equivalent Attributes are part of both the Image and the Presentation State, then the Presentation State Modality LUT shall be used instead of the Image Modality LUT or equivalent Attributes in the Image. If the Modality LUT is not present in the Presentation State it shall be assumed to be an identity transformation. Any Modality LUT or equivalent Attributes in the Image shall not be used.

    // busquem si existeix una modality lut TODO eliminem el parseLookupTable, això hauria de ser feina de
    // la classe que apliqui les transformacions de grisos i  presentation states
    m_modalityLut = NULL;
    if( m_modalityLut )
    {
        m_modalityRange[0] = m_modalityLut->GetTableRange()[0];
        m_modalityRange[1] = m_modalityLut->GetTableRange()[1];
        if( m_modalityLUTRescale )
        {
            m_modalityLUTRescale->Delete();
            m_modalityLUTRescale = 0;
        }
    }
    // si no hi ha lut busquem els paràmetres de rescale
    else
    {
        // mirar el de la imatge, només per curiositat perquè les itk ja l'apliquen directament
        DEBUG_LOG( QString("Image Modality LUT Adjustment: Rescale Slope %1, Rescale Intercept %2")
        .arg( m_mainVolume->getImages().at(0)->getRescaleSlope() )
        .arg( m_mainVolume->getImages().at(0)->getRescaleIntercept() )
         );
        if( m_modalityLUTRescale )
        {
            m_modalityLUTRescale->Delete();
            m_modalityLUTRescale = 0;
        }
        m_modalityRange[0] = m_mainVolume->getVtkData()->GetScalarRange()[0];
        m_modalityRange[1] = m_mainVolume->getVtkData()->GetScalarRange()[1];
    }
}

void Q2DViewer::computeVOILUT()
{
    //
    // 3. VOI LUT
    //
    // If a VOI LUT is part of both the Image and the Presentation State then the Presentation State VOI LUT shall be used instead of the Image VOI LUT. If a VOI LUT (that applies to the Image) is not present in the Presentation State , it shall be assumed to be an identity transformation. Any VOI LUT or equivalent values in the Image shall not be used.

    // aquests canvis es poden aplicar a un subconjunt de imatges/frames. Per tant podem tenir diverses VOI LUT per una mateixa sèrie que s'apliquen a diverses imatges.

    // primer busquem voi lut, sinó busquem window level TODO eliminem el parseLookupTable, això hauria de ser feina de
    // la classe que apliqui les transformacions de grisos i  presentation states
    m_windowLevelLut = NULL;
    // si hi ha una VOI LUT
    if( m_windowLevelLut )
    {
        // Ara podem considerar els tres casos anteriors de la modality: hi ha rescale, hi ha lut o no hi ha res. el 1r i el tercer es tracten igual.
        if( m_modalityLut )
        {
            DEBUG_LOG("Encara no sabem què fer en aquest cas (concatenar modality + voi LUT)");
        }
        else
        {
            m_defaultWindow = m_windowLevelLut->GetTableRange()[1] - m_windowLevelLut->GetTableRange()[0];
            m_defaultLevel = m_defaultWindow / 2.0;
        }
    }
    else
    {
        // només mirem el del nostre propi volum

        if( m_mainVolume->getImages().at(0)->getNumberOfWindowLevels() > 0 )
        {
            // Encara que en tingui més d'un window level, agafarem el primer i prou. Si n'hi ha més s'escolliran desde l'extensió adequada
            m_defaultWindow = m_mainVolume->getImages().at(0)->getWindowLevel().first;
            m_defaultLevel = m_mainVolume->getImages().at(0)->getWindowLevel().second;
            if( m_defaultWindow == 0.0 && m_defaultLevel == 0.0 )
            {
                double *range = m_mainVolume->getVtkData()->GetScalarRange();
                m_defaultWindow = range[1] - range[0];
                m_defaultLevel = (m_defaultWindow / 2.) + range[0];
            }
            DEBUG_LOG( QString("Image VOI Adjustment: Window: %1, Level: %2")
                .arg( m_defaultWindow )
                .arg( m_defaultLevel )
                );
        }
        else
        {
            // ajustar un al rang de dades adequat
            m_defaultWindow = fabs( m_modalityRange[1] - m_modalityRange[0] );
            m_defaultLevel = ( m_modalityRange[1] + m_modalityRange[0] )/ 2.0;
            DEBUG_LOG( QString("No Image VOI Adjustment, creating a nice and automatic one: Window: %1, Level: %2")
            .arg( m_defaultWindow )
            .arg( m_defaultLevel )
            );
        }
    }
}

void Q2DViewer::computeRangeAndSlice( int newSlabThickness )
{
    // checking del nou valor
    if( newSlabThickness < 1 )
    {
        DEBUG_LOG(" valor invàlid de thickness. Ha de ser >= 1 !!!!!");
        return;
    }
    if( newSlabThickness == m_slabThickness )
    {
        DEBUG_LOG(" tenim el mateix slab thickness, no canviem res ");
        return;
    }
    if( newSlabThickness > m_maxSliceValue + 1 )
    {
        DEBUG_LOG(" el nou thickness supera el thickness màxim, tot queda igual ");
        // TODO podríem aplicar newSlabThickness=m_maxSliceValue+1
        return;
    }

    int difference = newSlabThickness - m_slabThickness;
    // si la diferència és positiva, augmentem el thickness
    if( difference > 0 )
    {
        m_firstSlabSlice -= difference / 2; // divisió entera!
        m_lastSlabSlice += difference / 2;

        // si la diferència és senar creix més per un dels límits
        if( (difference % 2) != 0 )
        {
            // si el thickness actual és parell creixem per sota
            if( (m_slabThickness % 2) == 0 )
                m_firstSlabSlice--;
            else // sinó creixem per dalt
                m_lastSlabSlice++;
        }
        //check per si ens passem de rang superior o inferior
        if( m_firstSlabSlice < this->getMinimumSlice() )
        {
            // si ens passem per sota, cal compensar creixent per dalt
            m_lastSlabSlice = this->getMinimumSlice() + newSlabThickness - 1;
            m_firstSlabSlice = this->getMinimumSlice(); // queda al límit inferior
        }
        else if( m_lastSlabSlice > m_maxSliceValue )
        {
            // si ens passem per dalt, cal compensar creixent per sota
            m_firstSlabSlice = m_maxSliceValue - newSlabThickness + 1;
            m_lastSlabSlice = m_maxSliceValue;
        }
    }
    else // la diferència és negativa, decreix el thickness
    {
        // la convertim a positiva per conveniència
        difference *= -1;
        m_firstSlabSlice += difference / 2;
        m_lastSlabSlice -= difference / 2;

        // si la diferència és senar decreix més per un dels límits
        if( (difference % 2) != 0 )
        {
            // si el thickness actual és parell decreixem per amunt
            if( (m_slabThickness%2) == 0 )
                m_lastSlabSlice--;
            else // sinó decreixem per avall
                m_firstSlabSlice++;
        }
    }
    // actualitzem el thickness
    m_slabThickness = newSlabThickness;
    // actualitzem la llesca
    m_currentSlice = m_firstSlabSlice;
}

void Q2DViewer::checkAndUpdateSliceValue( int value )
{
    if( value < 0 )
        m_currentSlice = 0;
    else if( value + m_slabThickness-1 > m_maxSliceValue )
        m_currentSlice = m_maxSliceValue - m_slabThickness + 1;
    else
        m_currentSlice = value;

    m_firstSlabSlice = m_currentSlice;
    m_lastSlabSlice = m_firstSlabSlice + m_slabThickness;
}

double *Q2DViewer::pointInModel( int screen_x, int screen_y )
{
    double *bounds = m_imageActor->GetBounds();
    double position[4];
    computeDisplayToWorld( getRenderer(), screen_x, screen_y, 0, position );
    double *lastPointInModel = new double[3];

    lastPointInModel[0] = position[0];
    lastPointInModel[1] = position[1];
    lastPointInModel[2] = position[2];
    //Cas axial
    switch( m_lastView )
    {
        case Axial:

            if( bounds[0] < position[0] && bounds[1] > position[0] ) // La x cau dins del model
            {
                lastPointInModel[0] = position[0];
            }
            else {// La x cau fora del model
                if( bounds[0] > position[0] )
                    lastPointInModel[0] = bounds[0];
                else lastPointInModel[0] = bounds[1];
            }

            if( bounds[2] < position[1] && bounds[3] > position[1] ) // La y cau dins del model
            {
                lastPointInModel[1] = position[1];
            }
            else{
                if( bounds[2] > position[1] )
                    lastPointInModel[1] = bounds[2];
                else lastPointInModel[1] = bounds[3];
            }
            break;

        case Coronal:

            if( bounds[0] < position[0] && bounds[1] > position[0] )
            {
                lastPointInModel[0] = position[0];
            }
            else{
                if( bounds[0] > position[0] )
                    lastPointInModel[0] = bounds[0];
                else lastPointInModel[0] = bounds[1];
            }

            if( bounds[4] < position[2] && bounds[5] > position[2] )
            {
                lastPointInModel[2] = position[2];
            }
            else{
                if( bounds[4] > position[2] )
                    lastPointInModel[2] = bounds[4];
                else lastPointInModel[2] = bounds[5];
            }
            break;

        case Sagital:

            if( bounds[4] < position[2] && bounds[5] > position[2] )
            {
                lastPointInModel[2] = position[2];
            }
            else{
                if( bounds[4] > position[2] )
                    lastPointInModel[2] = bounds[4];
                else lastPointInModel[2] = bounds[5];
            }

            if( bounds[2] < position[1] && bounds[3] > position[1] )
            {
                lastPointInModel[1] = position[1];
            }
            else{
                if( bounds[2] > position[1] )
                    lastPointInModel[1] = bounds[2];
                else lastPointInModel[1] = bounds[3];
            }
            break;
    }
    return lastPointInModel;
}

vtkImageData* Q2DViewer::getCurrentSlabProjection()
{
    return m_thickSlabProjectionFilter->GetOutput();
}

int Q2DViewer::getNearestSlice( double projectedPosition[3], double &distance )
{
    int i;
    double actualDistance;
    double minimumDistance = -1.0;
    int minimumSlice = -1;
    double currentPlaneOrigin[3], currentNormalVector[3];
    ImagePlane *currentPlane = 0;
    int maxSlice = this->getMaximumSlice();

    for( i = 0; i < maxSlice ; i++ )
    {
        currentPlane = this->getImagePlane( i, m_currentPhase );

        if( currentPlane )
        {
            currentPlane->getOrigin( currentPlaneOrigin );
            currentPlane->getNormalVector( currentNormalVector );

            actualDistance = vtkPlane::DistanceToPlane ( projectedPosition, currentNormalVector, currentPlaneOrigin );

            if( ( actualDistance < minimumDistance ) || ( minimumDistance == -1.0 ))
            {
                minimumDistance = actualDistance;
                minimumSlice = i;
            }
        }
    }
    distance = minimumDistance;

    return minimumSlice;
}

};  // end namespace udg

