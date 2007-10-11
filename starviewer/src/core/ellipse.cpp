/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "ellipse.h"

namespace udg {

Ellipse::Ellipse( double minorRadius[3], double majorRadius[3], double center[3], QString behavior ) : Polygon()
{
    disableBackground();
    m_behavior = behavior;
    
    for( int i = 0; i < 3; i++ )
    {
        m_minorRadius[i] = minorRadius[i];
        m_majorRadius[i] = majorRadius[i];
        m_center[i] = center[i];
    }
    discontinuousOff();
    computeRectanglePoints();
}

Ellipse::Ellipse( double topLeft[3], double bottomRight[3], QString behavior ) : Polygon()
{
    disableBackground();
    m_behavior = behavior;
    
    for( int i = 0; i < 3; i++ )
    {
        m_topLeft[i] = topLeft[i];
        m_bottomRight[i] = bottomRight[i];
    }
    discontinuousOff();
    computeCenterAndRadius();
}

Ellipse::~Ellipse()
{}

void Ellipse::setMinorRadius( double p1[3] )
{
    for( int i = 0; i < 3; i++ )
        m_minorRadius[i] = p1[i];
    
    computeRectanglePoints();
}

void Ellipse::setMajorRadius( double p1[3] )
{
    for( int i = 0; i < 3; i++ )
        m_majorRadius[i] = p1[i];
    
    computeRectanglePoints();
}

void Ellipse::setCenter( double p1[3] )
{
    for( int i = 0; i < 3; i++ )
        m_center[i] = p1[i];
    
    computeRectanglePoints();
}

void Ellipse::setTopLeftPoint( double point[3] )
{
    for( int i = 0; i < 3; i++ )
        m_topLeft[i] = point[i];
    
    computeCenterAndRadius();
}
    
void Ellipse::setBottomRightPoint( double point[3] )
{
    for( int i = 0; i < 3; i++ )
        m_bottomRight[i] = point[i];
    
    computeCenterAndRadius();
}

void Ellipse::convertToCircle()
{
    setMinorRadius( getMajorRadius() );
    m_behavior = "Circle";
    
    computeRectanglePoints();
}

void Ellipse::computeCenterAndRadius()
{
    m_center[0] = ( m_topLeft[0] + m_bottomRight[0] ) / 2;
    m_center[1] = ( m_topLeft[1] + m_bottomRight[1] ) / 2;
    m_center[2] = m_bottomRight[2]; //qualsevol z dels punts
     
    m_minorRadius[0] = m_center[0];
    m_minorRadius[1] = m_topLeft[1];
    m_minorRadius[2] = m_topLeft[2];
     
    m_majorRadius[0] = m_bottomRight[0];
    m_majorRadius[1] = m_center[1];
    m_majorRadius[2] = m_bottomRight[2];
}

void Ellipse::computeRectanglePoints()
{
    double diference = m_majorRadius[0] - m_center[0]; 
    m_topLeft[0] = m_center[0] - diference; 
    m_topLeft[1] = m_minorRadius[1];
    m_topLeft[2] = m_minorRadius[2];
  
    diference = m_minorRadius[1] - m_center[1]; 
    m_bottomRight[0] = m_majorRadius[0];
    m_bottomRight[1] = m_center[1] - diference;
    m_bottomRight[2] = m_majorRadius[2];
}

}
