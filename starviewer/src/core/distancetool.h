#ifndef UDGDISTANCETOOL_H
#define UDGDISTANCETOOL_H

#include "measurementtool.h"
#include <QPointer>

namespace udg {

class DrawerLine;
class DrawerText;

/**
    Eina per mesurar distàncies en un visor 2D.
    Ens permet fixar dos punts a la pantalla i calcular la distància que els separa.

    Per marcar cadascun dels punts es farà amb un clic o doble-clic amb el botó esquerre
    del ratolí.
    Un cop marcat el primer punt de la distància a calcular, es mostrarà una línia que unirà
    el primer punt marcat amb el punt on es trobi el punter del mouse. Mentres l'usuari no
    hagi marcat el segon punt, no es mostrarà cap informació respecte a la distància en aquell moment.

    Un cop marcat el segon punt, la mesura es mostrarà en mil·límetres, si existeix informació
    d'espaiat, en píxels altrament.

    Un cop acabada la mesura, es poden annotar successivament tantes mesures com es desitji.

    Les mesures annotades només apareixeran sobre les imatges sobre les que s'han realitzat,
    però no s'esborren si canviem d'imatge. Si canviem d'imatge però tornem a mostrar-la més endavant
    les annotacions fetes sobre aquesta anteriorment, tornaran a aparèixer.

    Quan es canvïi l'input del visor, les annotacions fetes fins aquell moment s'esborraran.
    Quan es desactivi l'eina, les annotacions fetes fins aquell moment es mantindran.
  */
class DistanceTool : public MeasurementTool {
Q_OBJECT
public:
    DistanceTool(QViewer *viewer, QObject *parent = 0);
    ~DistanceTool();

    void handleEvent(long unsigned eventID);

private:
    /// Gets the text corresponding to the current measurement
    QString getMeasurementText();

    /// Places the measurement text in an proper position relative to the drawn line
    void placeMeasurementText(DrawerText *text);
    
    /// Gestiona quin punt de la distància estem dibuixant. Es cridarà cada cop que
    /// haguem fet un clic amb el botó esquerre del mouse.
    void handlePointAddition();

    /// Marca un nou punt de la distància. Si la corresponent primitiva
    /// no s'ha creat es crea abans d'afegir el nou punt.
    void annotateNewPoint();

    /// Simula la línia quan es mou el ratolí i tenim el primer punt marcat.
    void simulateLine();

    /// Elimina la representacio temporal de la tool
    void deleteTemporalRepresentation();

    /// Equalitza la profunditat dels elements que formen la distància final.
    void equalizeDepth();

private slots:
    /// Inicialitza l'estat de la tool.
    void initialize();

private:
    /// Estats possibles de la línia dibuixada.
    enum { NoPointFixed, FirstPointFixed };

    /// Línia que es dibuixa.
    QPointer<DrawerLine> m_line;

    /// Estat de la línia.
    int m_lineState;
};

}

#endif
