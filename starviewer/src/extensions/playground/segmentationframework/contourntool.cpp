/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "contourntool.h"
#include "volume.h"
#include "q2dviewer.h"
#include "iostream.h"
//#include <determinatecontour.h>
#include <llescacontorn.h>
#include "logging.h"
#include "areaspline.h"
//QT
#include <QAction>
#include <QApplication>
//VTK
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSplineWidget.h>
#include <vtkProperty.h>


namespace udg {

ContournTool::ContournTool( Q2DViewer *viewer, Volume * seg,QObject *parent )

{
    m_state = NONE;
    m_2DViewer = viewer;
    m_seedSlice = -1;
    m_lastView= -1;
    m_conAxial = 0;
    m_seg=seg;
    m_spline = vtkSplineWidget::New();
    m_areaSpline=new AreaSpline();
    m_calculat = false;
    connect( m_2DViewer , SIGNAL( sliceChanged(int) ) , this , SLOT( sliceChanged(int) ) );
    connect( m_2DViewer , SIGNAL( viewChanged(int) ) , this , SLOT( viewChanged(int) ) );
    connect( m_2DViewer , SIGNAL( eventReceived(unsigned long) ) , this , SLOT( handleEvent(unsigned long) ) );
    //connect( m_conAxial, SIGNAL(finish()),this, SLOT(DibuixaSpline()));
    //connect( m_conCoronal, SIGNAL(finish()),this, SLOT(DibuixaSpline()));
    //connect( m_conSagital, SIGNAL(finish()),this, SLOT(DibuixaSpline()));
    setContourn();
    
    
}


ContournTool::~ContournTool()
{

     //m_spline->Off();
     //m_spline->Delete();

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
        getCoords();
        //std::cout<<"HE FET CLICK"<<std::endl;
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

void ContournTool::getCoords(){

m_2DViewer->getCurrentCursorPosition(m_seedPosition);
std::cout<<"HE FET CLICK"<<std::endl;
int slice=m_2DViewer->getSlice();
doContouring(slice);

}







void ContournTool::setContourn( )
{
    //vtkProperty *prop=vtkProperty::New();
    m_state=CONTOURING;
    
    xt=((ImageType*)m_seg->getItkData())->GetSpacing()[1];
    yt=((ImageType*)m_seg->getItkData())->GetSpacing()[2];
    zt=((ImageType*)m_seg->getItkData())->GetSpacing()[0];
    

    vtkProperty *prop=vtkProperty::New();
   // m_spline = vtkSplineWidget::New();
     m_spline->SetPriority(1.0);
    m_spline->SetInteractor(m_2DViewer->getInteractor());
     m_spline->ProjectToPlaneOn();
    
     prop->SetPointSize(0.05);
    //prop->SetOpacity(0.2);
     prop->SetColor(1,0,0);
     m_spline->SetHandleProperty(prop);


   /* m_spline->SetPriority(1.0);
    m_spline->SetInteractor(m_2DViewer->getInteractor());
    m_spline->ProjectToPlaneOn();
    
    prop->SetPointSize(0.05);
    //prop->SetOpacity(0.2);
    prop->SetColor(1,0,0);
    m_spline->SetHandleProperty(prop);*/
    //m_spline->AddObserver(vtkCommand::InteractionEvent,

     if(!m_calculat){
     //QApplication::setOverrideCursor(Qt::WaitCursor);
    ///SAGITAL
    // m_conSagital=new LlescaContorn((ImageType*)m_seg->getItkData(),1);
      //  m_con3=new DeterminateContour((ImageType*)m_seg->getItkData(),1);
    ///AXIAL
     //m_con2=new DeterminateContour((ImageType*)m_seg->getItkData(),2);
     m_conAxial=new LlescaContorn((ImageType*)m_seg->getItkData(),2);
       
     ///CORONAL 
     //m_con=new DeterminateContour((ImageType*)m_seg->getItkData(),0);
    // m_conCoronal = new LlescaContorn((ImageType*)m_seg->getItkData(),0);
    
     m_calculat=true;
    }
    //QApplication::restoreOverrideCursor();
    
    
    
    //m_conAxial->ObtenirContorn(m_seedSlice);
    std::cout<<"He de ser l'ultim missatge!!"<<std::endl;
    //doContouring();

   

}

void ContournTool::doContouring(int slice)
{

//if(m_state==CONTOURING){
int i;
bool trob;
//m_spline->SetNumberOfHandles(0);
switch( m_2DViewer->getView() )
            {
            case Axial:
            m_seedSlice=slice;
            m_conAxial->ObtenirContorn(slice , (int) m_seedPosition[0], (int) m_seedPosition[1]);
            if(m_conAxial->ItsFinish()){
            DibuixaSpline();
            }else{std::cout<<"no estic"<<std::endl;}
            //m_lastView=m_2DViewer->getView();
            //
            //m_spline->SetProjectionPosition(m_2DViewer->getSlice()-1);
            /*m_spline->SetProjectionPosition(slice-1);
            m_spline->SetProjectionNormal(2);

            if(m_conAxial->c->puntsContorn>=3){
                                
					m_spline->SetNumberOfHandles(m_conAxial->c->puntsContorn);
                         std::cout<<"tenim punts de contorn: "<<m_conAxial->c->puntsContorn<<std::endl;
					//int k=0;
					for(int j=0;j<m_conAxial->c->puntsContorn;j++){
						//m_spline->SetHandlePosition(k,0,(m_con->c->x[j])*m_con->c->xt,(m_con->c->y[j])*m_con->c->yt);//spacing
                                                m_spline->SetHandlePosition(j,(m_conAxial->c->x[j])*zt,(m_conAxial->c->y[j])*xt,0);//spacing
						//std::cout<<"C->x[j]"<<C->x[j]<<"   C->y[j]"<<C->y[j]<<"   C->z[j]"<<C->z[j]<<"   j"<<i<<std::endl;
						//k++;
					}
				}
            m_spline->On();
            m_2DViewer->getInteractor()->Render();
            */
            /*m_lastView=m_2DViewer->getView();
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
            //emit (setLenght(m_spline->GetSummedLength()));
            //m_spline->ClosedOn();
            m_spline->On();
            m_2DViewer->getInteractor()->Render();*/
            



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

            m_seedSlice=slice;
            //m_conSagital->ObtenirContorn(slice , (int) m_seedPosition[0], (int) m_seedPosition[1]);
            DibuixaSpline();
            //m_lastView=m_2DViewer->getView();
            /*
            m_spline->SetProjectionPosition(m_2DViewer->getSlice()-1);
            m_spline->SetProjectionNormal(1);

            if(m_conSagital->c->puntsContorn>=3){
                                
					m_spline->SetNumberOfHandles(m_conSagital->c->puntsContorn);
                         std::cout<<"tenim punts de contorn: "<<m_conSagital->c->puntsContorn<<std::endl;
					//int k=0;
					for(int j=0;j<m_conSagital->c->puntsContorn;j++){
						//m_spline->SetHandlePosition(k,0,(m_con->c->x[j])*m_con->c->xt,(m_con->c->y[j])*m_con->c->yt);//spacing
                                                m_spline->SetHandlePosition(j,(m_conSagital->c->x[j])*zt,0,(m_conSagital->c->y[j])*xt);//spacing
						//std::cout<<"C->x[j]"<<C->x[j]<<"   C->y[j]"<<C->y[j]<<"   C->z[j]"<<C->z[j]<<"   j"<<i<<std::endl;
						//k++;
					}
				}
            m_spline->On();
            m_2DViewer->getInteractor()->Render();
            */
           /* m_lastView=m_2DViewer->getView();
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
            m_2DViewer->getInteractor()->Render();*/
            //emit setLenght(m_spline->GetSummedLength());
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

            m_seedSlice=slice;
            //m_conCoronal->ObtenirContorn(slice , (int) m_seedPosition[0], (int) m_seedPosition[1]);
            DibuixaSpline();
            //m_lastView=m_2DViewer->getView();
            //
            /*m_spline->SetProjectionPosition(m_2DViewer->getSlice()+1);
            m_spline->SetProjectionNormal(0);

            if(m_conCoronal->c->puntsContorn>=3){
                                
					m_spline->SetNumberOfHandles(m_conCoronal->c->puntsContorn);
                         std::cout<<"tenim punts de contorn: "<<m_conCoronal->c->puntsContorn<<std::endl;
					//int k=0;
					for(int j=0;j<m_conCoronal->c->puntsContorn;j++){
						//m_spline->SetHandlePosition(k,0,(m_con->c->x[j])*m_con->c->xt,(m_con->c->y[j])*m_con->c->yt);//spacing
                                                m_spline->SetHandlePosition(j,0,(m_conCoronal->c->x[j])*zt,(m_conCoronal->c->y[j])*xt);//spacing
						//std::cout<<"C->x[j]"<<C->x[j]<<"   C->y[j]"<<C->y[j]<<"   C->z[j]"<<C->z[j]<<"   j"<<i<<std::endl;
						//k++;
					}
				}
            m_spline->On();
            m_2DViewer->getInteractor()->Render(); 
            */
           /* m_lastView=m_2DViewer->getView();
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
            m_2DViewer->getInteractor()->Render();*/
            

            break;
            }
//m_spline->ClosedOn();
//m_spline->SetResolution(1);
//m_spline->SetNumberOfHandles(2);
//double lenght=m_spline->GetSummedLength();
//emit setLenght(lenght);
//}
   


}

void ContournTool::DibuixaSpline(){


m_spline->Off();
m_2DViewer->getInteractor()->Render();

//vtkSplineWidget*  m_spline = vtkSplineWidget::New();
//vtkProperty *prop=vtkProperty::New();
   // m_spline = vtkSplineWidget::New();
  //   m_spline->SetPriority(1.0);
    // m_spline->SetInteractor(m_2DViewer->getInteractor());
    // m_spline->ProjectToPlaneOn();
    
     //prop->SetPointSize(0.05);
    //prop->SetOpacity(0.2);
    // prop->SetColor(1,0,0);
     //m_spline->SetHandleProperty(prop);
switch( m_2DViewer->getView() )
            {
            case Axial:
            //m_seedSlice=m_2DViewer->getSlice();
            //m_conAxial->ObtenirContorn(slice);
            m_lastView=m_2DViewer->getView();
            //
            //m_spline->SetProjectionPosition(m_2DViewer->getSlice()-1);
            m_spline->SetProjectionPosition(m_seedSlice-1);
            m_spline->SetProjectionNormal(2);

            if(m_conAxial->c->puntsContorn>=3){
                                
					m_spline->SetNumberOfHandles(m_conAxial->c->puntsContorn);
                         std::cout<<"tenim punts de contorn: "<<m_conAxial->c->puntsContorn<<std::endl;
					//int k=0;
					for(int j=0;j<m_conAxial->c->puntsContorn;j++){
						//m_spline->SetHandlePosition(k,0,(m_con->c->x[j])*m_con->c->xt,(m_con->c->y[j])*m_con->c->yt);//spacing
                                                m_spline->SetHandlePosition(j,(m_conAxial->c->x[j])*zt,(m_conAxial->c->y[j])*xt,0);//spacing
						//std::cout<<"C->x[j]"<<C->x[j]<<"   C->y[j]"<<C->y[j]<<"   C->z[j]"<<C->z[j]<<"   j"<<i<<std::endl;
						//k++;
					}
				}
            m_spline->On();
            m_spline->ClosedOn();
            m_2DViewer->getInteractor()->Render();

          
          
            break;
            case Sagital:

           // m_seedSlice=m_2DViewer->getSlice();
            //m_conSagital->ObtenirContorn(m_seedSlice);
            m_lastView=m_2DViewer->getView();
            //
            m_spline->SetProjectionPosition(m_seedSlice-1);
            m_spline->SetProjectionNormal(1);

            if(m_conSagital->c->puntsContorn>=3){
                                
					m_spline->SetNumberOfHandles(m_conSagital->c->puntsContorn);
                         std::cout<<"tenim punts de contorn: "<<m_conSagital->c->puntsContorn<<std::endl;
					//int k=0;
					for(int j=0;j<m_conSagital->c->puntsContorn;j++){
						//m_spline->SetHandlePosition(k,0,(m_con->c->x[j])*m_con->c->xt,(m_con->c->y[j])*m_con->c->yt);//spacing
                                                m_spline->SetHandlePosition(j,(m_conSagital->c->x[j])*zt,0,(m_conSagital->c->y[j])*xt);//spacing
						//std::cout<<"C->x[j]"<<C->x[j]<<"   C->y[j]"<<C->y[j]<<"   C->z[j]"<<C->z[j]<<"   j"<<i<<std::endl;
						//k++;
					}
				}
            m_spline->On();
            m_spline->ClosedOn();
            m_2DViewer->getInteractor()->Render();
            
          
            break;
            case Coronal:

            //m_seedSlice=m_2DViewer->getSlice();
            //m_conCoronal->ObtenirContorn(m_seedSlice);
            m_lastView=m_2DViewer->getView();
            //
            m_spline->SetProjectionPosition(m_seedSlice+1);
            m_spline->SetProjectionNormal(0);

            if(m_conCoronal->c->puntsContorn>=3){
                                
					m_spline->SetNumberOfHandles(m_conCoronal->c->puntsContorn);
                         std::cout<<"tenim punts de contorn: "<<m_conCoronal->c->puntsContorn<<std::endl;
					//int k=0;
					for(int j=0;j<m_conCoronal->c->puntsContorn;j++){
						//m_spline->SetHandlePosition(k,0,(m_con->c->x[j])*m_con->c->xt,(m_con->c->y[j])*m_con->c->yt);//spacing
                                                m_spline->SetHandlePosition(j,0,(m_conCoronal->c->x[j])*zt,(m_conCoronal->c->y[j])*xt);//spacing
						//std::cout<<"C->x[j]"<<C->x[j]<<"   C->y[j]"<<C->y[j]<<"   C->z[j]"<<C->z[j]<<"   j"<<i<<std::endl;
						//k++;
					}
				}
            m_spline->On();
            m_spline->ClosedOn();
            m_2DViewer->getInteractor()->Render(); 
            //m_spline->Delete();
        
      
            

            break;
            }





}

void ContournTool::sliceChanged( int s )
{
   /* if(m_seedSlice==s)
    {
        m_spline->On();
        m_2DViewer->getInteractor()->Render();
    }
    else
    {
        m_spline->Off();
        m_2DViewer->getInteractor()->Render();
        doContouring();
        
    }*/

}


void ContournTool::viewChanged( int s )
{
   /* if(m_lastView!=s)
    {
        m_spline->Off();
        m_2DViewer->getInteractor()->Render();
        doContouring();
       
    }
    else{ m_2DViewer->getInteractor()->Render(); }
   */
}

double ContournTool::getLength( )
{
    double area=0;
    
    m_areaSpline->CalculateArea(m_spline,m_lastView);
    area=m_areaSpline->getArea();
    //area = area^2;
    //area = area^1/2;
    return(area);
  //return ( m_spline->GetSummedLength());
   
}



}
