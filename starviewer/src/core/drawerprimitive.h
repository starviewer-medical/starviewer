/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGDRAWERPIMITIVE_H
#define UDGDRAWERPIMITIVE_H

#include <QObject>
#include <QColor>

class vtkProp;
class vtkCoordinate;

namespace udg {

/**
    Classe base de les primitives que pintarà la classe Drawer

    Defineix les propietats que poden tenir les primitives i els mètodes
    per obtenir aquesta primitiva en diferents formats (vtk, opengl, etc)
  */
class DrawerPrimitive : public QObject {
Q_OBJECT
public:
    DrawerPrimitive(QObject *parent = 0);
    ~DrawerPrimitive();

    /// Mètodes per configurar la visibilitat de la primitiva
    virtual void setVisibility(bool visible);
    void visibilityOn();
    void visibilityOff();
    bool isVisible() const;

    enum { WorldCoordinateSystem, DisplayCoordinateSystem };

    /// Mètodes per configurar el sistema de coordenades en que es troben
    /// els punts que defineixen la primitiva. Per defecte el sistema seran coordenades de món
    void setCoordinateSystem(int system);
    int getCoordinateSystem() const;

    /// Mètodes per configurar el color
    void setColor(QColor color);
    QColor getColor() const;

    /// Mètodes per configurar si la forma es pinta amb color de fons o no.
    /// Aquest atribut només tindrà sentit per formes tancades
    void setFilled(bool fill);
    bool isFilled() const;

    enum LinePatternType{ ContinuousLinePattern = 0xFFFF, DiscontinuousLinePattern = 0xFF00 };

    /// Mètodes per configurar el patró de la línia
    void setLinePattern(int pattern);
    int getLinePattern() const;

    /// Assigna/Obté l'amplada de la línia
    void setLineWidth(double width);
    double getLineWidth() const;

    /// Assigna/Obté l'opacitat de l'objecte
    void setOpacity(double opacity);
    double getOpacity() const;

    /// Assigna/Obté si la primitiva és esborrable o no
    void setErasable(bool erasable);
    bool isErasable() const;

    // Mètodes per construir l'objecte a representar gràficament
    
    /// Construeix l'objecte vtk que representa aquesta primitiva
    /// Retornem un vtkProp, el qual ens permet poder retornar tant vtkActor, vtkActor2D o vtkPropAssembly
    /// @return El vtkProp amb el que es representa la primitiva
    virtual vtkProp* getAsVtkProp();

    /// Ens diu si alguna de les propietats de la primitiva han estat modificades.
    /// Aquesta propietat indica si la seva representació gràfica, ja sigui vtk o openGL, està d'acord
    /// amb el que la defineix l'objecte
    /// @return Cert si s'ha modificat alguna propietat des de l'últim "update". Fals altrament
    bool isModified() const;

    /// Aquest mètode ens retorna la distància que hi ha des d'una determinada primitiva fins al punt passat per paràmetre.
    /// i ens dóna un paràmetre de sortida indicant quin és el punt de la primitiva més proper a aquest punt.
    virtual double getDistanceToPoint(double *point3D, double closestPoint[3]) = 0;

    /// Ens retorna els límits de l'hexahedre que encapsula la primitiva
    /// en aquest ordre: minX, maxX, minY, maxY, minZ, maxZ
    virtual void getBounds(double bounds[6]) = 0;

    /// HACK això és una solució temporal, minimitzar el seu ús a casos molt concrets!
    /// Mètodes per emular els smart pointers.
    /// En molts casos necessitem que una primitiva creada per una classe
    /// no es pugui esborrar (ni de memòria ni del drawer) ja que aquella classe
    /// volem que sigui propietària de la primitiva i que si se'ns fa un "RemovePrimitives"
    /// no ens afecti.
    /// Mentres no tinguem alguna classe smart pointer, el més aproximat és fer-ho a través d'aquests
    /// mètodes de forma manual (at your own risk!!)
    /// De moment, si no cridem cap d'aquests mètodes, tindrem un punter com sempre, per tant
    /// quan es faci servir aquest sucedani d'smart pointer n'haurem de ser completament conscients
    /// fins que trobem una solució sòlida

    /// Augmenta en 1 la referència a la primitiva. Es farà servir quan volem
    /// ser "propietaris" d'una primitiva
    void increaseReferenceCount();

    /// Decrementa en 1 la referència a la primitiva. Després d'això donem a entendre
    /// que ja no volem ser propietaris de la primitiva
    void decreaseReferenceCount();

    /// Ens retorna el comptatge de referències
    /// (hauria d'equivaldre al nombre de "propietaris")
    int getReferenceCount() const;

    /// Ens diu si la primitiva té "propietaris" o no.
    /// Retorna fals si getReferenceCount() == 0, true, altrament
    bool hasOwners() const;

public slots:
    /// Mètode virtual que implementarà cada primitiva i que actualitzarà les representacions
    /// internament. Ara mateix, el 100% dels casos serà VTK.
    /// TODO Si volem que es puguin tenir diverses representacions segons la plataforma
    /// caldria implementar-ho amb algun patró tipus Factory
    virtual void update() = 0;

signals:
    /// S'emet quan alguna de les propietats ha canviat
    void changed();

    /// S'emet just quan s'invoca el destructor
    void dying(DrawerPrimitive *);

protected slots:
    /// Aquest slot s'hauria de re-implementar per cada classe i és on s'actualitza
    /// l'estructura vtk que s'ha creat per representar la primitiva
    virtual void updateVtkProp(){};

    /// Li diem si està modificat o no
    /// @param modified
    void setModified(bool modified = true);

public:
    /// Ens retorna un objecte vtkCoordinate configurat a partir del sistema de coordenades
    /// que té especificada la primitiva. Aquest mètode ens és útil per quan es vol construir
    /// la primitiva en format vtk
    /// @return L'objecte vtkCoordinate
    vtkCoordinate* getVtkCoordinateObject();

protected:
    /// Tipus de representació que podem usar per construir la Primitiva en escena
    enum RepresentationType { VTKRepresentation, OpenGLRepresentation };

    /// Representació interna que es fa servir per la primitiva
    RepresentationType m_internalRepresentation;

    /// Defineix si la primitiva és visible o no
    bool m_isVisible;

    /// Sistema de coordenades
    int m_coordinateSystem;

    /// Color de la primitiva, per defecte Qt::Green
    QColor m_color;

    /// Controla si cal omplir les formes tancades amb el color per defecte. Per defecte no s'omplen
    bool m_isFilled;

    /// Patró de la línia
    int m_linePattern;

    /// Ample de la línia. Per defecte 1.0
    double m_lineWidth;

    /// Opacitat de l'objecte, per defecte 1.0, és a dir, completament opac
    double m_opacity;

    /// Indica si alguna de les propietats s'han modificat
    bool m_modified;

    /// Stores the \c vtkProp representing this primitive.
    vtkProp* m_vtkProp;

private:
    /// Propietat d'esborrabilitat de la primitiva
    bool m_isErasable;
    
    /// Portarà el control de reference count (sucedani d'smart pointer(TM))
    int m_referenceCount;

    /// Guarda un punter a l'objecte vtkCoordinate que retorna el mètode getVtkCoordinateObject() per poder-lo destruir després.
    vtkCoordinate *m_coordinate;

};

}

#endif
