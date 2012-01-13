#ifndef UDGPERPENDICULARDISTANCETOOL_H
#define UDGPERPENDICULARDISTANCETOOL_H

#include "tool.h"

#include <QPointer>

namespace udg {

class DrawerLine;
class Q2DViewer;

/**
    Eina per mesurar distàncies perpendiculars en un visor 2D. Ens permet fixar dos punts a la pantalla per dibuixar una primera línia.
    Llavors amb un tercer clic es dibuixa una altra línia perpendicular a la primera que passa pel punt del tercer clic i es calcula la llargada d'aquesta.

    Per marcar cadascun dels punts es farà amb un clic o doble-clic amb el botó esquerre del ratolí.
    Un cop marcat el primer punt, es mostrarà una línia que unirà el primer punt marcat amb el punt on es trobi el punter del ratolí.
    Un cop marcat el segon punt, es mostrarà una línia perpendicular a la primera que unirà aquesta amb el punt on es trobi el punter del ratolí.
    Mentre l'usuari no hagi marcat el tercer punt, no es mostrarà cap informació respecte a la distància en aquell moment.

    Un cop marcat el tercer punt, la mesura es mostrarà en mil·límetres si existeix informació d'espaiat i en píxels altrament.

    Un cop acabada la mesura, es poden annotar successivament tantes mesures com es desitgi.

    Les mesures anotades només apareixeran sobre les imatges sobre les que s'han realitzat, però no s'esborren si canviem d'imatge.
    Si canviem d'imatge però tornem a mostrar-la més endavant les anotacions fetes sobre aquesta anteriorment tornaran a aparèixer.

    Quan es canviï l'input del visor, les anotacions fetes fins aquell moment s'esborraran.
    Quan es desactivi l'eina, les anotacions fetes fins aquell moment es mantindran.
  */
class PerpendicularDistanceTool : public Tool {

Q_OBJECT

public:

    PerpendicularDistanceTool(QViewer *viewer, QObject *parent = 0);
    ~PerpendicularDistanceTool();

    /// Decideix què s'ha de fer per cada esdeveniment rebut.
    virtual void handleEvent(long unsigned eventId);

private:

    /// Possibles estats de l'eina.
    enum State { NotDrawing, DrawingFirstLine, DrawingDistanceLine };

private:

    /// Decideix què s'ha de fer quan es rep un clic.
    void handleClick();
    /// Afegeix el primer punt, per començar a dibuixar la primera línia.
    void addFirstPoint();
    /// Afegeix el segon punt, per acabar de dibuixar la primera línia i començar la segona, la de distància.
    void addSecondPoint();
    /// Afegeix el tercer punt, per acabar de dibuixar la segona línia i calcular la distància.
    void addThirdPoint();
    /// Actualitza la primera línia perquè vagi del primer punt a la posició actual del ratolí.
    void updateFirstLine();
    /// Actualitza la línia de distància perquè sigui perpendicular a la primera línia i vagi des d'aquesta a la posició actual del ratolí (o al punt més proper
    /// de manera que encara toqui amb la primera línia).
    void updateDistanceLine();
    /// Actualitza la primera línia i refresca la visualització.
    void updateFirstLineAndRender();
    /// Actualitza la línia de distància i refresca la visualització.
    void updateDistanceLineAndRender();
    /// Calcula la distància i la dibuixa.
    void drawDistance() const;
    /// Calcula i retorna la llargada de la línia de distància en forma de text.
    QString getDistanceText() const;
    /// Avorta el dibuix de la distància perpendicular.
    void abortDrawing();
    /// Equalitza la profunditat dels elements que formen la distància perpendicular final.
    void equalizeDepth();

private slots:

    /// Torna l'eina al seu estat inicial.
    void reset();

private:

    /// Visor 2D sobre el qual treballem.
    Q2DViewer *m_2DViewer;
    /// La primera línia.
    QPointer<DrawerLine> m_firstLine;
    /// La segona línia, perpendicular a la primera, sobre la qual mesurem la distància.
    QPointer<DrawerLine> m_distanceLine;
    /// Estat de l'eina.
    State m_state;

};

}

#endif
