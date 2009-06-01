/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "editortool.h"
#include "editortooldata.h"
#include "logging.h"
#include "q2dviewer.h"
#include "volume.h"

//vtk
#include <vtkCommand.h>
#include <vtkPoints.h>
#include <vtkUnstructuredGrid.h>
#include <vtkProperty.h>

#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkImageIterator.h>
#include <vtkDataSetMapper.h>
#include <vtkContourGrid.h>
#include <vtkRenderWindowInteractor.h>

//itk
#include "itkMinimumMaximumImageCalculator.h"

namespace udg {

EditorTool::EditorTool( QViewer *viewer, QObject *parent )
 : Tool(viewer,parent), m_volumeCont(0), m_insideValue(255), m_outsideValue(0), m_isLeftButtonPressed(false)
{
    m_editorState = Paint;
    m_editorSize  = 3;
    m_toolName = "EditorTool";
    m_squareActor = vtkActor::New();
    m_myData = new EditorToolData;

    m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
    // ens assegurem que desde la creació tenim un viewer vàlid
    Q_ASSERT( m_2DViewer );

    m_2DViewer->setCursor( QCursor( QPixmap(":/images/pencilcursor.png") ) );
    this->initialize();

    // \TODO:cada cop que canvïi l'input cal fer algunes inicialitzacions
    connect( m_2DViewer, SIGNAL(overlayChanged() ), SLOT( initialize() ) );
}

EditorTool::~EditorTool()
{
    m_2DViewer->setCursor(Qt::ArrowCursor);
    m_squareActor -> Delete();
}

void EditorTool::initialize(  )
{
    if(m_2DViewer->getOverlayInput()!=0)
    {
        itk::MinimumMaximumImageCalculator< Volume::ItkImageType >::Pointer minmaxCalc = itk::MinimumMaximumImageCalculator< Volume::ItkImageType >::New();
    
        minmaxCalc->SetImage(m_2DViewer->getOverlayInput()->getItkData());
        minmaxCalc->SetRegion(m_2DViewer->getOverlayInput()->getItkData()->GetRequestedRegion());
        minmaxCalc->Compute();
    
        m_outsideValue = minmaxCalc->GetMinimum();
        m_insideValue  = minmaxCalc->GetMaximum();

//        DEBUG_LOG( QString( "Initialize: Minim = %1 // Maxim = %2" ).arg( m_outsideValue ).arg( m_insideValue ) );

        int ext[6];
        int i,j,k;
        m_volumeCont = 0;
        m_2DViewer->getOverlayInput()->getWholeExtent(ext);
    
        Volume::VoxelType *value = m_2DViewer->getOverlayInput()->getScalarPointer();
        for(i=ext[0];i<=ext[1];i++)
        {
            for(j=ext[2];j<=ext[3];j++)
            {
                for(k=ext[4];k<=ext[5];k++)
                {
                    if ((*value) == m_insideValue)
                    {
                        m_volumeCont++;
                    }
                    value++;
                }
            }
        }
        m_myData->setVolumeVoxels(m_volumeCont);
    }
}

void EditorTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::LeftButtonPressEvent:
        if(m_2DViewer->getOverlayInput()!=0)
        {
            m_isLeftButtonPressed = true;
            this->setEditorPoint(  );
        }
    break;

    case vtkCommand::MouseMoveEvent:
        if(m_2DViewer->getInput()!=0)
        {
            this->setPaintCursor();
        }
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        m_isLeftButtonPressed = false;
    break;

    case vtkCommand::MouseWheelForwardEvent:
        this->increaseState();
    break;

    case vtkCommand::MouseWheelBackwardEvent:
        this->decreaseState();
    break;

    case vtkCommand::KeyPressEvent:
    {
        int key = m_viewer->getInteractor()->GetKeyCode();
        // '+' = key code 43
        // '-' = key code 45
        switch( key )
        {
        case 43: // '+'
            this->increaseEditorSize();
        break;

        case 45: // '-'
            this->decreaseEditorSize();
        break;
        }
    }
    break;

    default:
    break;
    }
}

void EditorTool::increaseState()
{
        //Prova!!!!!!!!!!!!!!!!!1
/*
        itk::MinimumMaximumImageCalculator< Volume::ItkImageType >::Pointer minmaxCalc = itk::MinimumMaximumImageCalculator< Volume::ItkImageType >::New();
    
        minmaxCalc->SetImage(m_2DViewer->getOverlayInput()->getItkData());
        minmaxCalc->SetRegion(m_2DViewer->getOverlayInput()->getItkData()->GetRequestedRegion());
        minmaxCalc->Compute();
    
        m_outsideValue = minmaxCalc->GetMinimum();
        m_insideValue  = minmaxCalc->GetMaximum();

        DEBUG_LOG( QString( "Initialize: Minim = %1 // Maxim = %2" ).arg( m_outsideValue ).arg( m_insideValue ) );
  */      //Prova!!!!!!!!!!!!!!!!!1


    switch( m_editorState )
    {
    case Paint:
        m_editorState = Erase;
        m_2DViewer->setCursor( QCursor( QPixmap(":/images/erasercursor.png") ) );
    break;
    case Erase:
        m_editorState = EraseRegion;
        m_2DViewer->setCursor( QCursor( QPixmap(":/images/eraseregioncursor.png") ) );
        m_squareActor->VisibilityOff();
        m_2DViewer->refresh();
    break;
    case EraseRegion:
        m_editorState = EraseSlice;
        m_2DViewer->setCursor( QCursor( QPixmap(":/images/slicecursor.png") ) );
    break;
    case EraseSlice:
        m_editorState = Paint;
        m_2DViewer->setCursor( QCursor( QPixmap(":/images/pencilcursor.png") ) );
        this->setPaintCursor();
    break;
    default:
    break;
    }
}

void EditorTool::decreaseState()
{
    switch( m_editorState )
    {
    case EraseRegion:
        m_editorState = Erase;
        m_2DViewer->setCursor( QCursor( QPixmap(":/images/erasercursor.png") ) );
        this->setPaintCursor();
    break;
    case EraseSlice:
        m_editorState = EraseRegion;
        m_2DViewer->setCursor( QCursor( QPixmap(":/images/eraseregioncursor.png") ) );
    break;
    case Paint:
        m_editorState = EraseSlice;
        m_2DViewer->setCursor( QCursor( QPixmap(":/images/slicecursor.png") ) );
        m_squareActor->VisibilityOff();
        m_2DViewer->refresh();
    break;
    case Erase:
        m_editorState = Paint;
        m_2DViewer->setCursor( QCursor( QPixmap(":/images/pencilcursor.png") ) );
    break;
    default:
    break;
    }

}

void EditorTool::setErase()
{
    m_editorState = Erase;
}

void EditorTool::setPaint()
{
    m_editorState = Paint;
}

void EditorTool::setEraseSlice()
{
    m_editorState = EraseSlice;
}

void EditorTool::setEraseRegion()
{
    m_editorState = EraseRegion;
}


void EditorTool::setEditorPoint(  )
{
    double pos[3];
    if(m_editorState != NoEditor)
    {
        if( m_2DViewer->getCurrentCursorImageCoordinate(pos) )
        {
            switch( m_editorState)
            {
                case Erase:
                {
                    this->eraseMask();
                    break;
                }
                case Paint:
                {
                    this->paintMask();
                    break;
                }
                case EraseSlice:
                {
                    this->eraseSliceMask();
                    break;
                }
                case EraseRegion:
                {
                    this->eraseRegionMask();
                    break;
                }
            }
            m_myData->setVolumeVoxels(m_volumeCont);
            m_2DViewer->isOverlayModified();
            m_2DViewer->refresh();
        }
    }
}

void EditorTool::setPaintCursor()
{
    if(m_isLeftButtonPressed)
    {
        setEditorPoint();
    }
  
    double pos[3];
    if((m_editorState == Erase || m_editorState == Paint)&& m_2DViewer->getCurrentCursorImageCoordinate(pos) )
    {
        int size = m_editorSize;
        vtkPoints *points = vtkPoints::New();
        points->SetNumberOfPoints(4);

        double spacing[3];
        m_2DViewer->getInput()->getSpacing(spacing);
        double sizeView[2];
        sizeView[0]=(double)size*spacing[0];
        sizeView[1]=(double)size*spacing[1];
  
        points->SetPoint(0, pos[0] - sizeView[0], pos[1] - sizeView[1], pos[2]-1);
        points->SetPoint(1, pos[0] + sizeView[0], pos[1] - sizeView[1], pos[2]-1);
        points->SetPoint(2, pos[0] + sizeView[0], pos[1] + sizeView[1], pos[2]-1);
        points->SetPoint(3, pos[0] - sizeView[0], pos[1] + sizeView[1], pos[2]-1);
  
  
        vtkIdType pointIds[4];
  
        pointIds[0] = 0;
        pointIds[1] = 1;
        pointIds[2] = 2;
        pointIds[3] = 3;
  
  
        vtkUnstructuredGrid*    grid = vtkUnstructuredGrid::New();
  
        grid->Allocate(1);
        grid->SetPoints(points);
  
        grid->InsertNextCell(VTK_QUAD,4,pointIds);
  
        m_squareActor -> GetProperty()->SetColor(0.15, 0.83, 0.26);
        m_squareActor -> GetProperty()->SetOpacity(0.2);
  
        vtkDataSetMapper *squareMapper = vtkDataSetMapper::New();
        squareMapper->SetInput( grid );
  
        m_squareActor->SetMapper( squareMapper );
  
        m_squareActor->VisibilityOn();
  
        m_2DViewer->getRenderer()->AddViewProp( m_squareActor );
        m_2DViewer->getRenderer()->ResetCameraClippingRange();
        m_2DViewer->refresh();
  
        squareMapper->Delete();
        points->Delete();
        grid->Delete();
    }
    else
    {
        m_squareActor->VisibilityOff();
    }
}

void EditorTool::eraseMask()
{
    int i,j;
	Volume::VoxelType *value;
    double pos[3];
    double origin[3];
    double spacing[3];
    int centralIndex[3];
    int index[3];
    m_2DViewer->getCurrentCursorImageCoordinate(pos);
    m_2DViewer->getInput()->getSpacing(spacing);
    m_2DViewer->getInput()->getOrigin(origin);
    centralIndex[0]=(int)(((double)pos[0]-origin[0])/spacing[0]);
    centralIndex[1]=(int)(((double)pos[1]-origin[1])/spacing[1]);
    index[2]=m_2DViewer->getCurrentSlice();

    for(i=-m_editorSize;i<=m_editorSize;i++)
    {
        for(j=-m_editorSize;j<=m_editorSize;j++)
        {
            index[0]=centralIndex[0]+i;
            index[1]=centralIndex[1]+j;
            value = m_2DViewer->getOverlayInput()->getScalarPointer(index);
            if(value && ( (*value) == m_insideValue) )
            {
                (*value) = m_outsideValue;
                m_volumeCont--;
            }
        }
    }
}

void EditorTool::paintMask()
{

    //DEBUG_LOG( QString( "Màxim = %1 // Mínim = %2" ).arg( m_outsideValue ).arg( m_insideValue ) );

    int i,j;
    Volume::VoxelType *value;
    double pos[3];
    double origin[3];
    double spacing[3];
    int centralIndex[3];
    int index[3];
    m_2DViewer->getCurrentCursorImageCoordinate(pos);
    m_2DViewer->getInput()->getSpacing(spacing);
    m_2DViewer->getInput()->getOrigin(origin);
    centralIndex[0]=(int)(((double)pos[0]-origin[0])/spacing[0]);
    centralIndex[1]=(int)(((double)pos[1]-origin[1])/spacing[1]);
    index[2]=m_2DViewer->getCurrentSlice();
    for(i=-m_editorSize;i<=m_editorSize;i++)
    {
        for(j=-m_editorSize;j<=m_editorSize;j++)
        {
            index[0]=centralIndex[0]+i;
            index[1]=centralIndex[1]+j;
            value = m_2DViewer->getOverlayInput()->getScalarPointer(index);
            if(value && ((*value) != m_insideValue) )
            {
                (*value) = m_insideValue;
                m_volumeCont++;
            }
        }
    }
}

void EditorTool::eraseSliceMask()
{
    int i,j;
    Volume::VoxelType *value;
    double pos[3];
    double origin[3];
    double spacing[3];
    int centralIndex[3];
    int index[3];
    int ext[6];
    m_2DViewer->getInput()->getWholeExtent(ext);
    m_2DViewer->getCurrentCursorImageCoordinate(pos);
    m_2DViewer->getInput()->getSpacing(spacing);
    m_2DViewer->getInput()->getOrigin(origin);
    centralIndex[0]=(int)(((double)pos[0]-origin[0])/spacing[0]);
    centralIndex[1]=(int)(((double)pos[1]-origin[1])/spacing[1]);
    index[2]=m_2DViewer->getCurrentSlice();
    for(i=ext[0];i<=ext[1];i++)
    {
        for(j=ext[2];j<=ext[3];j++)
        {
            index[0]=i;
            index[1]=j;
            value = m_2DViewer->getOverlayInput()->getScalarPointer(index);
            if((*value) == m_insideValue)
            {
                (*value) = m_outsideValue;
                m_volumeCont--;
            }
        }
    }
}

void EditorTool::eraseRegionMask()
{
    double pos[3];
    double origin[3];
    double spacing[3];
    int index[3];
    int ext[6];
    m_2DViewer->getInput()->getWholeExtent(ext);
    m_2DViewer->getCurrentCursorImageCoordinate(pos);
    m_2DViewer->getInput()->getSpacing(spacing);
    m_2DViewer->getInput()->getOrigin(origin);
    index[0]=(int)(((double)pos[0]-origin[0])/spacing[0]);
    index[1]=(int)(((double)pos[1]-origin[1])/spacing[1]);
    index[2]=m_2DViewer->getCurrentSlice();
    eraseRegionMaskRecursive(index[0],index[1],index[2]);
}

void EditorTool::eraseRegionMaskRecursive(int a, int b, int c)
{
    int ext[6];
    m_2DViewer->getInput()->getWholeExtent(ext);
    if((a>=ext[0])&&(a<=ext[1])&&(b>=ext[2])&&(b<=ext[3])&&(c>=ext[4])&&(c<=ext[5]))
    {
        Volume::VoxelType *value = m_2DViewer->getOverlayInput()->getScalarPointer(a,b,c);
        if ((*value) == m_insideValue)
        {
            (*value)= m_outsideValue;
            m_volumeCont--;
            eraseRegionMaskRecursive( a+1, b, c);
            eraseRegionMaskRecursive( a-1, b, c);
            eraseRegionMaskRecursive( a, b+1, c);
            eraseRegionMaskRecursive( a, b-1, c);
        }
    }
}

void EditorTool::increaseEditorSize()
{
    m_editorSize++;
    this->setPaintCursor();
}

void EditorTool::decreaseEditorSize()
{
    if(m_editorSize > 0)
    {
        m_editorSize--;
        this->setPaintCursor();
    }
}

ToolData *EditorTool::getToolData() const
{
    return m_myData;
}

}

