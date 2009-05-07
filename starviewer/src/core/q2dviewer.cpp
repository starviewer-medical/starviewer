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
#include "dicomtagreader.h" // per les annotacions de mamo
// TODO això estarà temporalment pel tema de penjar correctament les imatges de mamo
#include "hangingprotocolmanager.h"
//thickslab
#include "vtkProjectionImageFilter.h"

// include's qt
#include <QResizeEvent>

// include's bàsics vtk
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>
#include <vtkMath.h> // per ::Round()
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
#include <vtkWindowLevelLookupTable.h>
// voxel information
#include <vtkPointData.h>
#include <vtkCell.h>
#include <vtkImageActor.h>
// displayed area
#include <vtkImageChangeInformation.h>
#include <vtkImageResample.h>
// grayscale pipeline
#include <vtkImageMapToWindowLevelColors.h>
// projeccio de punts
#include <vtkMatrix4x4.h>
#include <vtkPlane.h>

namespace udg {

Q2DViewer::Q2DViewer( QWidget *parent )
: QViewer( parent ), m_lastView(Q2DViewer::Axial), m_currentSlice(0), m_currentPhase(0), m_overlayVolume(0), m_blender(0), m_cornerAnnotations(0), m_enabledAnnotations(Q2DViewer::AllAnnotation), m_overlay( Q2DViewer::CheckerBoard ), m_sideRuler(0), m_bottomRuler(0), m_scalarBar(0), m_rotateFactor(0), m_numberOfPhases(1), m_maxSliceValue(0), m_applyFlip(false), m_isImageFlipped(false), m_slabThickness(1), m_firstSlabSlice(0), m_lastSlabSlice(0), m_thickSlabActive(false), m_slabProjectionMode( AccumulatorFactory::Maximum )
{
    // CheckerBoard
    // el nombre de divisions per defecte, serà de 2, per simplificar
    m_divisions[0] = m_divisions[1] = m_divisions[2] = 2;
    m_imageSizeInformation[0] = 0;
    m_imageSizeInformation[1] = 0;

    // filtre de thick slab + grayscale
    m_thickSlabProjectionFilter = vtkProjectionImageFilter::New();
    m_windowLevelLUTMapper = vtkImageMapToWindowLevelColors::New();

    // creem anotacions i actors
    createAnnotations();
    m_imageActor = vtkImageActor::New();
    addActors();

    //creem el drawer, passant-li com a visor l'objecte this
    m_drawer = new Drawer( this );
    connect( this, SIGNAL(cameraChanged()), SLOT(updateRulers()) );

    // TODO això estarà temporalment pel tema de penjar correctament les imatges de mamo
    m_hangingProtocolManager = new HangingProtocolManager(this);

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
    m_imageActor->Delete();
    m_anchoredRulerCoordinates->Delete();
    m_windowLevelLUTMapper->Delete();
    m_thickSlabProjectionFilter->Delete();
    // Fem delete d'altres objectes vtk en cas que s'hagin hagut de crear
    if( m_blender )
        m_blender->Delete();
    // TODO hem hagut de fer eliminar primer el drawer per davant d'altres objectes
    // per solucionar el ticket #539, però això denota que hi ha algun problema de
    // disseny que fa que no sigui prou robust. L'ordre en que s'esborren els objectes
    // no ens hauria d'afectar
    // HACK imposem que s'esborri primer el drawer
    delete m_drawer;
}

void Q2DViewer::createAnnotations()
{
    // contenidor d'anotacions
    m_cornerAnnotations = vtkCornerAnnotation::New();
    m_cornerAnnotations->GetTextProperty()->SetFontFamilyToArial();
    m_cornerAnnotations->GetTextProperty()->ShadowOn();
    m_cornerAnnotations->GetTextProperty()->SetShadow(1);

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
        m_patientOrientationTextActor[i]->GetTextProperty()->ShadowOn();
        m_patientOrientationTextActor[i]->GetTextProperty()->SetShadow(1);

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
    m_patientOrientationTextActor[3]->GetTextProperty()->SetVerticalJustificationToTop();
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

void Q2DViewer::rotateClockWise( int times )
{
    // almenys ha de ser 1 ( +90º )
    if( times <= 0 )
        return;

    if( m_isImageFlipped )
    {
        m_rotateFactor = (m_rotateFactor-times) % 4 ;
    }
    else
    {
        m_rotateFactor = (m_rotateFactor+times) % 4 ;
    }
    updateCamera();
}

void Q2DViewer::rotateCounterClockWise( int times )
{
    // almenys ha de ser 1 ( +90º )
    if( times <= 0 )
        return;

    times += 3;
    if( m_isImageFlipped )
    {
        m_rotateFactor = (m_rotateFactor-times) % 4 ;
    }
    else
    {
        m_rotateFactor = (m_rotateFactor+times) % 4 ;
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

QVector<QString> Q2DViewer::getCurrentDisplayedImageOrientationLabels() const
{
    int index = (m_lastView == Axial) ? m_currentSlice : 0;
    // això es fa per si tenim un mhd que realment només té un arxiu (imatge) però té més llesques
    // TODO caldria millorar l'accés a les imatges a partir del volum, per no haver de fer aquestes filigranes
    // és a dir, al preguntar a Volume, getImage(index) ell ens retorna la imatge que toca i ja comprova rangs si cal
    // i no ens retorna la llista d'imatges a saco
    index = ( index >= m_mainVolume->getImages().size() ) ? 0 : index;
    QString orientation = m_mainVolume->getImages().at(index)->getPatientOrientation();
    // tenim les orientacions originals de la imatge en una llista
    QStringList list = orientation.split("\\");

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

    QVector<QString> labelsVector(4);
    // ara caldrà posar, en funció de les rotacions, flips i vista, les etiquetes en l'ordre adequat
    if( ok )
    {
        int index = 4+m_rotateFactor;
        // 0:Esquerra, 1:A dalt, 2:Dreta, 3:Abaix
        if( m_lastView == Axial )
        {
            labelsVector[ (0 + index) % 4 ] = this->getOppositeOrientationLabel( list.at(0) ); // esquerra
            labelsVector[ (2 + index) % 4 ] = list.at(0); // dreta
            labelsVector[ (1 + index) % 4 ] = this->getOppositeOrientationLabel( list.at(1) ); // a dalt
            labelsVector[ (3 + index) % 4 ] = list.at(1); // a baix
        }
        else if( m_lastView == Sagital )
        {
            // HACK FLIP de moment necessitem fer aquest truc. Durant el refactoring caldria
            // veure si es pot fer d'una manera millor
            if( m_isImageFlipped )
                index -= 2;

            labelsVector[ (0 + index) % 4 ] =  this->getOppositeOrientationLabel( list.at(1) ); // esquerra
            labelsVector[ (2 + index) % 4 ] =  list.at(1); // dreta
            labelsVector[ (1 + index) % 4 ] =  list.at(2); // a dalt
            labelsVector[ (3 + index) % 4 ] =  this->getOppositeOrientationLabel( list.at(2) ); // a baix
        }
        else if( m_lastView == Coronal )
        {
            // HACK FLIP de moment necessitem fer aquest truc. Durant el refactoring caldria
            // veure si es pot fer d'una manera millor
            if( m_isImageFlipped )
                index -= 2;

            labelsVector[ (0 + index) % 4 ] = this->getOppositeOrientationLabel( list.at(0) ); // esquerra
            labelsVector[ (2 + index) % 4 ] = list.at(0); // dreta
            labelsVector[ (1 + index) % 4 ] = list.at(2); // a dalt
            labelsVector[ (3 + index) % 4 ] = this->getOppositeOrientationLabel( list.at(2) ); // a baix
        }
        if( m_isImageFlipped )
        {
            qSwap( labelsVector[0], labelsVector[2] );
        }
    }
    else
    {
        DEBUG_LOG("L'orientació del pacient conté un nombre incorrecte d'elements:[" + QString::number(list.size()) + "]. No s'aplicaran annotacions de referència sobre la imatge");
    }
    return labelsVector;
}

QString Q2DViewer::getCurrentPlaneProjectionLabel() const
{
    QVector<QString> labels = this->getCurrentDisplayedImageOrientationLabels();
    return Image::getProjectionLabelFromPlaneOrientation( labels[0]+"\\"+labels[1] );
}

void Q2DViewer::mapOrientationStringToAnnotation()
{
    // obtenim els labels que estem veient en aquest moment
    QVector<QString> labels = this->getCurrentDisplayedImageOrientationLabels();

    // text actor -> 0:Esquerra, 1:Abaix , 2:Dreta, 3:A dalt
    // labels     -> 0:Esquerra, 1:A dalt, 2:Dreta, 3:Abaix
    m_patientOrientationTextActor[0]->SetInput( qPrintable( labels[0] ) );
    m_patientOrientationTextActor[1]->SetInput( qPrintable( labels[3] ) );
    m_patientOrientationTextActor[2]->SetInput( qPrintable( labels[2] ) );
    m_patientOrientationTextActor[3]->SetInput( qPrintable( labels[1] ) );
}

void Q2DViewer::refreshAnnotations()
{
    if( !m_mainVolume )
        return;

    if( m_enabledAnnotations & Q2DViewer::PatientInformationAnnotation )
	{
		m_cornerAnnotations->SetText( 3, qPrintable( m_upperRightText ) );
        m_cornerAnnotations->SetText( 1, qPrintable( m_lowerRightText.trimmed() ) );
	}
    else
	{
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

    vtkRenderer *renderer = getRenderer();
    Q_ASSERT( renderer );
    // anotacions de texte
    renderer->AddViewProp( m_cornerAnnotations );
    renderer->AddViewProp( m_patientOrientationTextActor[0] );
    renderer->AddViewProp( m_patientOrientationTextActor[1] );
    renderer->AddViewProp( m_patientOrientationTextActor[2] );
    renderer->AddViewProp( m_patientOrientationTextActor[3] );
    renderer->AddViewProp( m_sideRuler );
    renderer->AddViewProp( m_bottomRuler );
    renderer->AddViewProp( m_scalarBar );
    renderer->AddViewProp( m_imageActor );
    // TODO colocar això en un lloc mes adient
    renderer->GetActiveCamera()->ParallelProjectionOn();
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

void Q2DViewer::setInput( Volume *volume )
{
    if( !volume )
        return;

    //al fer un nou input, les distàncies que guardava el drawer no tenen sentit, pertant s'esborren
    if( m_mainVolume )
        m_drawer->removeAllPrimitives();

    // HACK
    // Desactivem el refresh per tal de millorar l'eficiencia del setInput ja que altrament es renderitza multiples vegades
    this->m_isRefreshActive = false;

    // TODO caldria fer netejar? bloquejar? per tal que quedi en negre mentres es carrega el nou volum?
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

    // aquí corretgim el fet que no s'hagi adquirit la imatge en un espai ortogonal
    //No s'aplica perquè afectaria al cursor3D entre d'altres
//     ImagePlane * currentPlane = new ImagePlane();
//     currentPlane->fillFromImage( m_mainVolume->getImage(0,0) );
//     double currentPlaneRowVector[3], currentPlaneColumnVector[3];
//     currentPlane->getRowDirectionVector( currentPlaneRowVector );
//     currentPlane->getColumnDirectionVector( currentPlaneColumnVector );
//     vtkMatrix4x4 *projectionMatrix = vtkMatrix4x4::New();
//     projectionMatrix->Identity();
//     int row;
//     for( row = 0; row < 3; row++ )
//     {
//         projectionMatrix->SetElement(row,0, (currentPlaneRowVector[ row ]));
//         projectionMatrix->SetElement(row,1, (currentPlaneColumnVector[ row ]));
//     }
//
//     m_imageActor->SetUserMatrix(projectionMatrix);
//     delete currentPlane;


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
	// obtenim valors de gris i aquestes coses
    // aquí es crea tot el pipeline del visualitzador
    this->applyGrayscalePipeline();

    // Preparem el thickSlab // TODO cada cop que fem setInput resetejem els valors per defecte??
    m_thickSlabProjectionFilter->SetInput( m_mainVolume->getVtkData() );
    m_thickSlabProjectionFilter->SetProjectionDimension( m_lastView );
    m_thickSlabProjectionFilter->SetAccumulatorType( (AccumulatorFactory::AccumulatorType) m_slabProjectionMode );
    m_thickSlabProjectionFilter->SetFirstSlice( m_firstSlabSlice * m_numberOfPhases + m_currentPhase );
    m_thickSlabProjectionFilter->SetNumberOfSlicesToProject( m_slabThickness );
    m_thickSlabProjectionFilter->SetStep( m_numberOfPhases );

	updateDisplayExtent(); // TODO BUG sino fem aquesta crida ens peta al canviar d'input entre un que fos més gran que l'anterior
    resetViewToAxial();

    updatePatientAnnotationInformation();
    this->enableAnnotation( m_enabledAnnotations );

    // actualitzem la informació de window level
    this->updateWindowLevelData();
    // HACK
    // S'activa el refresh per tal de que es renderitzi el visualitzador
    this->m_isRefreshActive = true;
}

void Q2DViewer::setOverlayInput( Volume *volume )
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
        emit overlayChanged();

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

void Q2DViewer::isOverlayModified( )
{
    switch( m_overlay )
    {
    case None:
    break;

    case CheckerBoard:
    break;

    case Blend:
        // \TODO Revisar la manera de donar-li l'input d'un blending al visualitzador
        m_blender->Modified();
        m_windowLevelLUTMapper->SetInputConnection( m_blender->GetOutputPort() );
        emit overlayChanged();
    break;

    case RectilinearWipe:
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
        getRenderer()->Render();
    }
    else
    {
        DEBUG_LOG( "::render() : No hi ha cap volum per visualitzar" );
    }
}

void Q2DViewer::resetView( CameraOrientationType view )
{
    m_lastView = view;
    m_alignPosition = Q2DViewer::AlignCenter;
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

        vtkCamera *camera = getRenderer()->GetActiveCamera();
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
            getRenderer()->ResetCamera();

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
            getRenderer()->ResetCamera();

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
            getRenderer()->ResetCamera();

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
			m_thickSlabProjectionFilter->SetFirstSlice( m_firstSlabSlice * m_numberOfPhases + m_currentPhase );
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
        if( isThickSlabActive() )
        {
            m_thickSlabProjectionFilter->SetFirstSlice( m_firstSlabSlice * m_numberOfPhases + m_currentPhase );
        }
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
        double bounds[6];
        // ajustem la imatge al viewport
        m_imageActor->GetBounds( bounds );
        switch( m_lastView )
        {
            case Axial:
                scaleToFit3D( bounds[1], bounds[3], 0.0, bounds[0], bounds[2], 0.0 );
            break;

            case Sagital:
                scaleToFit3D( 0.0, bounds[2], bounds[5], 0.0, bounds[3], bounds[4] );
            break;
            case Coronal:
                scaleToFit3D( bounds[1], 0.0, bounds[4], bounds[0], 0.0, bounds[5] );
            break;
        }
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
    // TODO cal refactoritzar aquest mètode i substituir-lo per aquest que és el seu equivalent
    getEventWorldCoordinate(xyz);
    return true;
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
    emit seedPositionChanged(pos[0], pos[1], pos[2]);
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
                .arg( vtkMath::Round( m_windowLevelLUTMapper->GetWindow() ) )
                .arg( vtkMath::Round( m_windowLevelLUTMapper->GetLevel() ) );
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
		Patient *patient = m_mainVolume->getPatient();
		Study *study = m_mainVolume->getStudy();
		Series *series = m_mainVolume->getSeries();
        // informació fixa
        m_upperRightText = tr("%1\n%2\n%3 %4 %5\nAcc:%6\n%7\n%8")
                    .arg( series->getInstitutionName() )
					.arg( patient->getFullName() )
					.arg( study->getPatientAge() )
                    .arg( patient->getSex() )
                    .arg( patient->getID() )
                    .arg( study->getAccessionNumber() )
                    .arg( study->getDateAsString() )
                    .arg( study->getTimeAsString() ); // TODO seria més correcte mostrar l'hora de la sèrie i inclús de la imatge

		if( series->getModality() == "MG" )
		{
			m_lowerRightText.clear();
		}
		else
		{
			// Si protocol i descripció coincideixen posarem el contingut de protocol
			// Si són diferents, els fusionarem
			QString protocolName, description;
			protocolName = m_mainVolume->getSeries()->getProtocolName();
			description = m_mainVolume->getSeries()->getDescription();
			m_lowerRightText = protocolName;
			if( description != protocolName )
				m_lowerRightText += "\n" + description;
		}

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
    Q_ASSERT( m_mainVolume );

    if( m_mainVolume->getSeries()->getModality() == "MG" )
    {
        m_enabledAnnotations =  m_enabledAnnotations & ~Q2DViewer::SliceAnnotation;
        Image *image = getCurrentDisplayedImage();
        if( image )
        {
            DICOMTagReader reader( image->getPath() );
            QString laterality = reader.getAttributeByName( DCM_ImageLaterality );
            QString desiredOrientation;


            QStringList tagValue = reader.getSequenceAttributeByName( DCM_ViewCodeSequence, DCM_CodeMeaning );
            if( ! tagValue.isEmpty() )
            {
                QString projection = tagValue.at(0);
                /// PS 3.16 - 2008, Page 408, Context ID 4014, View for mammography
                // TODO tenir-ho carregat en arxius, maps, etc..
                // TODO fer servir millor els codis [Code Value (0008,0100)] en compte dels "code meanings" podria resultar més segur
                if( projection == "medio-lateral" )
                    projection = "ML";
                else if( projection == "medio-lateral oblique" )
                    projection = "MLO";
                else if( projection == "latero-medial" )
                    projection = "LM";
                else if( projection == "latero-medial oblique" )
                    projection = "LMO";
                else if( projection == "cranio-caudal" )
                    projection = "CC";
                else if( projection == "caudo-cranial (from below)" )
                    projection = "FB";
                else if( projection == "superolateral to inferomedial oblique" )
                    projection = "SIO";
                else if( projection == "exaggerated cranio-caudal" )
                    projection = "XCC";
                else if( projection == "cranio-caudal exaggerated laterally" )
                    projection = "XCCL";
                else if( projection == "cranio-caudal exaggerated medially" )
                    projection = "XCCM";

                // S'han de seguir les recomanacions IHE de presentació d'imatges de Mammografia
                // IHE Techincal Framework Vol. 2 revision 8.0, apartat 4.16.4.2.2.1.1.2 Image Orientation and Justification
                if( projection == "CC" || projection == "XCC" || projection == "XCCL" || projection == "XCCM" || projection == "FB" )
                {
                    if( laterality == "L" )
                        desiredOrientation = "A\\R";
                    else if( laterality == "R" )
                        desiredOrientation = "P\\L";
                }
                else if( projection == "MLO" || projection == "ML" || projection == "LM" || projection == "LMO" || projection == "SIO" )
                {
                    if( laterality == "L" )
                        desiredOrientation = "A\\F";
                    else if( laterality == "R" )
                        desiredOrientation = "P\\F";
                }
                else
                {
                    DEBUG_LOG("Projecció no tractada! :: " + projection );
                }

                m_lowerRightText = laterality + " " + projection;
                // TODO això estarà temporalment pel tema de penjar correctament les imatges de mamo
                QVector<QString> labels = getCurrentDisplayedImageOrientationLabels();
                m_hangingProtocolManager->applyDesiredDisplayOrientation( labels[2]+"\\"+labels[3] , desiredOrientation, this);
            }
        }
        else
            m_lowerRightText.clear();

        m_cornerAnnotations->SetText( 1, qPrintable( m_lowerRightText.trimmed() ) );
    }

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
		// TODO ara només tenim en compte de posar l'slice location si estem en la vista "original"
		if( m_lastView == Q2DViewer::Axial )
		{
			Image *image = getCurrentDisplayedImage();
			if( image )
			{
				QString location = image->getSliceLocation();
				if( !location.isEmpty() )
				{
					lowerLeftText = tr("Loc: %1").arg( location.toDouble(), 0, 'f', 2 );
					if( isThickSlabActive() )
					{
						Image *secondImage = m_mainVolume->getSeries()->getImageByIndex( ((m_currentSlice + m_slabThickness-1) * m_numberOfPhases) + m_currentPhase );
						if( secondImage )
						{
							lowerLeftText += tr("-%1").arg( secondImage->getSliceLocation().toDouble(), 0, 'f', 2 );
						}
					}
					lowerLeftText += "\n";
				}
			}
		}

        if( maxPhase > 1 ) // tenim fases
        {
            if( m_slabThickness > 1 )
            {
                lowerLeftText += tr("Slice: %1-%2/%3 Phase: %4/%5")
                        .arg( currentSlice )
                        .arg( currentSlice+m_slabThickness-1 )
                        .arg( maxSlice )
                        .arg( currentPhase )
                        .arg( maxPhase );
            }
            else
            {
                lowerLeftText += tr("Slice: %1/%2 Phase: %3/%4")
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
                lowerLeftText += tr("Slice: %1-%2/%3")
                        .arg( currentSlice )
                        .arg( currentSlice+m_slabThickness-1 )
                        .arg( maxSlice );
            }
            else
            {
                lowerLeftText += tr("Slice: %1/%2")
                        .arg( currentSlice )
                        .arg( maxSlice );
            }
        }
        //afegim el thickness de la llesca nomes si es > 0mm
        if ( this->getThickness() > 0.0 )
            lowerLeftText += tr(" Thickness: %1 mm").arg( this->getThickness(), 0, 'f', 2 );

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

	//TODO potser el càlcul de l'índex de l'imatge l'hauria de fer Volume que
	// és qui coneix com es guarda la informació de la imatge, ja que si canviem la manera
	// de guardar les phases, això ja no ens valdria
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
    getRenderer()->ResetCameraClippingRange();
}

void Q2DViewer::enableAnnotation( AnnotationFlags annotation, bool enable )
{
    if( enable )
        m_enabledAnnotations = m_enabledAnnotations | annotation;
    else
        m_enabledAnnotations =  m_enabledAnnotations & ~annotation;

    refreshAnnotations();
    this->refresh();
}

void Q2DViewer::removeAnnotation( AnnotationFlags annotation )
{
    enableAnnotation( annotation, false );
}

void Q2DViewer::applyGrayscalePipeline()
{
    DEBUG_LOG( "*** Grayscale Transform Pipeline Begin ***" );
    DEBUG_LOG( QString("Image Information: Bits Allocated: %1, Bits Stored: %2, Pixel Range %3 to %4, SIGNED?Pixel Representation: %5, Photometric interpretation: %6")
    .arg( m_mainVolume->getImages().at(0)->getBitsAllocated() )
    .arg( m_mainVolume->getImages().at(0)->getBitsStored() )
    .arg( m_mainVolume->getVtkData()->GetScalarRange()[0] )
    .arg( m_mainVolume->getVtkData()->GetScalarRange()[1] )
    .arg( m_mainVolume->getImages().at(0)->getPixelRepresentation() )
    .arg( m_mainVolume->getImages().at(0)->getPhotometricInterpretation() )
                     );
    // Fins que no implementem Presentation states aquest serà el cas que sempre s'executarà el 100% dels casos
    if( isThickSlabActive() )
    {
        DEBUG_LOG("Grayscale pipeline: Source Data -> ThickSlab -> [Window Level] -> Output ");
        m_windowLevelLUTMapper->SetInput( m_thickSlabProjectionFilter->GetOutput() );
    }
    else
    {
        DEBUG_LOG("Grayscale pipeline: Source Data -> [Window Level] -> Output ");
        m_windowLevelLUTMapper->SetInput( m_mainVolume->getVtkData() );
    }

    m_imageActor->SetInput( m_windowLevelLUTMapper->GetOutput() );
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
        DEBUG_LOG("Desactivem thick Slab i resetejem pipeline normal");
        m_thickSlabActive = false;
        // resetejem el pipeline
        applyGrayscalePipeline();
        updateDisplayExtent();
        updateSliceAnnotationInformation();
        this->refresh();
    }
    if ( m_slabThickness > 1 && !isThickSlabActive() ) // la comprovacio es per constuir el pipeline nomes el primer cop
    {
        DEBUG_LOG("Activem thick Slab i resetejem pipeline amb thickSlab");
        m_thickSlabActive = true;
        // resetejem el pipeline
        applyGrayscalePipeline();
    }

    m_lastSlabSlice = m_currentSlice + m_slabThickness - 1;

    if( isThickSlabActive() )
    {
        m_thickSlabProjectionFilter->SetFirstSlice( m_firstSlabSlice * m_numberOfPhases + m_currentPhase );
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
    if( newSlabThickness == 1 )
    {
        m_slabThickness = 1;
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
    computeDisplayToWorld( screen_x, screen_y, 0, position );
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

vtkImageData *Q2DViewer::getCurrentSlabProjection()
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


void Q2DViewer::restore()
{
    if( ! m_mainVolume )
        return;

    //S'esborren les anotacions
    if( m_mainVolume )
        m_drawer->removeAllPrimitives();

    // HACK
    // Desactivem el refresh per tal de millorar l'eficiencia del setInput ja que altrament es renderitza multiples vegades
    this->m_isRefreshActive = false;

    this->applyGrayscalePipeline();
    this->resetView( m_lastView );
    this->resetWindowLevelToDefault();
    this->updateWindowLevelData();

    // Activem el refresh i refresquem
    this->m_isRefreshActive = true;
    this->setAlignPosition( m_alignPosition );

    this->refresh();
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
    window = wl[0]*-1;
    level = wl[1];

    setWindowLevel( window, level );
}

void Q2DViewer::alignLeft()
{
    double viewerLeft[4];
    double bounds[6];
    double motionVector[4];

    computeDisplayToWorld( 0.0, 0.0, 0.0, viewerLeft );
    m_imageActor->GetBounds( bounds );
    motionVector[0]=0.0;
    motionVector[1]=0.0;
    motionVector[2]=0.0;
    motionVector[3]=0.0;

    // Càlcul del desplaçament
    switch( m_lastView )
    {
        case Axial:
            motionVector[0]=bounds[0]-viewerLeft[0];
            break;
        case Sagital:
            motionVector[1]=bounds[2]-viewerLeft[1];
            break;
        case Coronal:
            motionVector[0]=bounds[0]-viewerLeft[0];
            break;
    }

    pan( motionVector );

    // Canviem els rulers de posició
    m_anchoredRulerCoordinates->SetValue( 0.95 , -0.9 , -0.95 );
    m_alignPosition = Q2DViewer::AlignLeft;
}

void Q2DViewer::alignRight()
{
    int * size;
    double viewerRight[4];
    double bounds[6];
    double motionVector[4];

    size = this->getRenderer()->GetSize();
    computeDisplayToWorld( (double)size[0], 0.0, 0.0, viewerRight );
    m_imageActor->GetBounds( bounds );
    motionVector[0]=0.0;
    motionVector[1]=0.0;
    motionVector[2]=0.0;
    motionVector[3]=0.0;

    // Càlcul del desplaçament
    switch( m_lastView )
    {
        case Axial:
            motionVector[0] = bounds[1]-viewerRight[0];
            break;
        case Sagital:
            motionVector[1] = bounds[3]-viewerRight[1];
            break;
        case Coronal:
            motionVector[0] = bounds[1]-viewerRight[0];
            break;
    }

    pan( motionVector );
    m_alignPosition = Q2DViewer::AlignRight;

}

void Q2DViewer::setAlignPosition( AlignPosition alignPosition )
{
    switch( alignPosition )
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

};  // end namespace udg

