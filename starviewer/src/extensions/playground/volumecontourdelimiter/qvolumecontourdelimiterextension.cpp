#include "qvolumecontourdelimiterextension.h"
#include "toolsactionfactory.h"
#include "volume.h"
#include "deletedirectory.h"
#include "point.h"
#include "drawerpolyline.h"
// VTK
#include <vtkRenderWindowInteractor.h>
#include <vtkImageThreshold.h>
#include <vtkCommand.h>
#include <vtkImageChangeInformation.h>
#include <vtkMetaImageWriter.h>
#include <vtkPNGWriter.h>
#include <vtkPNGReader.h>
#include <vtkImageStencil.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkImageShiftScale.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkLinearExtrusionFilter.h>
/////
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>

namespace udg {

class vtkSWCallback : public vtkCommand
{
    public:
        static vtkSWCallback *New()
        { return new vtkSWCallback; }
        virtual void Execute(vtkObject *caller, unsigned long, void*)
        {
            vtkSplineWidget *spline = reinterpret_cast<vtkSplineWidget*>(caller);
            spline->GetPolyData(Poly);
        }
        vtkSWCallback():Poly(0){};
        vtkPolyData* Poly;
};

QVolumeContourDelimiterExtension::QVolumeContourDelimiterExtension(QWidget *parent)
 : QWidget(parent)
{
    setupUi( this );
    m_volume = 0;

    m_tempDirectory="./";

    QString s = m_tempDirectory;
    s += "dadesTemporalsStencil";

    QDir dir( s );

    if ( !dir.exists() )
    {
        QDir directory( m_tempDirectory );
        directory.mkdir( "dadesTemporalsStencil" );
    }

    m_totalVolume = 0.;

    m_splineClosed = false;
    m_firstSplineIsFixed = false;
    m_points = vtkPoints::New();
    m_extrude = vtkLinearExtrusionFilter::New();
    m_addSpline->setEnabled( false );
    m_createMask->setEnabled( false );
    createActions();
}

QVolumeContourDelimiterExtension::~QVolumeContourDelimiterExtension()
{
    //esborrem el directori temporal de proves i el seu contigut
    DeleteDirectory deleteDirectory;
    deleteDirectory.deleteDirectory( ( m_tempDirectory + "dadesTemporalsStencil/" ) , true );

    m_points->Delete();
    m_splineWidget->Delete();
    m_extrude->Delete();
}

void QVolumeContourDelimiterExtension::setInput( Volume* volume )
{
    m_volume = volume;

    //Fem una transformació sobre el volum d'entrada: passem el tipus de dades de float a char per poder crear una imatge PNG que ens servirà per fer la màscara.
    vtkImageShiftScale* shifter = vtkImageShiftScale::New();
    shifter->SetInput( m_volume->getVtkData() );
    shifter->SetOutputScalarTypeToUnsignedChar();
    shifter->ReleaseDataFlagOff();
    shifter->Update();

    //Pintem tota la imatge de negre, donant un threshold impossible.
    vtkImageThreshold *thresh = vtkImageThreshold::New();
    thresh->SetInputConnection( shifter->GetOutputPort() );
    thresh->ThresholdBetween(200,0);
    thresh->SetInValue(0);
    thresh->SetOutValue(0);
    thresh->ReleaseDataFlagOff();
    thresh->Update();

    //guardem la imatge en format PNG
    vtkPNGWriter *writer = vtkPNGWriter::New();
    writer->SetInputConnection( thresh->GetOutputPort() );

    writer->SetFileName( qPrintable( m_tempDirectory + "dadesTemporalsStencil/" + "blackImage.png" ) );

    //Per solventar problemes a sobre la màscara resultant, cal fer un canvi de la informació,
    //que es correspon amb fer un canvi d'origen al volum de treball, altrament la màscara
    //surt desplaçada, pertant deformada.
    vtkImageChangeInformation *information = vtkImageChangeInformation::New();
    information->SetInput ( m_volume->getVtkData() );
    information->SetOutputOrigin ( 0,0,0 );
    information->Update();
    m_volume->setData( information->GetOutput() );
    m_2DView->setInput( m_volume );

    //Determinem les propietats dels dos visors
    m_2DView->updateAnnotationsInformation(Q2DViewer::NoAnnotation);

    m_view = m_2DView->getView();

    //com a mínim necessitem un spline, pertant el creem i l'afegim a la llista
    m_splineWidget = vtkSplineWidget::New();
    setSplineAtributes( m_splineWidget );
    setAttributes();
    m_2DView->refresh();

    writer->Write();
    thresh->Delete();
    shifter->Delete();
    writer->Delete();
    information->Delete();
}

void QVolumeContourDelimiterExtension::createActions()
{
    m_toolsActionGroup = new QActionGroup( 0 );
    m_toolsActionGroup->setExclusive( true );

    m_actionFactory = new ToolsActionFactory( 0 );

    m_splineAction = new QAction( 0 );
    m_setSpline->setDefaultAction( m_splineAction );
    m_splineAction->setCheckable( true );
    m_splineAction->setIcon( QIcon( ":/images/polyline.png" ) );

    m_windowLevelAction = m_actionFactory->getActionFrom( "WindowLevelTool" );
    m_windowLevelToolButton->setDefaultAction( m_windowLevelAction );

    m_zoomAction = m_actionFactory->getActionFrom( "ZoomTool" );
    m_zoomToolButton->setDefaultAction( m_zoomAction );

    m_slicingAction = m_actionFactory->getActionFrom( "SlicingTool" );
    m_slicingToolButton->setDefaultAction( m_slicingAction );

    m_toolsActionGroup = new QActionGroup( 0 );
    m_toolsActionGroup->setExclusive( true );
    m_toolsActionGroup->addAction( m_zoomAction );
    m_toolsActionGroup->addAction( m_windowLevelAction );
    m_toolsActionGroup->addAction( m_splineAction );
    m_toolsActionGroup->addAction( m_slicingAction );

    connect( m_createMask, SIGNAL( clicked() )  , this, SLOT( createModelOfVoxelsWithObtainedMasks() ) );
    connect( m_setSpline, SIGNAL( clicked ( bool ) ), this, SLOT( buttonAddSplineEnabled( bool ) ) );
    connect( m_addSpline, SIGNAL( clicked() ), this, SLOT( addNewSpline() ) );
    connect( m_splineAction, SIGNAL( triggered() ), m_2DView, SLOT( disableTools() ) );
    connect( m_zoomAction, SIGNAL( triggered() ), m_2DView, SLOT( enableTools() ) );
    connect( m_windowLevelAction, SIGNAL( triggered() ), m_2DView, SLOT( enableTools() ) );
    connect( m_slicingAction, SIGNAL( triggered() ), m_2DView, SLOT( enableTools() ) );
    connect( m_2DView, SIGNAL( sliceChanged( int ) ), this, SLOT( saveCurrentSplineAndGetNeededSplines( int ) ) );
    connect( m_2DView, SIGNAL( sliceChanged( int ) ), m_spinBox, SLOT( setValue( int ) ) );
    connect( m_2DView, SIGNAL( sliceChanged( int ) ), this, SLOT( computeCurrentArea( int ) ) );
    connect( m_2DView, SIGNAL( sliceChanged( int ) ), this, SLOT( computeTotalVolume() ) );
    connect( m_refreshButton, SIGNAL( clicked() ), this, SLOT( computeAreaAndVolume() ) );
    connect( m_spinBox, SIGNAL( valueChanged(int) ), this, SLOT( saveCurrentSplineAndGetNeededSplines( int ) ) );
    connect( m_spinBox, SIGNAL( valueChanged(int) ), m_2DView, SLOT( setSlice( int ) ) );
    connect( m_2DView, SIGNAL( eventReceived( unsigned long ) ), this, SLOT( myEventHandler( unsigned long )) );
    connect( m_actionFactory, SIGNAL( triggeredTool( QString ) )  , m_2DView, SLOT( setTool( QString ) ) );

    m_2DView->removeAnnotation( Q2DViewer::AllAnnotation );
    m_2DView->enableAnnotation( Q2DViewer::SliceAnnotation );
}

void QVolumeContourDelimiterExtension::setSplineAtributes( vtkSplineWidget *spline )
{
    switch ( m_view )
    {
        case Q2DViewer::Axial:
            spline->SetProjectionNormalToZAxes();
            break;

        case Q2DViewer::Coronal:
            spline->SetProjectionNormalToYAxes();
            break;

        case Q2DViewer::Sagital:
            spline->SetProjectionNormalToXAxes();
            break;

        default:
            break;
    }
    spline->GetHandleProperty()->SetPointSize( 0.1 );
    spline->GetHandleProperty()->SetOpacity ( 0.30 );
    spline->GetLineProperty()->SetColor( 0.93, 0.53, 0.0 );

    spline->SetCurrentRenderer( m_2DView->getRenderer() );
    spline->SetDefaultRenderer( m_2DView->getRenderer() );
    spline->SetInput( m_volume->getVtkData() );
    spline->SetInteractor( m_2DView->getInteractor() );
    spline->On();
}

void QVolumeContourDelimiterExtension::setAttributes()
{
    int extent[6];
    m_volume->getWholeExtent( extent );

    switch ( m_view )
    {
        case Q2DViewer::Axial:
            m_firstSlice = extent[4];
            m_lastSlice = extent[5];
            m_extrude->SetVector( 0, 0, 1 );
            break;

        case Q2DViewer::Coronal:
            m_firstSlice = extent[2];
            m_lastSlice = extent[3];
            m_extrude->SetVector( 0, 1, 0 );
            break;

        case Q2DViewer::Sagital:
            m_firstSlice = extent[0];
            m_lastSlice = extent[1];
            m_extrude->SetVector( 1, 0, 0 );
            break;
    }

    m_extrude->SetScaleFactor( 1 );
    m_extrude->SetExtrusionTypeToNormalExtrusion();
}

void QVolumeContourDelimiterExtension::myEventHandler( unsigned long id )
{
    switch( id )
    {
        case vtkCommand::LeftButtonPressEvent:
            if ( m_setSpline->isChecked() )
            {
                insertPoint();
                if ( m_pointList.size() == 1 )
                    m_sliceOfCurrentSpline = m_2DView->getCurrentSlice();
                else if ( m_pointList.size() > 1 ) //a partir del segon punt que faci aparèixer l'spline
                {
                    showSplines();
                    m_createMask->setEnabled( true );
                }
            }
            m_2DView->render();
            break;

        case vtkCommand::KeyPressEvent:
            if ( m_2DView->getInteractor()->GetKeyCode() == 'c' && m_pointList.size() > 2 )
            {

                m_splineWidget->SetClosed( true );
                m_splineClosed = true;
                m_splinesMap.insert( m_sliceOfCurrentSpline, m_splineWidget );
                showSplines();

                //ja podem afegir un altre spline, perquè hem tancat l'actual
                m_addSpline->setEnabled( true );

                computeCurrentArea( m_2DView->getCurrentSlice() );
                computeTotalVolume();
            }
            break;
    }
}

void QVolumeContourDelimiterExtension::buttonAddSplineEnabled( bool enabled )
{
    if ( m_firstSplineIsFixed )
        m_addSpline->setEnabled( enabled );
}

void QVolumeContourDelimiterExtension::saveCurrentSplineAndGetNeededSplines( int slice )
{
    //fem invisible l'spline actual, si cal, en cas contrari el deixem visible
    if ( m_sliceOfCurrentSpline == slice )
        m_splineWidget->On();
    else
        m_splineWidget->Off();

    //desactivem tots els splines, per després només activar els que estan a la llesca actual
    QMultiMap<int, vtkSplineWidget*>::const_iterator iterator = m_splinesMap.constBegin();
    while (iterator != m_splinesMap.constEnd())
    {
        iterator.value()->Off();
        ++iterator;
    }

    //comptem el nombre d'splines que hi han editats a la llesca actual
    int splinesOnCurrentSlice = m_splinesMap.count( slice );

    if ( splinesOnCurrentSlice > 0 )
    {
        //obtenim la llista amb els splines d'aquesta llesca
        QList<vtkSplineWidget*> currentSplinesList = m_splinesMap.values( slice );

        //activem cadascun dels splines de la llesca.
        for ( int i = 0; i < currentSplinesList.count(); i++ )
            currentSplinesList[i]->On();

        m_2DView->refresh();
    }
}

void QVolumeContourDelimiterExtension::addNewSpline()
{
    //si el primer spline ha sigut fixat, ho notifiquem
    if ( !m_firstSplineIsFixed )
        m_firstSplineIsFixed = true;

    //deixem el botó d'afegir un nou spline desactivat perquè el visor espera que es dibuixi un nou spline
    m_addSpline->setEnabled( false );

    //el nou spline està obert
    m_splineClosed = false;
//     m_splinesMap.insert( m_sliceOfCurrentSpline, m_splineWidget );

    //esborrem tots els punts acumulats
    m_pointList.clear();
    m_points->Reset();

    //creem un de nou
    m_splineWidget = vtkSplineWidget::New();
    setSplineAtributes( m_splineWidget );
}

void QVolumeContourDelimiterExtension::insertPoint()
{
    int xy[2];
    double position[4];

    m_2DView->getInteractor()->GetEventPosition( xy );
    m_2DView->computeDisplayToWorld( m_2DView->getRenderer() , xy[0], xy[1], 0, position );

    Point p( position[0], position[1] );
    m_pointList.append( p );
}

void QVolumeContourDelimiterExtension::showSplines()
{
    double coords[3];
    for ( int i = 0 ; i < m_pointList.size() ; i++  )
    {
        coords[0] = m_pointList[i].getX();
        coords[1] = m_pointList[i].getY();
        coords[2] = 0;
        m_points->InsertPoint( i, coords );
    }
    m_splineWidget->SetNumberOfHandles( m_pointList.size() );
    m_splineWidget->InitializeHandles( m_points );
    m_splineWidget->On();

    m_2DView->refresh();
    m_points->Reset();
}

void QVolumeContourDelimiterExtension::createModelOfVoxelsWithObtainedMasks()
{
    int numOfSplines, *extent;

    if ( !m_splineClosed )
    {
        if ( m_pointList.size() > 2 )
        {
            m_splineWidget->SetClosed( true );
            m_splineClosed = true;
        }
        else
        {
            m_splineWidget->Delete();
        }
    }

    QMessageBox::information( 0, "Information",
            "Resultant volume will be saved in a directory, not in PACS!!\nThe next step is select this directory." );

    QDir saveDir( "./" );
    QString fileName = QFileDialog::getSaveFileName( this, tr("Save Volume as..."), saveDir.home().path(), tr("MHD Files (*.mhd)") );
//     m_splinesMap.insert( m_sliceOfCurrentSpline, m_splineWidget );

    m_currentSlice = m_2DView->getCurrentSlice();

    vtkImageData *m_volumeToImageData;
    vtkImageData *auxiliar = vtkImageData::New();

    vtkPNGReader *reader = vtkPNGReader::New();
    reader->SetFileName( qPrintable( ( m_tempDirectory + "dadesTemporalsStencil/" ) + "blackImage.png" ) );

    //rescalem els nivells de grisos i passem a unsigned char
    vtkImageShiftScale* shifter = vtkImageShiftScale::New();
    shifter->SetScale( 0 );
    shifter->SetOutputScalarTypeToUnsignedChar();
    shifter->SetInput( m_volume->getVtkData() );
    shifter->ReleaseDataFlagOn();
    shifter->Update();

    m_volumeToImageData = shifter->GetOutput();

    //assignem l'spacing adequat, ja que el que dóna per defecte no és correcte
    vtkImageChangeInformation *information = vtkImageChangeInformation::New();
    information->SetInput ( reader->GetOutput() );
    information->SetOutputSpacing( m_volumeToImageData->GetSpacing() );
    information->Update();

    vtkPolyData* splinePoly = vtkPolyData::New();
    vtkPolyDataToImageStencil* dataToStencil = vtkPolyDataToImageStencil::New();
    vtkImageStencil* stencil = vtkImageStencil::New();
    stencil->ReverseStencilOff();
    stencil->SetBackgroundValue( 255 );

    for ( int i = 0; i < m_lastSlice; i++ )
    {
        numOfSplines = m_splinesMap.count( i );
        if ( numOfSplines > 0 )
        {
            for ( int j = 0; j < numOfSplines; j++ )
            {
                m_splineWidget = m_splinesMap.take( i );

                m_splineWidget->GetPolyData( splinePoly );
                m_extrude->SetInput( splinePoly );
                dataToStencil->SetInputConnection( m_extrude->GetOutputPort() );

                if ( j == 0 )
                    stencil->SetInput( information->GetOutput() );
                else
                    stencil->SetInput( auxiliar );

                stencil->SetStencil( dataToStencil->GetOutput() );
                stencil->Update();
                m_stencilOutputDataImage = stencil->GetOutput();
                auxiliar->DeepCopy( stencil->GetOutput() );
                extent = m_stencilOutputDataImage->GetExtent();

                //hem de tenir en compte la vista per saber com col·loquem la llesca modificada
                switch ( m_view )
                {
                    case Q2DViewer::Axial:
                        extent[4] = i;
                        extent[5] = i;
                        break;

                    case Q2DViewer::Coronal:
                        extent[2] = i;
                        extent[3] = i;
                        break;

                    case Q2DViewer::Sagital:
                        extent[0] = i;
                        extent[1] = i;
                        break;

                    default:
                        break;
                }
                m_volumeToImageData->CopyAndCastFrom( m_stencilOutputDataImage, extent );
            }
        }
    }

    if ( !fileName.isEmpty() )
    {
        if ( QFileInfo( fileName ).suffix() != "mhd" )
            fileName += ".mhd";

        vtkMetaImageWriter *miwriter = vtkMetaImageWriter::New();
        miwriter->SetFileName( qPrintable( fileName ) );
        miwriter->SetInput( m_volumeToImageData );
        miwriter->Write();
        miwriter->Delete();
    }

    information->Delete();
    shifter->Delete();
    reader->Delete();
    auxiliar->Delete();
    dataToStencil->Delete();
    stencil->Delete();
    splinePoly->Delete();
}

void QVolumeContourDelimiterExtension::computeCurrentArea( int slice )
{
    double *currentPoint;
    double *aux;
    vtkPolyData *pd  = vtkPolyData::New();
    vtkPoints *points;
    int i,j;
    double currentArea = 0.;
    DrawerPolyline polyline;

    QList<vtkSplineWidget*> m_splinesList = m_splinesMap.values( slice );
    QList<double*> m_pointsList;

    foreach( vtkSplineWidget *spline, m_splinesList )
    {
        spline->GetPolyData( pd );
        points = pd->GetPoints();

        //en el següent loop reduïm el nombre de punts per tal de facilitar el càlcul i perquè tants punts no són necessaris.
        for ( j = 0; j < points->GetNumberOfPoints(); j = j + 4 )
        {
            currentPoint = points->GetPoint( j );
            aux = new double[3];

            for ( i = 0; i < 3; i++)
            aux[i] = currentPoint[i];

            polyline.addPoint( aux );
        }

        currentPoint = points->GetPoint( points->GetNumberOfPoints() - 1 );
        aux = new double[3];

        for ( i = 0; i < 3; i++)
            aux[i] = currentPoint[i];

        polyline.addPoint( aux );

        currentPoint = points->GetPoint( 0 );
        aux = new double[3];

        for ( i = 0; i < 3; i++)
            aux[i] = currentPoint[i];

        polyline.addPoint( aux );

        currentArea += polyline.computeArea( m_view );
        polyline.deleteAllPoints();
    }
    m_areaLabel->setText( QString("%1 mm2").arg( currentArea, 0, 'f', 2 ) );
}

void QVolumeContourDelimiterExtension::computeTotalVolume()
{
    double *currentPoint, *aux;
    vtkPolyData *pd  = vtkPolyData::New();
    vtkPoints *points;
    int i,j;
    double currentVolume = 0.;
    DrawerPolyline polyline;

    QList<vtkSplineWidget*> m_splinesList = m_splinesMap.values();
    QList<double*> m_pointsList;

    foreach( vtkSplineWidget *spline, m_splinesList )
    {
        spline->GetPolyData( pd );
        points = pd->GetPoints();

        //en el següent loop reduïm el nombre de punts per tal de facilitar el càlcul i perquè tants punts no són necessaris.
        for ( j = 0; j < points->GetNumberOfPoints(); j = j + 4 )
        {
            currentPoint = points->GetPoint( j );
            aux = new double[3];

            for ( i = 0; i < 3; i++)
                aux[i] = currentPoint[i];

            polyline.addPoint( aux );
        }

        currentPoint = points->GetPoint( points->GetNumberOfPoints() - 1 );
        aux = new double[3];

        for ( i = 0; i < 3; i++)
            aux[i] = currentPoint[i];

        polyline.addPoint( aux );

        currentPoint = points->GetPoint( 0 );
        aux = new double[3];

        for ( i = 0; i < 3; i++)
            aux[i] = currentPoint[i];

        polyline.addPoint( aux );

        currentVolume += polyline.computeArea( m_view );
        polyline.deleteAllPoints();
    }
    //per saber el volum, multipliquem l'àrea per l'espaiat de profunditat, en aquest cas pel de z.
    if ( m_volume->getNumberOfPhases() == 1 )
        currentVolume *= m_2DView->getInput()->getSpacing()[2];
    else
        currentVolume *= m_volume->getPhaseVolume( m_2DView->getCurrentPhase() )->getSpacing()[2];

    m_volumeLabel->setText( QString("%1 mm3").arg( currentVolume, 0, 'f', 2 ) );
}

void QVolumeContourDelimiterExtension::computeAreaAndVolume()
{
    computeCurrentArea( m_2DView->getCurrentSlice() );
    computeTotalVolume();
}
};

