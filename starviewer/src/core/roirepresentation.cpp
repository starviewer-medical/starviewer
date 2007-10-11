/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "roirepresentation.h"
#include "roitooldata.h"
#include "q2dviewer.h"
#include "logging.h"

namespace udg {

ROIRepresentation::ROIRepresentation( ROIToolData *dtd ) : Representation()
{ 
    m_polygon = new Polygon( dtd->getPoints() );
    m_text = new Text( dtd->getTextPosition(), dtd->getROIText() );
    
    m_roiToolData = dtd;
    
    //creem les connexions entre aquest objecte i els seu atribut ROIToolData
    connect( m_roiToolData, SIGNAL( roiTextChanged() ), this , SLOT( updateText() ) );
    connect( m_roiToolData, SIGNAL( listOfPointsChanged() ), this , SLOT( updatePolygonPoints() ) );
}
    
ROIRepresentation::~ROIRepresentation()
{
    delete m_text;
    delete m_polygon;
    delete m_roiToolData;
}    

void ROIRepresentation::setText( Text *text )
{
    m_text = text;
}

void ROIRepresentation::setPolygon( Polygon *polygon )
{
    m_polygon = polygon;
}

void ROIRepresentation::setROIToolData( ROIToolData *dtd )
{
    m_roiToolData = dtd;
}
    
Text* ROIRepresentation::getText()
{
    return( m_text );   
}

Polygon* ROIRepresentation::getPolygon()
{
    return( m_polygon );
} 

ROIToolData* ROIRepresentation::getROIToolData()
{
    return( m_roiToolData );
} 

void ROIRepresentation::updatePolygonPoints()
{
    m_polygon->setPoints( m_roiToolData->getPoints() );
}

void ROIRepresentation::updateText()
{
    m_text->setText( m_roiToolData->getROIText() );
    m_text->setAttatchmentPoint( m_roiToolData->getTextPosition() );
}

void ROIRepresentation::calculateTextAndPositionOfROI( int view )
{
//     m_roiToolData->calculateROI();
//     m_text->setText( m_roiToolData->getROIText() );
//     m_text->setAttatchmentPoint( m_roiToolData->getTextPosition() );    
// 
// //     //ara que tenim la posició del text podem calcular les coordenades del fons del text
// //     QList<double* > points;
// //     double *attachPoint = m_text->getAttatchmentPoint();
// //     double *attachPoint1 = new double[3];
// //     double *attachPoint2 = new double[3];
// //     double *attachPoint3 = new double[3];
// //     double *attachPoint4 = new double[3];
// //     
// //     for ( int i = 0; i < 3; i++ )
// //     { 
// //         attachPoint1[i] = attachPoint[i];
// //         attachPoint2[i] = attachPoint[i];
// //         attachPoint3[i] = attachPoint[i];
// //         attachPoint4[i] = attachPoint[i];
// //     }
// //     
// //     switch( view ){
// //     case Q2DViewer::Axial:
// //         attachPoint1[0] -= 15;
// //         attachPoint1[1] -= 2;
// //     
// //         attachPoint2[0] += 15;
// //         attachPoint2[1] -= 2;
// //     
// //         attachPoint3[0] += 15;
// //         attachPoint3[1] += 3;
// //     
// //         attachPoint4[0] -= 15;
// //         attachPoint4[1] += 3;
// //     break;
// //     case Q2DViewer::Sagittal:
// //         attachPoint1[1] -= 12;
// //         attachPoint1[2] -= 2.5;
// //     
// //         attachPoint2[1] += 12;
// //         attachPoint2[2] -= 2.5;
// //     
// //         attachPoint3[1] += 12;
// //         attachPoint3[2] += 2.5;
// //     
// //         attachPoint4[1] -= 12;
// //         attachPoint4[2] += 2.5;
// //     break;
// //     case Q2DViewer::Coronal:
// //         attachPoint1[0] -= 12;
// //         attachPoint1[2] -= 3;
// //     
// //         attachPoint2[0] += 12;
// //         attachPoint2[2] -= 3;
// //     
// //         attachPoint3[0] += 12;
// //         attachPoint3[2] += 2;
// //     
// //         attachPoint4[0] -= 12;
// //         attachPoint4[2] += 2;
// //     break;
// //     default:
// //         ERROR_LOG( "Vista no esperada!!!" );
// //     break;
// //     }
// // 
// //     points << attachPoint1;
// //     points << attachPoint2;
// //     points << attachPoint3;
// //     points << attachPoint4;
// // 
// //     m_polygon->setPoints( points );
}

void ROIRepresentation::refreshText( int view )
{ 
//     //primer calculem el text de la distància i la seva posició
//     calculateTextAndPositionOfROI( view );
//     
//     //refresquem el text
//     m_text->refreshText();
}

};  // end namespace udg

