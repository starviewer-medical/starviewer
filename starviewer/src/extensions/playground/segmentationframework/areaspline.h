/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef UDGAREASPLINE_H
#define UDGAREASPLINE_H

class vtkSplineWidget;

namespace udg {


class AreaSpline{


private:

double m_area;
double m_factor1,m_factor2;
vtkSplineWidget *m_spline;

enum ViewType{ Axial , Sagital , Coronal };

public:
    AreaSpline();

    ~AreaSpline();

    void CalculateArea(vtkSplineWidget *spline,int vista );
    double getArea();

};

};

#endif
