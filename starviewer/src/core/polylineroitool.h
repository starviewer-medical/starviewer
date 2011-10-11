#ifndef UDGPOLYLINEROITOOL_H
#define UDGPOLYLINEROITOOL_H

#include "roitool.h"

namespace udg {

class DrawerPolyline;
class DrawerPolygon;

/**
    Tool per crear ROIs amb una forma lliure determinada per un polígon irregular.
  */
class PolylineROITool : public ROITool {
Q_OBJECT
public:
    PolylineROITool(QViewer *viewer, QObject *parent = 0);
    ~PolylineROITool();

    void handleEvent(long unsigned eventID);

private slots:
    /// Inicialitza l'estat de la tool
    void initialize();

private:
    /// Gestiona quin punt de la ROI estem dibuixant. Es cridarà cada cop que
    /// haguem fet un clic amb el botó esquerre del mouse.
    void handlePointAddition();

    /// Ens permet anotar el següent punt de la polilínia. Si la primitiva no ha sigut creada, abans d'afegir el nou punt, la crea.
    void annotateNewPoint();

    /// Ens simula com quedaria la polilínia que estem editant si la tanquèssim. Ens serveix per a veure dinàmicament l'evolució de la polilínia.
    void simulateClosingPolyline();

    /// Mètode que tanca la forma de la polilínia que s'ha dibuixat
    void closeForm();

    /// Elimina la representacio temporal de la tool
    void deleteTemporalRepresentation();

private:
    /// Polilínia principal: és la polilínia que ens marca la forma que hem anat editant.
    QPointer<DrawerPolyline> m_mainPolyline;

    /// Polilínia de tancament: es la polilínia que ens simula com quedaria la polilínia principal si es tanques, es a dir, uneix l'últim punt anotat
    /// i el primer punt de la polilínia.
    QPointer<DrawerPolyline> m_closingPolyline;
};

}

#endif
