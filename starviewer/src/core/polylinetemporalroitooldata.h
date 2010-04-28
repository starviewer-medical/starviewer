/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPOLYLINETEMPORALROITOOLDATA_H
#define UDGPOLYLINETEMPORALROITOOLDATA_H

#include "tooldata.h"

#include <itkImage.h>

#include <QVector>

namespace udg {

/**
    @author Laboratori de Gràfics i Imatge  ( GILab ) <vismed@ima.udg.es>
*/
class PolylineTemporalROIToolData : public ToolData {
Q_OBJECT
public:
    PolylineTemporalROIToolData(QObject *parent = 0);
    ~PolylineTemporalROIToolData();

    /// Declaració de tipus
    // typedef PolylineTemporalROITool::TemporalImageType TemporalImageType;
    typedef itk::Image<double, 4> TemporalImageType;

    /// Li passem la imatge d'on ha de treure les dades
    void setTemporalImage(TemporalImageType::Pointer a) {std::cout<<"Set TemporalImage!!"<<std::endl; 
														m_temporalImage = a;m_temporalImageHasBeenDefined = true;}
    TemporalImageType::Pointer getTemporalImage(){return m_temporalImage;}

    bool temporalImageHasBeenDefined(){return m_temporalImageHasBeenDefined;}

    void setMeanVector( QVector<double> m );
    QVector<double> getMeanVector(){return m_mean;}

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
