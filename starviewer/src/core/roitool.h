/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGROITOOL_H
#define UDGROITOOL_H

#include "tool.h"
#include "volume.h"
#include <QPointer>

namespace udg {

class Q2DViewer;
class DrawerPolygon;

/**
*
*   Tool pare per totes aquelles tools destinades a crear ROIs. 
*   S'encarrega de proporcionar els mètodes necessaris per calcular
*   les dades estadístiques relacionades amb la ROI (àrea, mitjana, desviació estàndar).
*   La gestió dels events i de com es dibuixa la forma de la ROI queda delegada en les
*   tools filles. La forma final de la tool ha de quedar dibuixada amb el membre m_roiPolygon.
*
*/
class ROITool : public Tool {
Q_OBJECT
public:
    ROITool(QViewer *viewer, QObject *parent = 0);
    ~ROITool();

    virtual void handleEvent(long unsigned eventID) = 0;

protected:
    /// Mètode per escriure a pantalla les dades calculades.
    void printData();

protected:
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

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
    /// Calcula el vòxel a partir de l'espaiat de la imatge i la coordenada i retorna el valor de gris
    Volume::VoxelType getGrayValue(double *coordinate);

    /// Calcula les dades estadístiques de la ROI. 
    /// Serà necessari cridar aquest mètode abans si volem obtenir la mitjana i/o la desviació estàndar
    void computeStatisticsData();

private:
    /// Llista amb els valors de gris per calcular la mitjana i la desviació estàndard i altres dades estadístiques si cal.
    QList<double> m_grayValues;
};

}

#endif
