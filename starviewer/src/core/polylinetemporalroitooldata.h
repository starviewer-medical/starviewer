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

#ifndef UDGPOLYLINETEMPORALROITOOLDATA_H
#define UDGPOLYLINETEMPORALROITOOLDATA_H

#include "tooldata.h"

#include <itkImage.h>

#include <QVector>

namespace udg {

class PolylineTemporalROIToolData : public ToolData {
Q_OBJECT
public:
    PolylineTemporalROIToolData(QObject *parent = 0);
    ~PolylineTemporalROIToolData();

    /// Declaració de tipus
    // typedef PolylineTemporalROITool::TemporalImageType TemporalImageType;
    typedef itk::Image<double, 4> TemporalImageType;

    /// Li passem la imatge d'on ha de treure les dades
    void setTemporalImage(TemporalImageType::Pointer a)
    {
        std::cout<<"Set TemporalImage!!"<<std::endl;
        m_temporalImage = a;m_temporalImageHasBeenDefined = true;
    }
    TemporalImageType::Pointer getTemporalImage()
    {
        return m_temporalImage;
    }

    bool temporalImageHasBeenDefined()
    {
        return m_temporalImageHasBeenDefined;
    }

    void setMeanVector(QVector<double> m);
    QVector<double> getMeanVector()
    {
        return m_mean;
    }

signals:
    /// S'emet quan s'assigna un nou vector de dades
    void dataChanged();

private:
    /// Vector on hi desarem la mitjana temporal
    QVector<double> m_mean;

    /// Imatge amb les dades temporals
    TemporalImageType::Pointer m_temporalImage;
    bool m_temporalImageHasBeenDefined;

};

}

#endif
