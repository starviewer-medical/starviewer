#ifndef UDGDRAWER_H
#define UDGDRAWER_H

#include <QObject>
#include <QMultiMap>
#include <QSet>

#include "q2dviewer.h"

namespace udg {

class DrawerPrimitive;

/**
    Classe encarregada de pintar els objectes de primitiva gràfica en el viewer assignat
  */
class Drawer : public QObject {
Q_OBJECT
public:
    Drawer(Q2DViewer *viewer, QObject *parent = 0);
    ~Drawer();

    /// Dibuixa la primitiva donada sobre el pla i llesca indicats
    /// En el cas que slice sigui -1, vol dir que voldrem que quan estem sobre aquell pla
    /// la primitiva es veurà sempre. Si indiquem slice >-1, farem que la primitiva només
    /// es vegi quan estem en aquell pla i en aquella llesca en concret
    /// @param primitive Primitiva a pintar
    /// @param plane Pla sobre el qual volem pintar la primitiva
    /// @param slice Llesca a la que adjuntem la primitiva
    void draw(DrawerPrimitive *primitive, int plane, int slice = -1);

    /// Esborra totes les primitives esborrables que es veuen al visor, és a dir, en el pla i llesques actuals.
    void clearViewer();

    /// Afegim una primitiva al grup indicat.
    /// @param primitive Primitiva que volem afegir
    /// @param groupName nom del grup on la volem incloure
    void addToGroup(DrawerPrimitive *primitive, const QString &groupName);

    /// Deshabilita les primitives que hi ha en un determinat grup, fent que mai siguin visibles, sota cap condició
    /// @param groupName Nom del grup de primitives que volem deshabilitar
    void disableGroup(const QString &groupName);

    /// Habilita les primitives que hi ha en un determinat grup, fent que tornin a tenir el comportament habitual 
    /// on el Drawer decidirà si han de ser visibles o no segons el pla i llesca assignats
    /// @param groupName Nom del grup de primitives que volem habilitar
    void enableGroup(const QString &groupName);

    /// Retorna la primitiva esborrable més propera al punt donat, dins de la vista i llesca proporcionats
    /// Aquest mètode no té en compte cap llindar de proximitat, és a dir, ens retorna la primitiva que en termes
    /// absoluts és més propera al punt donat. En cas que no hi hagi cap primitiva per aquella vista i llesca, es retornarà nul.
    DrawerPrimitive* getNearestErasablePrimitiveToPoint(double point[3], int view, int slice, double closestPoint[3]);

    /// Ens esborra les primitives esborrables que estiguin dins de la zona delimitada pels punts passats per paràmetre.
    void erasePrimitivesInsideBounds(double bounds[6], Q2DViewer::CameraOrientationType view, int slice);

    /// Ens diu el total de primitives dibuixades en totes les vistes
    int getNumberOfDrawnPrimitives();

public slots:
    /// Deixa de mantenir la primitiva dins de la seva estructura interna
    /// i l'elimina de l'escena on s'estava pintant
    /// @param primitive Primitiva que volem deixar de controlar
    void erasePrimitive(DrawerPrimitive *primitive);

    /// Esborra totes les primitives registrades al drawer (inclou les primitives no esborrables).
    void removeAllPrimitives();

private:
    /// Mostra/amaga les primitives que hi ha en un pla i llesca determinats
    /// @param plane Pla sobre que volem mostrar/amagar les primitives
    /// @param slice Llesca dins d'aquell pla. En el cas que el pla sigui Top2DPlane, slice no es té en compte
    void hide(int plane, int slice);
    void show(int plane, int slice);

    /// Ens diu si la primitiva donada, que es troba a la vista view, està dins dels bounds indicats
    bool isPrimitiveInside(DrawerPrimitive *primitive, Q2DViewer::CameraOrientationType view, double bounds[6]);

    /// Esborra la primitiva donada del contenidor de primitives especificat.
    /// Si la troba l'esborra. Retorna cert si la troba, fals altrament.
    bool erasePrimitiveFromContainer(DrawerPrimitive *primitive, QMultiMap<int, DrawerPrimitive*> &primitiveContainer);

private slots:
    /// Refresca les primitives que s'han de veure pel viewer segons el seu estat
    void refresh();

private:
    /// Viewer sobre el qual pintarem les primitives
    Q2DViewer *m_2DViewer;

    /// Contenidors de primitives per cada pla possible
    QMultiMap<int, DrawerPrimitive*> m_axialPrimitives;
    QMultiMap<int, DrawerPrimitive*> m_sagitalPrimitives;
    QMultiMap<int, DrawerPrimitive*> m_coronalPrimitives;
    QList<DrawerPrimitive*> m_top2DPlanePrimitives;

    /// Pla i llesca en el que es troba en aquell moment el 2D Viewer. Serveix per controlar
    /// els canvis de llesca i de pla, per saber quines primitives hem de netejar
    int m_currentPlane;
    int m_currentSlice;

    /// Grups de primitives. Les agrupem per nom
    QMultiMap<QString, DrawerPrimitive*> m_primitiveGroups;

    /// Conjunt de primitives en estat disabled
    QSet<DrawerPrimitive*> m_disabledPrimitives;
};

}

#endif
