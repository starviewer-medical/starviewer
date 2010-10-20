/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPOLYLINETEMPORALROITOOL_H
#define UDGPOLYLINETEMPORALROITOOL_H

#include "tool.h"

#include <QPointer>
#include <itkImage.h>

namespace udg {

class Q2DViewer;
class DrawerPolyline;
class DrawerPrimitive;
class PolylineROIOutliner;
class PolylineTemporalROIToolRepresentation;

//class PolylineTemporalROIToolData;

/**
    Tool per dibuixar ROIS polilinies i calcular mitjanes de series temporals
*/
class PolylineTemporalROITool : public Tool {
Q_OBJECT
public:
    PolylineTemporalROITool(QViewer *viewer, QObject *parent = 0);
    ~PolylineTemporalROITool();

	void handleEvent(long unsigned eventID);

	//Declaració de tipus
	//typedef PolylineTemporalROIToolData::TemporalImageType TemporalImageType;
/*
    typedef itk::Image<double, 4> TemporalImageType;

    /// Retorna les dades pròpies de la seed
    ToolData *getToolData() const;

    /// Assigna les dades pròpies de l'eina (persistent data)
    void setToolData(ToolData * data);
*/

private slots:
    /// Indica que l'outliner ha acabat de dibuixar una primitiva
    void outlinerFinished(DrawerPrimitive *primitive);

    /// metode que es crida quan s'acaba de definir la roi
    //void start();
private:
/*
	// metode que converteix el volum de l'input (que té fases) en imatge itk temporal
	void convertInputImageToTemporalImage();

    /// metode per calcular la mitjana temporal de la regio del polyline
    double computeTemporalMean();

	/// metode que retorna la serie temporal de mida size en una determinada coordenada 
	QVector<double> getGraySerie(double *coords, int size);
*/
     
private:
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Outliner que s'utilitza per dibuixar el polígon
    PolylineROIOutliner *m_polylineROIOutliner;

    /// ToolRepresentation que s'utilitza
    PolylineTemporalROIToolRepresentation *m_polylineTemporalROIToolRepresentation;

    /// Polígon dibuixat corresponent a la ROI
    QPointer<DrawerPolyline> m_polyline;

    /// Dades específiques de la tool
    //PolylineTemporalROIToolData *m_myData;
};

}

#endif
