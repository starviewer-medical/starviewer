/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "roitooldata.h"
#include <QString>
#include "distance.h"
#include "q2dviewer.h"
#include "logging.h"

namespace udg {

ROIToolData::ROIToolData( ROIToolData::PointsList points, QString typeOfROI )
{
    setPoints( points );
    m_typeOfROI = typeOfROI;
}

ROIToolData::~ROIToolData()
{}
    
void ROIToolData::setPoints( ROIToolData::PointsList points )
{
   for ( int i = 0; i < points.count(); i++ ) 
    {
        m_points << points[i];
    }
    
    emit listOfPointsChanged();
}

void ROIToolData::calculateTextArea( double *topLeft, double *bottomRight, int view )
{
   double bottomLeft[3], topRight[3];
    
    switch( view )
    {
        case Q2DViewer::Axial:
            bottomLeft[0] = topLeft[0];
            bottomLeft[1] = bottomRight[1];
            bottomLeft[2] = topLeft[2];
            topRight[0] = bottomRight[0];
            topRight[1] = topLeft[1];
            topRight[2] = topLeft[2];
            break;
        case Q2DViewer::Sagittal:
            bottomLeft[0] = topLeft[1];
            bottomLeft[1] = bottomRight[2];
            bottomLeft[2] = topLeft[0];
            topRight[0] = bottomRight[1];
            topRight[1] = topLeft[2];
            topRight[2] = topLeft[0];
            break;
            
        case Q2DViewer::Coronal:
            bottomLeft[0] = topLeft[0];
            bottomLeft[1] = bottomRight[2];
            bottomLeft[2] = topLeft[1];
            topRight[0] = bottomRight[0];
            topRight[1] = topLeft[2];
            topRight[2] = topLeft[1];
            break;
         default:
            DEBUG_LOG( "El Q2DViewer no té assignada cap de les 3 vistes possibles!?" );
            break;
    }
    
    Distance heightD(bottomLeft, topLeft);
    Distance widthD(bottomRight, topRight);

    double height = heightD.getDistance3D(); 
    double width = widthD.getDistance3D();
    
    //tant l'àrea del cercle com de l'el·lipse es calculen igual, ja que en el cas del cercle height = width
    double area = M_PI * (height/2) * (width/2);

    m_text = QString( "%1 mm2" ).arg( area, 0, 'f',  2);
    
    m_textPosition[0] = ( topLeft[0] + bottomRight[0] ) / 2;
    m_textPosition[1] = ( topLeft[1] + bottomRight[1] ) / 2;
    m_textPosition[2] = ( topLeft[2] + bottomRight[2] ) / 2;
    
    emit roiTextChanged();
}
    
};  // end namespace udg

