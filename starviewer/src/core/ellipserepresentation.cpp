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

void EllipseRepresentation::calculateTextAndPositionOfEllipse( int view )
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
// //     m_ellipse->setPoints( points );
}

void EllipseRepresentation::refreshText( int view )
{ 
//     //primer calculem el text de la distància i la seva posició
//     calculateTextAndPositionOfROI( view );
//     
//     //refresquem el text
//     m_text->refreshText();
}

};  // end namespace udg

