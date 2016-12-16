/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGPOLYLINETEMPORALROITOOL_H
#define UDGPOLYLINETEMPORALROITOOL_H

#include "polylineroitool.h"

#include <itkImage.h>

namespace udg {
class PolylineTemporalROIToolData;

/**
    Tool per dibuixar ROIS polilinies i calcular mitjanes de series temporals
  */
class PolylineTemporalROITool : public PolylineROITool {
Q_OBJECT
public:
    PolylineTemporalROITool(QViewer *viewer, QObject *parent = 0);

    ~PolylineTemporalROITool();

    // Declaració de tipus
    // typedef PolylineTemporalROIToolData::TemporalImageType TemporalImageType;
    typedef itk::Image<double, 4> TemporalImageType;

    /// Retorna les dades pròpies de la seed
    ToolData* getToolData() const;

    /// Assigna les dades pròpies de l'eina (persistent data)
    void setToolData(ToolData *data);

private slots:
    /// Metode que es crida quan s'acaba de definir la roi
    void start();

private:
    /// Metode per calcular la mitjana temporal de la regio del polyline
    double computeTemporalMean();

    /// Metode que retorna la serie temporal de mida size en una determinada coordenada
    QVector<double> getGraySerie(const Vector3 &coords, int size);

private:
    /// Dades específiques de la tool
    PolylineTemporalROIToolData *m_myData;
};

}

#endif
