/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQ2DVIEWERBLACKBOARD_H
#define UDGQ2DVIEWERBLACKBOARD_H

#include <QObject>
#include <QMultiMap>
#include <QColor>

class vtkActor2D;
class vtkPolyData;
class vtkPoints;

namespace udg {

/**
Classe dissenyada per controlar les anotacions que es fan sobre un Q2DViewer. Cada anotació serà visible en funció de la vista i la llesca a la que la vinculem.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class Q2DViewer;

class Q2DViewerBlackBoard : public QObject
{
Q_OBJECT
public:

    /// Per especificar el tipus d'orientació que té el texte
    enum { NormalTextOrientation = 0, RightRotatedTextOrientation = 1, UpsideDownTextOrientation = 2, LeftRotatedTextOrientation = 3 };

    Q2DViewerBlackBoard( Q2DViewer *viewer, QObject *parent = 0);

    ~Q2DViewerBlackBoard();

    /// Afegeix un punt amb les coordenades de món donades vinculat a la llesca i vista donades
    void addPoint( double point[2], int slice, int view, QColor color = Qt::green );

    /// Afegeix una anotació de texte. Li hem d'indicar el punt d'anclatge, les coordenades de la bounding box que tanquen l'annotació, el texte i la llesca i vista corresponents. Com a paràmetres opcionals tenim, la orientació del texte, la justificació del texte, el color amb que es pinta, si es mostra o no el contorn de la bounding box i si té un punter cap al punt d'anclatge. Per defecte serà justificació esquerra, color verd, i sense contorn ni punter.
    void addTextAnnotation( double attachPoint[2], double position1[2], double position2[2], const char *text, int slice, int view, int orientation = NormalTextOrientation, int justification = 0, QColor color = Qt::green, bool hasBorder = true, bool hasLeader = false );
    /// Similar a l'anterior però aquí passem algun argument més que ens pot ser útil pels presenation states. A més a més canviem l'ordre d'alguns paràmetres
    void addTextAnnotation( const char *text, int slice, int view, bool hasBoundingBox, bool hasAttachPoint, double attachPoint[2], double position1[2], double position2[2], int orientation = NormalTextOrientation, int justification = 0, QColor color = Qt::green, bool hasBorder = true, bool hasLeader = false );
    /// Afegeix un cercle amb el centre i radi donats
    void addCircle( double center[2], double radius, int slice, int view, bool filled = false, QColor color = Qt::green );

    /// Afegeix una elipse donades les coordenades del extrems dels seus eixos
    void addEllipse( double xAxis1[2], double xAxis2[2], double yAxis1[2], double yAxis2[2], int slice, int view, bool filled = false, QColor color = Qt::green );

    /// Afegeix una polyline. Pot ser que sigui tancada o oberta. També podem indicar si volem que sigui interpolada per splines. Si és oberta, el paràmetre filled s'ignora. \TODO Per comoditat i qüestions de temps ara els punts es donen amb vtkPoints, però lo seu seria fer-ho amb un std::vector< double[2] >.
    void addPolyline( vtkPoints *points, int slice, int view, bool interpolated = false, bool filled = false, QColor color = Qt::green );

    /// Afegeix una marca i (opcionalment) una nota a la llesca indicada que representa un Key Image Note
    void addKeyImageNoteMark(int slice, int view, QColor color = Qt::red, const QString &description = "");

public slots:
    /// Elimina totes les annotacions
    void clear();

protected:
    /// El Q2DViewer amb que estem vinculats
    Q2DViewer *m_2DViewer;

    /// Tipus de la llista que ens retornarà el QMultiMap
    typedef QList< vtkActor2D * > ActorsListType;

    /// Donada una llesca i una vista ens retorna la corresponent llista d'actors
    ActorsListType getActorsList( int slice, int view );

    /// Amb aquests maps hi guardem les vinculacions de les llesques amb els actors creats
    QMultiMap< int, vtkActor2D * > m_axialAnnotations;
    QMultiMap< int, vtkActor2D * > m_sagitalAnnotations;
    QMultiMap< int, vtkActor2D * > m_coronalAnnotations;

    /// última llesca sobre la que es troba el viewer2D
    int m_lastSlice;

    /// última vista sobre la que es troba el viewer2D
    int m_lastView;

    /// Afegeix un actor a la llesca i vista indicades
    void addActor( vtkActor2D *actor, int slice, int view );

    /// Retorna un actor2D a partir d'un polydata
    vtkActor2D *createActorFromPolyData( vtkPolyData *polyData, QColor color );

protected slots:
    /// Donada una llesca s'encarrega de pintar els objectes corresponents a aquesta i d'esborrar els que no s'han de veure
    void refreshObjects( int slice );

    /// S'encarrega d'actualitzar paràmetres d'alguns actors que els afecten les rotacions de la càmera, ja sigui pel factor de rotació o pels graus de la càmera
    void refreshRotation( int factor );
    void refreshRotation( double degrees );
};

}

#endif
