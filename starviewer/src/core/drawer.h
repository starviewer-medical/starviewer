/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDRAWER_H
#define UDGDRAWER_H

#include <QObject>
#include <QMultiMap>

namespace udg {

class Q2DViewer;
class DrawerPrimitive;

/**
Classe encarregada de pintar els objectes de primitiva gràfica en el viewer assignat

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Drawer : public QObject
{
Q_OBJECT
public:
    Drawer( Q2DViewer *viewer, QObject *parent = 0 );
    ~Drawer();

    /**
     * Dibuixa la primitiva donada sobre el pla i llesca indicats
     * En el cas que slice sigui -1, vol dir que voldrem que quan estem sobre aquell pla
     * la primitiva es veurà sempre. Si indiquem slice >-1, farem que la primitiva només
     * es vegi quan estem en aquell pla i en aquella llesca en concret
     * @param primitive Primitiva a pintar
     * @param plane Pla sobre el qual volem pintar la primitiva
     * @param slice Llesca a la que adjuntem la primitiva
     */
    void draw( DrawerPrimitive *primitive, int plane, int slice = -1 );

    /**
     * Afegim una primitiva al grup indicat.
     * @param primitive Primitiva que volem afegir
     * @param groupName nom del grup on la volem incloure
     */
    void addToGroup( DrawerPrimitive *primitive, const QString &groupName );

    /**
     * Mostra/amaga les primitives que hi ha en un determinat grup
     * @param groupName nom del grup que volem mostrar/amagar
     */
    void hideGroup(const QString &groupName);
    void showGroup(const QString &groupName);

public slots:
    /**
     * Refresca les primitives que s'han de veure pel viewer segons el seu estat
     */
    void refresh();

    /**
     * Deixa de mantenir la primitiva dins de la seva estructura interna
     * i l'elimina de l'escena on s'estava pintant
     * @param primitive Primitiva que volem deixar de controlar
     */
    void erasePrimitive(DrawerPrimitive *primitive);

private:
    /**
     * Mostra/amaga les primitives que hi ha en un pla i llesca determinats
     * @param plane Pla sobre que volem mostrar/amagar les primitives
     * @param slice Llesca dins d'aquell pla. En el cas que el pla sigui Top2DPlane, slice no es té en compte
     */
    void hide( int plane, int slice );
    void show( int plane, int slice );

private:
    /// Viewer sobre el qual pintarem les primitives
    Q2DViewer *m_2DViewer;

    /// Contenidors de primitives per cada pla possible
    QMultiMap< int, DrawerPrimitive *> m_axialPrimitives;
    QMultiMap< int, DrawerPrimitive *> m_sagitalPrimitives;
    QMultiMap< int, DrawerPrimitive *> m_coronalPrimitives;
    QList< DrawerPrimitive * > m_top2DPlanePrimitives;

    /// Pla i llesca en el que es troba en aquell moment el 2D Viewer. Serveix per controlar
    /// els canvis de llesca i de pla, per saber quines primitives hem de netejar
    int m_currentPlane;
    int m_currentSlice;

    /// Grups de primitives. Les agrupem per nom
    QMultiMap<QString, DrawerPrimitive *> m_primitiveGroups;
};

}

#endif
