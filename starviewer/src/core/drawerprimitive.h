/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDRAWERPIMITIVE_H
#define UDGDRAWERPIMITIVE_H

#include <QObject>
#include <QColor>

class vtkProp;
class vtkCoordinate;

namespace udg {

/**
Classe base de les primitives que pintarà la classe Drawer

Defineix les propietats que poden tenir les primitives i
els mètodes per obtenir aquesta primitiva en diferents formats (vtk, opengl, etc)

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DrawerPrimitive : public QObject
{
Q_OBJECT
public:
    DrawerPrimitive(QObject *parent = 0);

    ~DrawerPrimitive();

    // mètodes per definir les propietats de la primitiva, com color, tipus de línia,
    // visibilitat, etc

    /**
     * Mètodes per configurar la visibilitat de la primitiva
     */
    void setVisibility( bool visible );
    void visibilityOn();
    void visibilityOff();
    bool isVisible() const;

    enum { WorldCoordinateSystem, DisplayCoordinateSystem };
    /**
     * Mètodes per configurar el sistema de coordenades en que es troben
     * els punts que defineixen la primitiva. Per defecte el sistema seran coordenades de món
     */
    void setCoordinateSystem( int system );
    int getCoordinateSystem() const;

    /**
     * Mètodes per configurar el color
     */
    void setColor( QColor color );
    QColor getColor() const;

    /**
     * Mètodes per configurar si la forma es pinta amb color de fons o no.
     * Aquest atribut només tindrà sentit per formes tancades
     */
    void setFilled( bool fill );
    bool isFilled() const;

    enum LinePatternType{ ContinuousLinePattern = 0xFFFF, DiscontinuousLinePattern = 2000 };
    /**
     * Mètodes per configurar el patró de la línia
     */
    void setLinePattern( int pattern );
    int getLinePattern() const;

    /**
     * Assigna/Obté l'amplada de la línia
     */
    void setLineWidth( double width );
    double getLineWidth() const;

    /**
     * Assigna/Obté l'opacitat de l'objecte
     */
    void setOpacity( double opacity );
    double getOpacity() const;

    // mètodes per construir l'objecte a representar gràficament
    /**
     * Construeix l'objecte vtk que representa aquesta primitiva
     * Retornem un vtkProp, el qual ens permet poder retornar tant vtkActor, vtkActor2D o vtkPropAssembly
     * @return El vtkProp amb el que es representa la primitiva
     */
    virtual vtkProp *getAsVtkProp();

signals:
    /// s'emet quan alguna de les propietats ha canviat
    void changed();

protected slots:
    /**
     * Aquest slot s'hauria de re-implementar per cada classe i és on s'actualitza
     * l'estructura vtk que s'ha creat per representar la primitiva
     */
    virtual void updateVtkProp(){};

protected:
    /**
     * Ens retorna un objecte vtkCoordinate configurat a partir del sistema de coordenades
     * que té especificada la primitiva. Aquest mètode ens és útil per quan es vol construir
     * la primitiva en format vtk
     * @return L'objecte vtkCoordinate
     */
    vtkCoordinate *getVtkCoordinateObject();

protected:
    /// Defineix si la primitiva és visible o no
    bool m_isVisible;

    /// Sistema de coordenades
    int m_coordinateSystem;

    /// Color de la primitiva, per defecte Qt::Green
    QColor m_color;

    /// Controla si cal omplir les formes tancades amb el color per defecte. Per defecte no s'omplen
    bool m_isFilled;

    /// patró de la línia
    int m_linePattern;

    /// Ample de la línia. Per defecte 1.0
    double m_lineWidth;

    /// Opacitat de l'objecte, per defecte 1.0, és a dir, completament opac
    double m_opacity;
};

}

#endif

