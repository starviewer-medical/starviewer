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
    //typedef PolylineTemporalROIToolData::TemporalImageType TemporalImageType;
    typedef itk::Image<double, 4> TemporalImageType;

    /// Retorna les dades pròpies de la seed
    ToolData *getToolData() const;

    /// Assigna les dades pròpies de l'eina (persistent data)
    void setToolData(ToolData *data);

private slots:

    /// Metode que es crida quan s'acaba de definir la roi
    void start();

private:

    // Metode que converteix el volum de l'input (que té fases) en imatge itk temporal
    void convertInputImageToTemporalImage();

    /// Metode per calcular la mitjana temporal de la regio del polyline
    double computeTemporalMean();

    /// Metode que retorna la serie temporal de mida size en una determinada coordenada
    QVector<double> getGraySerie(double *coords, int size);

private:

    /// Dades específiques de la tool
    PolylineTemporalROIToolData *m_myData;
};

}

#endif
