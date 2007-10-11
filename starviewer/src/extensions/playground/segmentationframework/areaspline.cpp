/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "areaspline.h"
#include "vtkSplineWidget.h"
#include "vtkPolyData.h"
#include "vtkTriangleFilter.h"
#include "vtkQuadricDecimation.h"
namespace udg {

AreaSpline::AreaSpline(){
m_area=0;
m_spline=0;
m_factor1=0;
m_factor2=0;
}

AreaSpline::~AreaSpline(){}


void AreaSpline::CalculateArea(vtkSplineWidget *spline, int vista){
/*
m_spline=spline;
vtkPolyData *poly=vtkPolyData::New();
vtkPolyData *newpoly=vtkPolyData::New();
m_spline->GetPolyData(poly);

vtkTriangleFilter *tri=vtkTriangleFilter::New();
vtkQuadricDecimation *qua=vtkQuadricDecimation::New();
//vtkTriangleStrip *Strip=vtkTriangleStrip::New();

tri->SetInput(poly);

//tri->SetInput(Strip);
tri->Update();

//m_area=Strip->GetNumberOfFaces ();
//qua->SetInput(Strip->GetOutput());
//qua->Update();
//newpoly=qua->GetOutput();

int i = newpoly->GetNumberOfPolys();
m_area=i;*/
m_spline=spline;
double pinici[3];
double pseg[3];
int points=m_spline->GetNumberOfHandles();
m_area=0;
m_factor1=0;
m_factor2=0;
switch (vista){

    case 0:///Axial

        for(int k=0;k<points-1;k++){

            m_spline->GetHandlePosition(k,pinici);
            m_spline->GetHandlePosition(k+1,pseg);
            m_factor1=m_factor1+(pinici[0]*pseg[1]);
            m_factor2=m_factor2+(pinici[1]*pseg[0]);
            //m_area =m_area + ((pinici[0]*pseg[1]) - (pinici[1]*pseg[0]))/2 ;

        }

        m_spline->GetHandlePosition(0,pseg);
        m_spline->GetHandlePosition(points-1,pinici);
        m_factor1=m_factor1+(pinici[0]*pseg[1]);
        m_factor2=m_factor2+(pinici[1]*pseg[0]);
        //m_area =m_area + (pinici[0]*pseg[1] - pinici[1]*pseg[0])/2 ;
        m_area=(m_factor1-m_factor2)/2;
        //m_area = m_area/2;
    break;
    case 1:///Sagital

        for(int k=0;k<points-1;k++){

            m_spline->GetHandlePosition(k,pinici);
            m_spline->GetHandlePosition(k+1,pseg);
            m_factor1=m_factor1+(pinici[0]*pseg[2]);
            m_factor2=m_factor2+(pinici[2]*pseg[0]);


        /*    m_spline->GetHandlePosition(k,pinici);
            m_spline->GetHandlePosition(k+1,pseg);

            m_area =m_area + ((pinici[0]*pseg[2]) - (pinici[2]*pseg[0]))/2 ;*/

        }

        /*m_spline->GetHandlePosition(0,pinici);
        m_spline->GetHandlePosition(points-1,pseg);
        m_area =m_area + ((pinici[0]*pseg[2]) - (pinici[2]*pseg[0]))/2 ;
        //m_area = m_area/2;*/

        m_spline->GetHandlePosition(0,pseg);
        m_spline->GetHandlePosition(points-1,pinici);
        m_factor1=m_factor1+(pinici[0]*pseg[2]);
        m_factor2=m_factor2+(pinici[2]*pseg[0]);
        //m_area =m_area + (pinici[0]*pseg[1] - pinici[1]*pseg[0])/2 ;
        m_area=(m_factor1-m_factor2)/2;


    break;
    case 2:///Coronal

        for(int k=0;k<points-1;k++){

            m_spline->GetHandlePosition(k,pinici);
            m_spline->GetHandlePosition(k+1,pseg);
            m_factor1=m_factor1+(pinici[1]*pseg[2]);
            m_factor2=m_factor2+(pinici[2]*pseg[1]);


            /*m_spline->GetHandlePosition(k,pinici);
            m_spline->GetHandlePosition(k+1,pseg);

            m_area =m_area + ((pinici[1]*pseg[2]) - (pinici[2]*pseg[1]))/2 ;*/

        }

        m_spline->GetHandlePosition(0,pseg);
        m_spline->GetHandlePosition(points-1,pinici);
        m_factor1=m_factor1+(pinici[1]*pseg[2]);
        m_factor2=m_factor2+(pinici[2]*pseg[1]);
        //m_area =m_area + (pinici[0]*pseg[1] - pinici[1]*pseg[0])/2 ;
        m_area=(m_factor1-m_factor2)/2;

        /*m_spline->GetHandlePosition(0,pinici);
        m_spline->GetHandlePosition(points-1,pseg);
        m_area =m_area + ((pinici[1]*pseg[2]) - (pinici[2]*pseg[1]))/2 ;
        //m_area = m_area/2;*/


    break;
    }


m_spline=0;





}


double AreaSpline::getArea(){

return m_area;
}

}
