/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "distancerepresentation.h"
#include "olddistancetooldata.h"
#include "q2dviewer.h"
#include "logging.h"

namespace udg {

DistanceRepresentation::DistanceRepresentation( OldDistanceToolData *dtd ) : Representation()
{
    m_line = new Line( dtd->getFirstPoint(), dtd->getSecondPoint() );
    m_text = new Text( dtd->getTextPosition(), dtd->getDistanceText() );
//     m_polygon = new Polygon();

    m_distanceToolData = dtd;

    //creem les connexions entre aquest objecte i els seu atribut OldDistanceToolData
    connect( m_distanceToolData, SIGNAL( firstPointChanged() ), this , SLOT( updateFirstPointLine() ) );
    connect( m_distanceToolData, SIGNAL( secondPointChanged() ), this , SLOT( updateSecondPointLine() ) );
    connect( m_distanceToolData, SIGNAL( distanceTextChanged() ), this , SLOT( updateText() ) );
}

DistanceRepresentation::~DistanceRepresentation()
{
    delete m_line;
    delete m_text;
//     delete m_polygon;
    delete m_distanceToolData;
}

void DistanceRepresentation::setText( Text *text )
{
    m_text = text;
}

void DistanceRepresentation::setLine( Line *line )
{
    m_line = line;
    m_distanceToolData->setFirstPoint( m_line->getFirstPoint() );
    m_distanceToolData->setSecondPoint( m_line->getSecondPoint() );
}

// void DistanceRepresentation::setPolygon( Polygon *polygon )
// {
//     m_polygon = polygon;
// }

void DistanceRepresentation::setDistanceToolData( OldDistanceToolData *dtd )
{
    m_distanceToolData = dtd;
}

Text* DistanceRepresentation::getText()
{
    return( m_text );
}

Line* DistanceRepresentation::getLine()
{
    return( m_line );
}

/*Polygon* DistanceRepresentation::getPolygon()
{
    return( m_polygon );
}*/

OldDistanceToolData* DistanceRepresentation::getDistanceToolData()
{
    return( m_distanceToolData );
}

void DistanceRepresentation::updateFirstPointLine()
{
    m_line->setFirstPoint( m_distanceToolData->getFirstPoint() );
}

void DistanceRepresentation::updateSecondPointLine()
{
    m_line->setSecondPoint( m_distanceToolData->getSecondPoint() );
}

void DistanceRepresentation::updateText()
{
    m_text->setText( m_distanceToolData->getDistanceText() );
    m_text->setAttatchmentPoint( m_distanceToolData->getTextPosition() );
}

void DistanceRepresentation::calculateTextAndPositionOfDistance( int view )
{
    m_distanceToolData->calculateDistance();
    m_text->setText( m_distanceToolData->getDistanceText() );
    m_text->setAttatchmentPoint( m_distanceToolData->getTextPosition() );

//     //ara que tenim la posició del text podem calcular les coordenades del fons del text
//     QList<double* > points;
//     double *attachPoint = m_text->getAttatchmentPoint();
//     double *attachPoint1 = new double[3];
//     double *attachPoint2 = new double[3];
//     double *attachPoint3 = new double[3];
//     double *attachPoint4 = new double[3];
//
//     for ( int i = 0; i < 3; i++ )
//     {
//         attachPoint1[i] = attachPoint[i];
//         attachPoint2[i] = attachPoint[i];
//         attachPoint3[i] = attachPoint[i];
//         attachPoint4[i] = attachPoint[i];
//     }
//
//     switch( view ){
//     case Q2DViewer::Axial:
//         attachPoint1[0] -= 15;
//         attachPoint1[1] -= 2;
//
//         attachPoint2[0] += 15;
//         attachPoint2[1] -= 2;
//
//         attachPoint3[0] += 15;
//         attachPoint3[1] += 3;
//
//         attachPoint4[0] -= 15;
//         attachPoint4[1] += 3;
//     break;
//     case Q2DViewer::Sagittal:
//         attachPoint1[1] -= 12;
//         attachPoint1[2] -= 2.5;
//
//         attachPoint2[1] += 12;
//         attachPoint2[2] -= 2.5;
//
//         attachPoint3[1] += 12;
//         attachPoint3[2] += 2.5;
//
//         attachPoint4[1] -= 12;
//         attachPoint4[2] += 2.5;
//     break;
//     case Q2DViewer::Coronal:
//         attachPoint1[0] -= 12;
//         attachPoint1[2] -= 3;
//
//         attachPoint2[0] += 12;
//         attachPoint2[2] -= 3;
//
//         attachPoint3[0] += 12;
//         attachPoint3[2] += 2;
//
//         attachPoint4[0] -= 12;
//         attachPoint4[2] += 2;
//     break;
//     default:
//         ERROR_LOG( "Vista no esperada!!!" );
//     break;
//     }
//
//     points << attachPoint1;
//     points << attachPoint2;
//     points << attachPoint3;
//     points << attachPoint4;
//
//     m_polygon->setPoints( points );
}

void DistanceRepresentation::refreshText( int view )
{
    //primer calculem el text de la distància i la seva posició
    calculateTextAndPositionOfDistance( view );

    //refresquem el text
    m_text->refreshText();

    //refresquem el fons del text
//     m_polygon->refreshPolygon();
}

};  // end namespace udg

