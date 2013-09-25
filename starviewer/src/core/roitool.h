#ifndef UDGROITOOL_H
#define UDGROITOOL_H

#include "measurementtool.h"
#include "volume.h"
#include "line3d.h"
#include <QPointer>

namespace udg {

class DrawerPolygon;
class DrawerText;

/**
    Tool pare per totes aquelles tools destinades a crear ROIs.
    S'encarrega de proporcionar els mètodes necessaris per calcular
    les dades estadístiques relacionades amb la ROI (àrea, mitjana, desviació estàndar).
    La gestió dels events i de com es dibuixa la forma de la ROI queda delegada en les
    tools filles. La forma final de la tool ha de quedar dibuixada amb el membre m_roiPolygon.
  */
class ROITool : public MeasurementTool {
Q_OBJECT
public:
    ROITool(QViewer *viewer, QObject *parent = 0);
    ~ROITool();

    virtual void handleEvent(long unsigned eventID) = 0;

protected:
    MeasureComputer* getMeasureComputer();
    
    /// Mètode per escriure a pantalla les dades calculades.
    void printData();

    /// Mètode que genera el text a mostrar
    QString getAnnotation();
    /// Mètode per assignar propietats de posició al text
    virtual void setTextPosition(DrawerText *text);

protected:
    /// Polígon que defineix la ROI
    QPointer<DrawerPolygon> m_roiPolygon;

    /// Controlarà si cal o no recalcular les dades estadístiques
    /// Quan modifiquem la roi (afegint punts, creant una de nova, etc) el valor serà true
    /// Un cop s'hagin calculat, serà false fins que no es torni a modificar
    /// Per defecte el valor és true
    bool m_hasToComputeStatisticsData;

    /// Mitjana de valors de la ROI
    double m_mean;

    /// Desviació estàndar de la ROI
    double m_standardDeviation;

private:
    /// Methods to compute statistics data, such as mean and standard deviation, upon the contained voxels on the ROI
    double computeMean(const QList<double> &grayValues);
    double computeStandardDeviation(const QList<double> &grayValues);
    
    /// Calcula les dades estadístiques de la ROI.
    /// Serà necessari cridar aquest mètode abans si volem obtenir la mitjana i/o la desviació estàndar
    void computeStatisticsData();

    /// Crea una còpia de m_roiPolygon, amb la diferència que aquesta tindrà el mateix valor de profunditat que la llesca actual
    /// Això ho necessitarem a l'hora de calcular els valors de vòxel, ja que la coordenada de profunditat de les annotacions
    /// és diferent a les llesques en sí, ja que estan en dos plans lleugerament separats
    DrawerPolygon* createProjectedROIPolygon();

    /// Returns a list with the indices of the corresponding segments of the given list which crosses the given height, that is, those segments
    /// which its initial and end point are between the specified heigh on the heightIndex
    QList<int> getIndexOfSegmentsCrossingAtHeight(const QList<Line3D> &segments, double height, int heightIndex);

    /// Gets the points that intersect with the subset of segments (pointed by the list of indices) with the given sweepLine and orders them by sortIndex
    QList<double*> getIntersectionPoints(const QList<Line3D> &polygonSegments, const QList<int> &indexListOfSegmentsToIntersect,  const Line3D &sweepLine, int sortIndex);

    /// Adds the voxels that are in the path of the intersection points to the given list
    void addVoxelsFromIntersections(const QList<double*> &intersectionPoints, int scanDirectionIndex, VolumePixelData *pixelData, int phaseIndex, QList<double> &grayValues);

private:
    /// Llista amb els valors de gris per calcular la mitjana i la desviació estàndard i altres dades estadístiques si cal.
    QList<double> m_grayValues;
};

}

#endif
