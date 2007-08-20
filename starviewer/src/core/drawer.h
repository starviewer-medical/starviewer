/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDRAWER_H
#define UDGDRAWER_H

#include <QObject>
#include <QString>
#include <QMultiMap>

//Foreward declarations
class vtkCoordinate;
class QColor;
class QString;
class vtkProp;

namespace udg {

//Forward declarations
class DrawingPrimitive;
class Point;
class Line;
class Text;
class Polygon;
class Ellipse;
class Q2DViewer;
class DrawingPrimitive;

/**
Classe experta en dibuixar totes les primitives gràfiques que podem representar en un visualitzador 2D i s'encarrega d'emmagatzemar-les per tal de controlar la visibilitat d'aquestes segons les vistes axial, sagital i coronal. Conté tres mapes (un per cada vista) que relacionen la llesca amb les annotacions contingudes per un visualitzador 2D.

@author Grup de Gràfics de Girona  ( GGG )
*/

class Drawer : public QObject{
    Q_OBJECT
            
    private:
    ///definició de l'struct per a guardar primitives i vtkProps
        struct PrimitivePropAssociation
        {
            DrawingPrimitive *primitive;
            vtkProp *actor;
        };
    
     /// Amb aquests maps hi guardem les vinculacions de les llesques amb els actors i primitives creades.
        QMultiMap< int, PrimitivePropAssociation > m_axialPrimitives;
        QMultiMap< int, PrimitivePropAssociation > m_sagitalPrimitives;
        QMultiMap< int, PrimitivePropAssociation > m_coronalPrimitives;
    
    ///visor 2D
        Q2DViewer *m_2DViewer;

    /// llista de representacions que ens retornarà el QMultiMap
        typedef QList< PrimitivePropAssociation > PrimitivesList;

    /// Donada una llesca i una vista ens retorna la corresponent llista d'actors i primitives
        PrimitivesList getPrimitivesList( int slice, int view );
    
    /// llesca sobre la que es troba el visor 2D associat
        int m_currentSlice;

    /// vista actual del visor 2D associat
        int m_currentView;
    
    ///Retorna el sistema de coordenades segons l'especificat per paràmetre 
        vtkCoordinate *getCoordinateSystem( QString coordinateSystem );
    
    ///permet assignar un determinat sistema de coordenades a un objecte vtkCoordinate.
        void setCoordinateSystem( QString coordinateSystem, vtkCoordinate *coordinates );
    
    ///retorna les coordenades adaptades segons la vista on estem treballant: representem els objectes amb actors 2D i això implica una selecció de coordenades
    ///segons la vista on es dibuixi l'objecte.
        double* adaptCoordinatesToCurrentView( double *coordinates, int view );
    
    /// Fa invisibles/visibles els actors d'una llesca i vista donats
        void hidePrimitivesFrom( int slice, int view );
        void showPrimitivesFrom( int slice, int view );
    
    ///Fa invisible/visible l'associació primitiva/actor passada per paràmetre
        void setVisibility( PrimitivePropAssociation primitive, bool visibility );
        
    ///Valida les coordenades segons la vista que es determina
        void validateCoordinates( double coordinates[3], int view );
            
public:
    
    Drawer( Q2DViewer *m_viewer , QObject *parent = 0 );
    ~Drawer();
    ///MÈTODES REFERENTS AL DIBUIXAT DE PRIMITIVES
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

    /// Afegeix una associació de primitiva/actor a la llesca i vista indicades
    void addPrimitive( PrimitivePropAssociation primitive, int slice, int view );
    
    ///afegeix l'actor passat per paràmetre al visor i al mapa corresponent segons la vista i actualitza el visor.
    void addActorAndRefresh( vtkProp *actor, DrawingPrimitive *primitive, int slice, int view );
    
public slots:
    /// Elimina totes les annotacions dels multimaps
        void removeAllPrimitives();    
        
         /// Actualitza la llesca/vista actual en la que es troba el visor 2D associat i els actors corresponents que cal visualitzar
        void setCurrentSlice( int slice );
        void setCurrentView( int view );
};

};  
#endif

