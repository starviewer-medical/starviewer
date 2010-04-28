/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGMAGICTOOL_H
#define UDGMAGICTOOL_H

#include "tool.h"

#include <QPointer>
#include <QVector>

class vtkActor;
class vtkImageData;

namespace udg {

class Q2DViewer;
class Volume;
class DrawerText;
class DrawerPolygon;

/**
    Tool que serveix per editar el volum sobreposat en un visor 2D

    @author Laboratori de Gràfics i Imatge  ( GILab ) <vismed@ima.udg.es>
*/
class MagicTool : public Tool {
Q_OBJECT
public:
    MagicTool( QViewer *viewer, QObject *parent = 0 );
    ~MagicTool();

    void handleEvent( unsigned long eventID );

private:
    /**
     * Crida a la generació de la regió màgica
     */
    void setMagicPoint();

    /**
     * Versió iterativa del region Growing
     */
    void paintRegionMask();

    /**
     * Fer un moviment dses d'un índex cap a una direcció
     * @param a, @param b índex del volum de la màscara que estem mirant en cada crida
     * @param movement direcció en el moviment
     */
    void doMovement(int &a, int &b, int movement);

    /**
     * Desfer un moviment dses d'un índex cap a una direcció
     * @param a, @param b índex del volum de la màscara que estem mirant en cada crida
     * @param movement direcció en el moviment
     */
    void undoMovement(int &a, int &b, int movement);

    /**
     * Detecta les vores 
     */
	void detectBorder();
	void getNextIndex(int direction,int x1,int y1,int &x2,int &y2);
	int getNextDirection(int direction);
	int getInverseDirection(int direction);
	void addPoint(int direction,int x1,int y1, double z1);
	bool isLoopReached();

	/**
     * Retorna la desviació estàndard dins la regió marcada per la magicSize
     * @param a, @param b, @param c índex del volum de la màscara que estem mirant
     */
	double getStandardDeviation(int a, int b, int c);

	/**
     * Incrementa la mida de l'editor
     */
    void increaseMagicSize();

    /**
     * Decrementa la mida de l'editor
     */
    void decreaseMagicSize();

private slots:
    /**
     * Inicialitza diverses dades necessàries per la tool: Valors de m_insideValue, m_outsideValue i m_volumeCont
     */
    void initialize();

private:
    /// Ens guardem aquest punter per ser més còmode
    Q2DViewer *m_2DViewer;

    ///Mida de la tool
    int m_magicSize;
	
    QVector<bool> m_mask;

    double m_magicFactor;

    int m_lowerlevel;
    int m_upperlevel;

    int m_a;
    int m_b;
    int m_c;
    int m_i;
    int m_ext[6];

    DrawerText * m_text;

    vtkImageData * m_input;

    /// Polígon principal: és la polígon que ens marca la forma que hem segmentat
    QPointer<DrawerPolygon> m_mainPolygon;
};

}

#endif
