/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPOLYLINETEMPORALROITOOL_H
#define UDGPOLYLINETEMPORALROITOOL_H

#include "polylineroitool.h"

#include <itkImage.h>

namespace udg {
class PolylineTemporalROIToolData;

/**
Tool per dibuixar ROIS polilinies i calcular mitjanes de series temporals

	@author Laboratori de Gràfics i Imatge  ( GILab ) <vismed@ima.udg.es>
*/
class PolylineTemporalROITool : public PolylineROITool
{
Q_OBJECT
public:
    PolylineTemporalROITool( QViewer *viewer, QObject *parent = 0 );

    ~PolylineTemporalROITool();

	//Declaració de tipus
	//typedef PolylineTemporalROIToolData::TemporalImageType TemporalImageType;
    typedef itk::Image<double, 4> TemporalImageType;

    /// Retorna les dades pròpies de la seed
    ToolData *getToolData() const;

    /// Assigna les dades pròpies de l'eina (persistent data)
    void setToolData(ToolData * data);

private slots:

    /// metode que es crida quan s'acaba de definir la roi
    void start();

private:

	// metode que converteix el volum de l'input (que té fases) en imatge itk temporal
	void convertInputImageToTemporalImage();

    /// metode per calcular la mitjana temporal de la regio del polyline
    double computeTemporalMean();

	/// metode que retorna la serie temporal de mida size en una determinada coordenada 
	QVector<double> getGraySerie( double *coords, int size );
     
private:

	/// Dades específiques de la tool
    PolylineTemporalROIToolData *m_myData;
};

}

#endif
