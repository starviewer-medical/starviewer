/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCONTOURNTOOL_H
#define UDGCONTOURNTOOL_H

#include "tool.h"
#include "vtkSplineWidget.h"
#include "itkImage.h"


namespace udg {

class Q2DViewer;
class Volume;
class DeterminateContour;
class AreaSpline;

/**
Tool que serveix per posar llavors en un visor 2D

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ContournTool : public Tool
{
Q_OBJECT
public:
    /// estats de la tool
    enum { NONE , CONTOURING };
    enum ViewType{ Axial , Sagital , Coronal };
    ContournTool(Q2DViewer *viewer,Volume *seg, QObject *parent = 0 );

    ~ContournTool();

    void handleEvent( unsigned long eventID );

    double getLength( );

signals:

    void setLenght(double);

private:
    
    typedef itk::Image<signed int,3>     ImageType;
    Q2DViewer *m_2DViewer;
    Volume *m_seg;
    int m_seedSlice;
    int m_lastView;
    DeterminateContour *m_con;
    DeterminateContour *m_con2;
    DeterminateContour *m_con3;
    ///Actor que dibuixa la llavor
    vtkSplineWidget*  m_spline;
    vtkSplineWidget*  m_splineY;
    vtkSplineWidget*  m_splineZ;
    ViewType vista;
    AreaSpline* m_areaSpline;    

    bool m_calculat;
    float xt;
    float yt;
    float zt;

   
/// \TODO potser aquests m�todes slots passen a ser p�blics
private slots:
   
    void setContourn( );
   
    void doContouring();

    void sliceChanged( int s );

    void viewChanged(int s);

};

}

#endif
