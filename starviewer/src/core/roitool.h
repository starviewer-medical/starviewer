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

private:
    typedef struct
    {
        double m_mean;
        double m_standardDeviation;
    } StatisticsData;
    
    /// Methods to compute statistics data, such as mean and standard deviation, upon the contained voxels on the ROI
    double computeMean(const QList<double> &grayValues);
    double computeStandardDeviation(const QList<double> &grayValues, double meanOfGrayValues);
    
    /// Calcula les dades estadístiques de la ROI.
    /// Serà necessari cridar aquest mètode abans si volem obtenir la mitjana i/o la desviació estàndar
    StatisticsData computeStatisticsData();

    /// Computes the voxel values contained inside polygonSegments. It will use the sweepLine algorithm, begining with the line defined 
    /// with the given points and will end at sweepLineEnd height
    QList<double> computeVoxelValues(const QList<Line3D> &polygonSegments, Point3D sweepLineBeginPoint, Point3D sweepLineEndPoint, double sweepLineEnd);
    
    /// Returns a list with the indices of the corresponding segments of the given list which crosses the given height, that is, those segments
    /// which its initial and end point are between the specified heigh on the heightIndex
    QList<int> getIndexOfSegmentsCrossingAtHeight(const QList<Line3D> &segments, double height, int heightIndex);

    /// Gets the points that intersect with polygonSegments and the given sweepLine and orders them by the xIndex of view
    QList<double*> getIntersectionPoints(const QList<Line3D> &polygonSegments, const Line3D &sweepLine, const OrthogonalPlane &view);

    /// Adds the voxels that are in the path of the intersection points to the given list
    void addVoxelsFromIntersections(const QList<double*> &intersectionPoints, double currentZDepth, const OrthogonalPlane &view, VolumePixelData *pixelData, int phaseIndex, QList<double> &grayValues);
};

}

#endif
