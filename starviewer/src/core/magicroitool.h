#ifndef UDGMAGICROITOOL_H
#define UDGMAGICROITOOL_H

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
class MagicROITool : public ROITool {
Q_OBJECT
public:
    
    // Creixement
    enum { LeftDown, Down, RightDown, Right, RightUp, Up, LeftUp, Left };
    // Moviments
    enum { MoveRight, MoveLeft, MoveUp, MoveDown };

    MagicROITool(QViewer *viewer, QObject *parent = 0);
    ~MagicROITool();

    void handleEvent(unsigned long eventID);

protected:
    virtual void setTextPosition(DrawerText *text);

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
    void startRegion();

    /// Calcula la regió definitiva i mostra per pantalla les mesures
    void endRegion();

    /// Modifica el Magic Factor #m_magicFactor segons el desplaçament del ratolí
    void modifyRegionByFactor();

    ///Troba els extrems per cada configuració
    void setBounds(int &minX, int &minY, int &maxX, int &maxY);

    ///Mini parche per obtenir el valor de Voxel depenent de si és Axial, Coronal o Sagital
    double getVoxelValue(int x, int y, int z);

    /// Elimina la representacio temporal de la tool
    void deleteTemporalRepresentation();

    /// Mètode auxiliar. Donats els índexs x,y i el nombre de columnes de la imatge, ens calcula el corresponent índex de la màscara.
    int computeMaskVectorIndex(int x, int y, int columns);

private slots:
    /// Inicialitza la tool
    void initialize();

    /// Reinicia la regió, invalidant l'anterior que hi hagués en curs si existia
    void restartRegion();

private:
    /// Mida de la tool
    static const int MagicSize;
    static const double InitialMagicFactor;

    double m_magicFactor;

    /// Màscara de la regió que formarà el polígon
    QVector<bool> m_mask;

    /// Rang de valors que es tindran en compte pel region growing
    double m_lowerLevel;
    double m_upperLevel;

    /// Coordenades de món a on s'ha fet el click inicial
    double m_pickedPosition[3];

    /// Polígon ple que es mostrarà durant l'edició de la ROI.
    QPointer<DrawerPolygon> m_filledRoiPolygon;

    /// Coordenades de pantalla a on s'ha fet el click inicial
    QPoint m_pickedPositionInDisplayCoordinates;
};

}

#endif
