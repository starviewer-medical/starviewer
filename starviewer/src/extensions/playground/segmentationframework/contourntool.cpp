/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "contourntool.h"
#include "volume.h"
#include "q2dviewer.h"
#include <determinatecontour.h>
#include "logging.h"
//QT
#include <QAction>
#include <QApplication>
//VTK
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSplineWidget.h>


namespace udg {

ContournTool::ContournTool( Q2DViewer *viewer, Volume * seg,QObject *parent )

{
    m_state = NONE;
    m_2DViewer = viewer;
    m_seedSlice = -1;
    m_lastView= -1;
    m_con = 0;
    m_seg=seg;
    m_spline = vtkSplineWidget::New();
    m_calculat = false;
    connect( m_2DViewer , SIGNAL( sliceChanged(int) ) , this , SLOT( sliceChanged(int) ) );
    connect( m_2DViewer , SIGNAL( viewChanged(int) ) , this , SLOT( viewChanged(int) ) );
    setContourn();
    
    
}


ContournTool::~ContournTool()
{

     m_spline->Off();
     m_spline->Delete();

}

/*void ContournTool::createAction()
{
}
*/
void ContournTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::LeftButtonPressEvent:
        //setContourn();
    break;

    case vtkCommand::MouseMoveEvent:
        //doContouring();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        //endContouring();
    break;

    default:
    break;
    }
}

void ContournTool::setContourn( )
{

    m_state=CONTOURING;
   
    xt=((ImageType*)m_seg->getItkData())->GetSpacing()[1];
    yt=((ImageType*)m_seg->getItkData())->GetSpacing()[2];
    zt=((ImageType*)m_seg->getItkData())->GetSpacing()[0];
    
    m_spline->SetPriority(1.0);
    m_spline->SetInteractor(m_2DViewer->getInteractor());
    m_spline->ProjectToPlaneOn();


     if(!m_calculat){
     //QApplication::setOverrideCursor(Qt::WaitCursor);

     m_con3=new DeterminateContour((ImageType*)m_seg->getItkData(),1);

     m_con2=new DeterminateContour((ImageType*)m_seg->getItkData(),2);
        
     m_con=new DeterminateContour((ImageType*)m_seg->getItkData(),0);
     
    
    m_calculat=true;
    }
    //QApplication::restoreOverrideCursor();
    
    doContouring();

   

}

void ContournTool::doContouring( )
{

if(m_state==CONTOURING){
int i;
bool trob;
switch( m_2DViewer->getView() )
            {
            case Axial:
            m_lastView=m_2DViewer->getView();
            m_seedSlice=m_2DViewer->getSlice();
            m_spline->SetProjectionPosition(m_2DViewer->getSlice()-1);
            m_spline->SetProjectionNormal(2);
		//sliceant=planeWidget->GetSliceIndex();
		trob=false;
		i=0;
		while((i<m_con2->c->np)&&(!trob)){
                
			if(m_con2->c->npslice[i][1]==(m_seedSlice)){
                        
				//std::cout<<"trobat:"<<C->npslice[i][1]<<"C->npslice[2]"<<C->npslice[i][2]<<",C->npslices[0]"<<C->npslice[i][0]<<std::endl;
				if(m_con2->c->npslice[i][0]>=3){
                                
					m_spline->SetNumberOfHandles(m_con2->c->npslice[i][0]);
					int k=0;
					for(int j=m_con2->c->npslice[i][2];j<m_con2->c->npslice[i][2]+m_con2->c->npslice[i][0];j++){
						//m_spline->SetHandlePosition(k,0,(m_con->c->x[j])*m_con->c->xt,(m_con->c->y[j])*m_con->c->yt);//spacing
                                                m_spline->SetHandlePosition(k,(m_con2->c->x[j])*zt,(m_con2->c->y[j])*xt,0);//spacing
						//std::cout<<"C->x[j]"<<C->x[j]<<"   C->y[j]"<<C->y[j]<<"   C->z[j]"<<C->z[j]<<"   j"<<i<<std::endl;
						k++;
					}
				}
					
				trob=true;
			}
		i++;
		}
            
            //m_spline->ClosedOn();
            m_spline->On();
            m_2DViewer->getInteractor()->Render();




            /*m_seedSlice=m_2DViewer->getSlice();
            m_lastView=m_2DViewer->getView();
            m_spline->SetProjectionPosition(m_2DViewer->getSlice()-1);
            m_spline->SetProjectionNormal(2);
            m_spline->SetNumberOfHandles(3);
            m_spline->SetHandlePosition(0,58*xt,133*yt,0);
            m_spline->SetHandlePosition(1,162*xt,77*yt,0);
            m_spline->SetHandlePosition(2,57*xt,33*yt,0);
            m_spline->ClosedOn();
            m_spline->On();
            m_2DViewer->getInteractor()->Render();*/
          
            break;
            case Sagital:
            m_lastView=m_2DViewer->getView();
            m_seedSlice=m_2DViewer->getSlice();
            m_spline->SetProjectionPosition(m_2DViewer->getSlice()-1);
            m_spline->SetProjectionNormal(1);
		//sliceant=planeWidget->GetSliceIndex();
		trob=false;
		i=0;
		while((i<m_con3->c->np)&&(!trob)){
                
			if(m_con3->c->npslice[i][1]==(m_seedSlice)){
                        
				//std::cout<<"trobat:"<<C->npslice[i][1]<<"C->npslice[2]"<<C->npslice[i][2]<<",C->npslices[0]"<<C->npslice[i][0]<<std::endl;
				if(m_con3->c->npslice[i][0]>=3){
                                
					m_spline->SetNumberOfHandles(m_con3->c->npslice[i][0]);
					int k=0;
					for(int j=m_con3->c->npslice[i][2];j<m_con3->c->npslice[i][2]+m_con3->c->npslice[i][0];j++){
						//m_spline->SetHandlePosition(k,0,(m_con->c->x[j])*m_con->c->xt,(m_con->c->y[j])*m_con->c->yt);//spacing
                                                m_spline->SetHandlePosition(k,(m_con3->c->x[j])*zt,0,(m_con3->c->y[j])*yt);//spacing
						//std::cout<<"C->x[j]"<<C->x[j]<<"   C->y[j]"<<C->y[j]<<"   C->z[j]"<<C->z[j]<<"   j"<<i<<std::endl;
						k++;
					}
				}
					
				trob=true;
			}
		i++;
		}
            
            //m_spline->ClosedOn();
            m_spline->On();
            m_2DViewer->getInteractor()->Render();

            /*
            m_seedSlice=m_2DViewer->getSlice();
            m_lastView=m_2DViewer->getView();
            m_spline->SetProjectionPosition(m_2DViewer->getSlice()-1);
            m_spline->SetProjectionNormal(1);
            m_spline->SetNumberOfHandles(3);
            m_spline->SetHandlePosition(0,58*xt,0,133*yt);
            m_spline->SetHandlePosition(1,162*xt,0,77*yt);
            m_spline->SetHandlePosition(2,57*xt,0,33*yt);
            m_spline->ClosedOn();
            m_spline->On();
            m_2DViewer->getInteractor()->Render();
           */
            break;
            case Coronal:
            m_lastView=m_2DViewer->getView();
            m_seedSlice=m_2DViewer->getSlice();
            m_spline->SetProjectionPosition(m_2DViewer->getSlice()+1);
            m_spline->SetProjectionNormal(0);
		
		trob=false;
		i=0;
		while((i<m_con->c->np)&&(!trob)){
                
			if(m_con->c->npslice[i][1]==(m_seedSlice)){
                        
				
				if(m_con->c->npslice[i][0]>=3){
                                
					m_spline->SetNumberOfHandles(m_con->c->npslice[i][0]);
					int k=0;
					for(int j=m_con->c->npslice[i][2];j<m_con->c->npslice[i][2]+m_con->c->npslice[i][0];j++){
						
                                                m_spline->SetHandlePosition(k,0,(m_con->c->x[j])*xt,(m_con->c->y[j])*yt);//spacing
						
						k++;
					}
				}
					
				trob=true;
			}
		i++;
		}
            
            
            m_spline->On();
            m_2DViewer->getInteractor()->Render();
                     

            break;
            }

}

}


void ContournTool::sliceChanged( int s )
{
    if(m_seedSlice==s)
    {
        m_spline->On();
        m_2DViewer->getInteractor()->Render();
    }
    else
    {
        m_spline->Off();
        m_2DViewer->getInteractor()->Render();
        doContouring();
        
    }

}


void ContournTool::viewChanged( int s )
{
    if(m_lastView!=s)
    {
        m_spline->Off();
        m_2DViewer->getInteractor()->Render();
        doContouring();
       
    }
    else{ m_2DViewer->getInteractor()->Render(); }
   
}





}
