/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "ellipserepresentation.h"
#include "roitooldata.h"
#include "q2dviewer.h"
#include "logging.h"

namespace udg {

EllipseRepresentation::EllipseRepresentation( ROIToolData *rtd ) : Representation()
{ 
    m_ellipse = new Ellipse( rtd->getPoints()[0], rtd->getPoints()[1], rtd->getTypeOfROI() );
    m_text = new Text( rtd->getTextPosition(), rtd->getROIText() );
    
    m_roiToolData = rtd;
    
    //creem les connexions entre aquest objecte i els seu atribut ROIToolData
    connect( m_roiToolData, SIGNAL( roiTextChanged() ), this , SLOT( updateText() ) );
    connect( m_roiToolData, SIGNAL( listOfPointsChanged() ), this , SLOT( updateEllipsePoints() ) );
}
    
EllipseRepresentation::~EllipseRepresentation()
{
    delete m_text;
    delete m_ellipse;
    delete m_roiToolData;
}    

void EllipseRepresentation::setText( Text *text )
{
    m_text = text;
}

void EllipseRepresentation::setEllipse( Ellipse *ellipse )
{
    m_ellipse = ellipse;
}

void EllipseRepresentation::setROIToolData( ROIToolData *rtd )
{
    m_roiToolData = rtd;
}
    
Text* EllipseRepresentation::getText()
{
    return( m_text );   
}

Ellipse* EllipseRepresentation::getEllipse()
{
    return( m_ellipse );
} 

ROIToolData* EllipseRepresentation::getROIToolData()
{
    return( m_roiToolData );
} 

void EllipseRepresentation::updateEllipsePoints()
{
    m_ellipse->setTopLeftPoint( m_roiToolData->getPoints()[0] );
    m_ellipse->setBottomRightPoint( m_roiToolData->getPoints()[0] );
}

void EllipseRepresentation::updateText()
{
    m_text->setText( m_roiToolData->getROIText() );
    m_text->setAttatchmentPoint( m_roiToolData->getTextPosition() );
}

void EllipseRepresentation::refreshText( double *topLeft, double *bottomRight, int view )
{ 
    m_roiToolData->calculateTextArea( topLeft, bottomRight, view );
    m_text->setText( m_roiToolData->getROIText() );
    m_text->setAttatchmentPoint( m_roiToolData->getTextPosition() );    
}

void EllipseRepresentation::refreshEllipse()
{ 
    m_ellipse->refresh(); 
}

};  // end namespace udg

