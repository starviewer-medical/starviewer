#ifndef UDGMAGICTOOL_H
#define UDGMAGICTOOL_H

#include "roitool.h"

#include <QVector>

namespace udg {

class Q2DViewer;
class Volume;
class DrawerText;
class DrawerPolygon;

/**
    Tool que serveix per editar el volum sobreposat en un visor 2D
*/
class MagicTool : public ROITool {
Q_OBJECT
public:

    // Creixement
    enum { LeftDown, Down, RightDown, Right, RightUp, Up, LeftUp, Left };
    // Moviments
    enum { MoveRight, MoveLeft, MoveUp, MoveDown };

    MagicTool(QViewer *viewer, QObject *parent = 0);
    ~MagicTool();

    void handleEvent(unsigned long eventID);

private:
    /// Crida a la generació de la regió màgica
    void generateRegion();

    /// Calcula el rang de valors d'intensitat vàlid a partir de \sa #m_magicSize i \see #m_magicFactor
    void computeLevelRange();

    /// Versió iterativa del region Growing
    void computeRegionMask();

    /// Fer un moviment des d'un índex cap a una direcció
    /// @param a, @param b índex del volum de la màscara que estem mirant en cada crida
    /// @param movement direcció en el moviment
    void doMovement(int &a, int &b, int movement);

    /// Desfer un moviment dses d'un índex cap a una direcció
    /// @param a, @param b índex del volum de la màscara que estem mirant en cada crida
    /// @param movement direcció en el moviment
    void undoMovement(int &a, int &b, int movement);

    /// Genera el polígon a partir de la màscara
    void computePolygon();

    /// Mètodes auxiliar per la generació del polígon
    void getNextIndex(int direction, int x, int y, int &nextX, int &nextY);
    int getNextDirection(int direction);
    int getInverseDirection(int direction);
    void addPoint(int direction, int x, int y, double z);
    bool isLoopReached();

    /// Retorna la desviació estàndard dins la regió marcada per la magicSize
    /// @param a, @param b, @param c índex del volum de la màscara que estem mirant
    double getStandardDeviation(int x, int y, int z);

    /// Comença la generació de la regió màgica
    void startMagicRegion();

    /// Calcula la regió definitiva i mostra per pantalla les mesures
    void closeForm();

    /// Modifica el Magic Factor \sa #m_magicFactor segons el desplaçament del ratolí
    void modifyMagicFactor();

    ///Troba els extrems per cada configuració
    void assignBounds(int &minX, int &minY, int &maxX, int &maxY);

private slots:
    /// Inicialitza la tool
    void initialize();

private:
    /// Mida de la tool
    int m_magicSize;
    double m_magicFactor;

    /// Màscara de la regió que formarà el polígon
    QVector<bool> m_mask;

    /// Rang de valors que es tindran en compte pel region growing
    double m_lowerLevel;
    double m_upperLevel;

    /// Coordenades de món a on s'ha fet el click inicial
    double m_pickedPosition[3];
};

}

#endif
