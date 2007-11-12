/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGROITOOL_H
#define UDGROITOOL_H

#include "oldtool.h"

#include <QColor>
#include <QMultiMap>
#include <QList>
#include <vtkPolyData.h>
#include <vtkActor2D.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkCaptionActor2D.h>
#include <vtkCoordinate.h>

class vtkRenderWindowInteractor;
class vtkPoints;
class vtkCellArray;
class vtkLineSource;
class vtkPropPicker;

namespace udg {

class Q2DViewer;
class EllipseRepresentation;

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class ROITool : public Tool
{
    Q_OBJECT
public:

    ///rois que podem representar
    enum ROIType{ RECTANGLE , CIRCLE , ELLIPSE , POLYLINE , NO_SPECIFIED };

    /// estats de la tool
    enum { STOPPED, ANNOTATION_STARTED, SIMULATING_ANNOTATION };

    ROITool( Q2DViewer *viewer , QObject *parent = 0 , const char *name = 0 );

    ~ROITool();

    /// tracta els events que succeeixen dins de l'escena
    void handleEvent( unsigned long eventID );

    ///anota el següent punt de la polilínia
    void annotateNextPolylinePoint();

    ///ens permet simular el darrer segment de la polilínia
    void simulationOfNewPolyLinePoint( double *point1, double *point2 );

    ///crea l'etiqueta amb la informació de la ROI
//     vtkCaptionActor2D* createCaption( double *point, double area/*, double mean*/ );

    ///ens retorna el valor de gris d'un píxel determinat amb coordenades de món.
//     double getGrayValue( double* );

private slots:
    /// Comença l'anotació de la roi
    void startROIAnnotation();

    /// simula la nova roi
    void doROISimulation();

    /// finalitza l'anotació de la ROI
    void stopROIAnnotation();

    ///respon als events de teclat
    void answerToKeyEvent();

private:

    ///objecte per a crear rois noves
    EllipseRepresentation *m_ellipseRepresentation;

    /// punts per controlar l'anotació de les rois
    double m_startPosition[3], m_currentPosition[3];

    ///calcula la mitjana dels nivells de gris que tenen els vòxels que estan dins de la ROI
//     double computeMean( ROIAssembly* );

    /// Ressalta l'actor 2D que estigui més a prop de la posició actual del cursor
//     void highlightNearestROI();

    ///ens retorna la ROIAssembly més propera al punt que li passem per paràmetre
//     ROIAssembly* getNearestROI( double[3] );

    /// Pinta una ROI del color que li diem
//     void setColor( ROIAssembly* roi, QColor color );

    ///ens determina el tipus de ROI que estem utilitzant
    ROIType m_ROIType;

    ///visor sobre el que es programa la tool
    Q2DViewer *m_2DViewer;
};

}
#endif
