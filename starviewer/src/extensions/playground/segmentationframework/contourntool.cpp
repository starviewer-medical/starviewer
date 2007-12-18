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
#include <llescacontorn.h>
#include <llenca.h>
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
#include <vtkPolyData.h>

namespace udg {

class vtkSWCallback : public vtkCommand
{

public:
  static vtkSWCallback *New()
    { return new vtkSWCallback;

    }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      vtkSplineWidget *spline = reinterpret_cast<vtkSplineWidget*>(caller);
      ll->fesllenca(spline->GetSummedLength());
    }
  vtkSWCallback():Poly(0){};
  vtkPolyData* Poly;
  llenca* ll;
};

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

    setContourn();
}

ContournTool::~ContournTool()
{

}

void ContournTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::LeftButtonPressEvent:
        getCoords();
    break;

    case vtkCommand::MouseMoveEvent:
    break;

    case vtkCommand::LeftButtonReleaseEvent:
    break;

    default:
    break;
    }
}

void ContournTool::getCoords()
{
    m_2DViewer->getCurrentCursorPosition(m_seedPosition);

    int slice=m_2DViewer->getCurrentSlice();
    doContouring(slice);
}

void ContournTool::setContourn( )
{

    m_state=CONTOURING;

    xt=((ImageType*)m_seg->getItkData())->GetSpacing()[1];
    yt=((ImageType*)m_seg->getItkData())->GetSpacing()[2];
    zt=((ImageType*)m_seg->getItkData())->GetSpacing()[0];


    vtkProperty *prop=vtkProperty::New();
    m_spline->SetPriority(1.0);
    m_spline->SetInteractor(m_2DViewer->getInteractor());
    m_spline->ProjectToPlaneOn();

    prop->SetPointSize(0.05);

    prop->SetColor(1,0,0);
    m_spline->SetHandleProperty(prop);

    vtkPolyData * poly= vtkPolyData::New();
    m_spline->GetPolyData(poly);
    l = new llenca();
    connect(l,SIGNAL(e(double)),this,SLOT(changespline(double )));


    vtkSWCallback* swcb =vtkSWCallback::New();
    swcb->Poly = poly;
    swcb->ll = l;

    m_spline->AddObserver(vtkCommand::InteractionEvent,swcb);



     if(!m_calculat){
     QApplication::setOverrideCursor(Qt::WaitCursor);
    ///SAGITAL
     m_conSagital=new LlescaContorn((ImageType*)m_seg->getItkData(),1);
      ///AXIAL
     m_conAxial=new LlescaContorn((ImageType*)m_seg->getItkData(),2);
     ///CORONAL
     m_conCoronal = new LlescaContorn((ImageType*)m_seg->getItkData(),0);
     m_calculat=true;
     QApplication::restoreOverrideCursor();
    }

}

void ContournTool::doContouring(int slice)
{


int i;
bool trob;

switch( m_2DViewer->getView() )
            {
            case Axial:
            m_seedSlice=slice;
            m_conAxial->ObtenirContorn(slice , (int) m_seedPosition[0], (int) m_seedPosition[1]);
            if(m_conAxial->ItsFinish()){
            DibuixaSpline();
            }


            break;
            case Sagital:

            m_seedSlice=slice;
            m_conSagital->ObtenirContorn(slice , (int) m_seedPosition[0], (int) m_seedPosition[2]);
            DibuixaSpline();

            break;
            case Coronal:

            m_seedSlice=slice;
            m_conCoronal->ObtenirContorn(slice , (int) m_seedPosition[1], (int) m_seedPosition[2]);
            DibuixaSpline();


            break;
            }


}

void ContournTool::DibuixaSpline(){


m_spline->Off();
m_2DViewer->refresh();

switch( m_2DViewer->getView() )
            {
            case Axial:

            m_lastView=m_2DViewer->getView();
            m_spline->SetProjectionPosition(m_seedSlice-1);
            m_spline->SetProjectionNormal(2);

            if(m_conAxial->c->puntsContorn>=3){

					m_spline->SetNumberOfHandles(m_conAxial->c->puntsContorn);

					for(int j=0;j<m_conAxial->c->puntsContorn;j++){

                                                m_spline->SetHandlePosition(j,(m_conAxial->c->x[j])*zt,(m_conAxial->c->y[j])*xt,0);//spacing

					}
				}
            m_spline->On();
            m_spline->ClosedOn();
            m_2DViewer->refresh();



            break;
            case Sagital:


            m_lastView=m_2DViewer->getView();
            m_spline->SetProjectionPosition(m_seedSlice-1);
            m_spline->SetProjectionNormal(1);

            if(m_conSagital->c->puntsContorn>=3){

					m_spline->SetNumberOfHandles(m_conSagital->c->puntsContorn);

					for(int j=0;j<m_conSagital->c->puntsContorn;j++){
						//m_spline->SetHandlePosition(k,0,(m_con->c->x[j])*m_con->c->xt,(m_con->c->y[j])*m_con->c->yt);//spacing
                                                m_spline->SetHandlePosition(j,(m_conSagital->c->x[j])*zt,0,(m_conSagital->c->y[j])*xt);//spacing

					}
				}
            m_spline->On();
            m_spline->ClosedOn();
            m_2DViewer->refresh();


            break;
            case Coronal:

            m_lastView=m_2DViewer->getView();
            m_spline->SetProjectionPosition(m_seedSlice+1);
            m_spline->SetProjectionNormal(0);

            if(m_conCoronal->c->puntsContorn>=3){

					m_spline->SetNumberOfHandles(m_conCoronal->c->puntsContorn);

					for(int j=0;j<m_conCoronal->c->puntsContorn;j++){
						//m_spline->SetHandlePosition(k,0,(m_con->c->x[j])*m_con->c->xt,(m_con->c->y[j])*m_con->c->yt);//spacing
                                                m_spline->SetHandlePosition(j,0,(m_conCoronal->c->x[j])*zt,(m_conCoronal->c->y[j])*xt);//spacing

					}
				}
            m_spline->On();
            m_spline->ClosedOn();
            m_2DViewer->refresh();
            break;
            }

emit originalspline(getLength(),m_seedSlice);



}

void ContournTool::sliceChanged( int s )
{
   /* if(m_seedSlice==s)
    {
        m_spline->On();
        m_2DViewer->refresh();
    }
    else
    {
        m_spline->Off();
        m_2DViewer->refresh();
        doContouring();

    }*/

}


void ContournTool::viewChanged( int s )
{
    /*if(m_lastView!=s)
    {

        m_spline->Off();
        m_2DViewer->refresh();
        doContouring();

    }
    else{ m_2DViewer->refresh(); }*/

}

double ContournTool::getLength( )
{
    double area=0;

    m_areaSpline->CalculateArea(m_spline,m_lastView);
    area=m_areaSpline->getArea();

    return(area);


}

void ContournTool::changespline(double j){

        double length=getLength();
        emit actualspline(length);
}



}
