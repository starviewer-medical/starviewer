/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGOLDDRAWER_H
#define UDGOLDDRAWER_H

#include <QString>
#include <QMultiMap>
#include <QPair>
#include <QSet>
#include "colorpalette.h"

//Foreward declarations
class vtkCoordinate;
class QColor;
class QString;
class vtkProp;
class vtkPoints;
class vtkCellArray;

namespace udg {

//Forward declarations
class DrawingPrimitive;
class Point;
class Line;
class Text;
class Polygon;
class Ellipse;
class Representation;
class Q2DViewer;

/**
Classe experta en dibuixar totes les primitives gràfiques que podem representar en un visualitzador 2D i s'encarrega d'emmagatzemar-les per tal de controlar la visibilitat d'aquestes segons les vistes axial, sagital i coronal. Conté tres mapes (un per cada vista) que relacionen la llesca amb les annotacions contingudes per un visualitzador 2D.

@author Grup de Gràfics de Girona  ( GGG )
*/

class OldDrawer : public QObject{
    Q_OBJECT

public:

    OldDrawer( Q2DViewer *m_viewer , QObject *parent = 0 );

    ~OldDrawer();

    /// Tipus definit:  parella de primitiva actor per tenir aquests dos tipus d'objectes relacionats:
    ///PrimitiveActorPair.first serà la DrawingPrimitive i PrimitiveActorPair.second el vtkProp
    typedef QPair< DrawingPrimitive*, vtkProp* > PrimitiveActorPair;

        /// Tipus definit:  map on hi guardem les vinculacions de les llesques amb els actors i primitives creades
    typedef QMultiMap< int, PrimitiveActorPair* > PrimitivesMap;

    /// llista de representacions que ens retornarà el QMultiMap
    typedef QList< PrimitiveActorPair* > PrimitivesPairsList;

    /// definim un tipus que representa un conjunt per guardar primitives relacionades entre sí, com per exemple, una línia i un text d'una determinada distància.
    typedef QSet< DrawingPrimitive* > PrimitivesSet;

    /// llista de conjunts de primitives relacionades entre sí. Ens permetrà controlar totes les relacions entre les diferents primitives.
    typedef QList< PrimitivesSet* > PrimitivesSetList;

    ///Parella d'objectes vtk per a poder encapsular els punts de l'el·lipse que volem calcular.
    typedef QPair< vtkPoints*, vtkCellArray*> EllipsePoints;

    ///ens retorna el nombre de primitives que ha dibuixat el drawer
    int getNumberOfDrawedPrimitives();

    ///dibuixa un punt amb els atributs passats dins l'objecte passat per paràmetre
    void drawPoint( Point *point, int slice, int view );

    ///dibuixa una línia amb els atributs passats dins l'objecte passat per paràmetre
    void drawLine( Line *line, int slice, int view );

    ///dibuixa text amb els atributs passats dins l'objecte passat per paràmetre
    void drawText( Text *text, int slice, int view );

    ///dibuixa el polígon passat per paràmetre
    void drawPolygon( Polygon *polygon, int slice, int view );

    ///dibuixa l'el·lipse passada per paràmetre.
    void drawEllipse( Ellipse *ellipse, int slice, int view );

    ///els editors gràfics solen enmarcar les el·lipses dins d'un rectangle, sense tenir en compte els eixos major i menor i el centre.
    ///Aquest mètode serveix per fer aquesta operació: passem com a punts els marges superior esquerre i l'inferior dret del rectangle que conté l'el·lipse.
    void drawEllipse( double rectangleCoordinate1[3], double rectangleCoordinate2[3], QColor color, QString behavior, int slice, int view );

    ///fa el resaltat de les primitives més properes
    void highlightNearestPrimitives();

    ///ens retorna la paleta de colors
    ColorPalette* getColorPalette()
    { return ( m_colorPalette ); }

    ///ens permet afegir un nou conjunt de primitives associades a la llista, a partir d'una representació
    void addSetOfPrimitives( Representation *representation );

    ///retorna el conjunt de primitives seleccionat
    PrimitivesSet* getSelectedSet()
    { return( m_selectedSet ); }

    ///passa a conjunt en estat seleccionat el que està com a més proper: highlight -> Selected. A més li canvia el color.
    void selectNearestSet();

    ///deselecciona el conjunt que està en estat de selecció
    void unselectSet();

    ///ens diu si hi ha algun conjunt seleccionat
    bool hasSelectedSet()
    { return( m_selectedSet != NULL ); }

    ///ens esborra el conjunt de primitives seleccionat
    void removeSelectedSet();

    ///ens diu si hi ha algun conjunt marcat com el més proper
    bool hasNearestSet()
    { return( m_nearestSet && !m_nearestSet->isEmpty() ); }

public slots:
    /// Elimina totes les annotacions dels multimaps
    void removeAllPrimitives();

    /// Actualitza la llesca/vista actual en la que es troba el visor 2D associat i els actors corresponents que cal visualitzar
    void setCurrentSlice( int slice );
    void setCurrentView( int view );

private:
     /// Donada una llesca i una vista ens retorna la corresponent llista d'actors i primitives
    PrimitivesPairsList getPrimitivesPairsList( int slice, int view );

    ///Retorna el sistema de coordenades segons l'especificat per paràmetre
    vtkCoordinate *getCoordinateSystem( QString coordinateSystem );

    ///permet assignar un determinat sistema de coordenades a un objecte vtkCoordinate.
    void setCoordinateSystem( QString coordinateSystem, vtkCoordinate *coordinates );

    ///retorna les coordenades adaptades segons la vista on estem treballant: representem els objectes amb actors 2D i això implica una selecció de coordenades
    ///segons la vista on es dibuixi l'objecte.
    void adaptCoordinatesToCurrentView( double *coordinates, int view );

    /// Fa invisibles/visibles els actors d'una llesca i vista donats
    void hidePrimitivesFrom( int slice, int view );
    void showPrimitivesFrom( int slice, int view );

    ///Fa invisible/visible l'associació primitiva/actor passada per paràmetre
    void setVisibility( PrimitiveActorPair *pair, bool visibility );

    ///Valida les coordenades segons la vista que es determina
    void validateCoordinates( double coordinates[3], int view );

    ///mètode per retornar-nos l'índex de la primitiva única de tipus \a primitiveType que contindrà la llista \a nearestPairslist de primitives més properes.
    int getIndexOfPairWhenTypeIs( PrimitivesPairsList nearestPairslist, QString primitiveType );

    ///mètode que ens diu si la llista \a list conté alguna primitiva de tipus \a primitiveType
    bool hasPrimitiveOfType( PrimitivesPairsList list, QString primitiveType );

    ///Ens retorna una llista de parelles de totes les primitives del tipus especificat d'un determinat mapa segons la vista
    PrimitivesPairsList getAllPrimitivesOfType( QString primitiveType );

    ///Ens retorna el conjunt de primitives de la llista \a m_primitivesSetList que conté la primitiva passada per paràmetre
    PrimitivesSet* getSetOf( DrawingPrimitive *primitive );

    ///ens permet assignar el color de highlight a la parella passada per paràmetre
    void setHighlightColor( PrimitiveActorPair *pair );

    ///ens permet assignar el color de selecció a la parella passada per paràmetre
    void setSelectedColor( PrimitiveActorPair *pair );

    ///ens permet assignar el color de normal a la parella passada per paràmetre
    void setNormalColor( PrimitiveActorPair *pair );

    ///ens diu si el punt està dins de les fronteres de la línia
    bool isPointIncludedInLineBounds( double point[3], double *lineP1, double *lineP2 );

    ///ens calcula els punts de l'el·lipse passada per paràmetre i ens retorna una parella d'objectes vtk que contenen aquests punts
    EllipsePoints computeEllipsePoints( Ellipse *ellipse );

    /// Afegeix una associació de primitiva/actor a la llesca i vista indicades
    void addPrimitive( PrimitiveActorPair *pair, int slice, int view );

    ///afegeix l'actor passat per paràmetre al visor i al mapa corresponent segons la vista i actualitza el visor.
    void addActorAndRefresh( vtkProp *actor, DrawingPrimitive *primitive, int slice, int view );

    ///cerca un objecte PrimitiveActorPair dins del map especificat segons la vista i en la llesca determinada
    PrimitiveActorPair* findPrimitiveActorPair( DrawingPrimitive *primitive, int slice, int view );

    ///cerca un objecte PrimitiveActorPair dins dels 3 maps, si és el cas en que no sabem la vista ni la llesca en que l'hem creat.
    PrimitiveActorPair* findPrimitiveActorPair( DrawingPrimitive *primitive );

    ///ens diu si les dades que conté són vàlides (NULL o no ). Aquest mètode ens servirà per saber si l'objecte retornat pel mètode findPrimitiveActorPair conté valors vàlids.
    bool isValid( PrimitiveActorPair *pair );

    ///fa invisible totes les primitives d'una determinada vista
    void hidePrimitivesOfView( int view );

    ///ens retorna la parella més propera al punt donat i segons la vista i llesca on estem
    PrimitiveActorPair* getNearestPrimitivePair( double point[3] );

    ///retorna el conjunt més proper a la posició del mouse
    PrimitivesSet* getNearestSet()
    { return( m_nearestSet ); }


private slots:
    ///cerca la línia que ha invocat el signal i n'actualitza els atributs gràfics
    void updateChangedLine( Line *line );

    ///cerca el que ha invocat el signal i n'actualitza els atributs gràfics
    void updateChangedText( Text *text );

    ///cerca l'el·lipse que ha invocat el signal i n'actualitza els atributs gràfics
    void updateChangedEllipse( Ellipse *ellipse );

private:
    /// Amb aquests maps hi guardem les vinculacions de les llesques amb els actors i primitives creades.
    PrimitivesMap m_axialPairs;
    PrimitivesMap m_sagittalPairs;
    PrimitivesMap m_coronalPairs;

    ///Llista on guardarem els conjunts de primitives relacionades entre sí.
    PrimitivesSetList m_primitivesSetList;

    ///conjunt de primitives més proper (candidat a highlight)
    PrimitivesSet *m_nearestSet;

    ///conjunt de primitives seleccionat
    PrimitivesSet *m_selectedSet;

    ///visor 2D
    Q2DViewer *m_2DViewer;

    /// llesca sobre la que es troba el visor 2D associat
    int m_currentSlice;

    /// vista actual del visor 2D associat
    int m_currentView;

    /// paleta de colors que fa servir el OldDrawer
    ColorPalette *m_colorPalette;
};

};
#endif

