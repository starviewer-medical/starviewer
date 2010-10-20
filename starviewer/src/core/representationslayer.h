/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef REPRESENTATIONSLAYER_H
#define REPRESENTATIONSLAYER_H

#include "toolrepresentation.h"

#include <QObject>
#include <QMultiMap>

namespace udg {

class Q2DViewer;
class ToolHandler;
class DrawerPrimitive;

/**
    Classe contenidor de ToolRepresentation
*/
class RepresentationsLayer : public QObject {
Q_OBJECT
public:
    RepresentationsLayer(Q2DViewer *viewer, QObject *parent = 0);
    ~RepresentationsLayer();

    /// Afegeix una representacio a la llista.
    void addRepresentation(ToolRepresentation *toolRepresentation, int plane, int slice = -1);

    /// Afegeix una primitiva a la llista.
    void addPrimitive(DrawerPrimitive *primitive, int plane, int slice = -1);

    /// Elimina una primitiva
    void removePrimitive(DrawerPrimitive *primitive);

    /// Elimina les primitives que es troben dins els bounds
    void removePrimitivesInsideBounds(double bounds[6]);

    /**
     * Afegim una primitiva al grup indicat.
     * @param primitive Primitiva que volem afegir
     * @param groupName nom del grup on la volem incloure
     */
    void addPrimitiveToGroup(DrawerPrimitive *primitive, const QString &groupName);

    /**
     * Mostra/amaga les primitives que hi ha en un determinat grup
     * @param groupName nom del grup que volem mostrar/amagar
     */
    void hidePrimitivesGroup(const QString &groupName);
    void showPrimitivesGroup(const QString &groupName);

    /// Esborra totes les primitives que es veuen al visor, és a dir, llesca actual.
    void clearViewer();
    
public slots:
    /// Actualitza les variables de plane i slice
    void refresh();

    /// Esborra totes les toolRepresentation's del viewer
    void clearAll();

    /// Avalua l'event que ha rebut del visualitzador al que està associat i l'envia als handlers
    void handleEvent(unsigned long eventID);

signals:
    /// Reenvia l'event rebut amb la posició on es troba
    void forwardEvent(unsigned long eventID, double *point);

    /// Indica als handlers que s'ha fet un click a la pantalla a la posició indicada
    void screenClicked(double *point);

    /// Indica que s'està editant o s'ha deixat d'editar
    void editing(bool editing);

private:
    /// Mostra/amaga les representacions que hi ha en un pla i llesca determinats
    /// @param slice Llesca dins d'aquell pla. En el cas que el pla sigui Top2DPlane, slice no es té en compte
    void hide(int slice);
    void show(int slice);

    /// Desconnecta el ToolHandler que s'editava
    void disconnectToolHandler();

    /// Manages the selected representations list
    void clearSelectedRepresentations();
    void deleteSelectedRepresentations();
    void addSelectedRepresentation(ToolRepresentation *toolRepresentation);
    void deleteSelectedRepresentation(ToolRepresentation *toolRepresentation);

    /// Delete a ToolRepresentation
    void removeRepresentation(ToolRepresentation *toolRepresentation);

private slots:
    /// Connecta els events rebuts al toolHandler per a l'edicio
    void connectToolHandler(ToolHandler *toolHandler);

    /// Selecciona una ToolRepresentation
    void selectRepresentation(ToolRepresentation *toolRepresentation);

private:
    /// Viewer al qual pertany la RepresentationsLayer
    Q2DViewer *m_2DViewer;

    /// Pla i llesca en el que es troba en aquell moment el 2D Viewer. Serveix per controlar
    /// els canvis de llesca i de pla, per saber quines primitives hem de netejar
    int m_currentPlane;
    int m_currentSlice;

    /// Contenidor de ToolRepresentation
    QMultiMap<int, ToolRepresentation *> m_toolRepresentations;

    /// Representations que es mostren a qualsevol pla
    QList<ToolRepresentation *> m_top2DPlaneRepresentations;

    /// ToolHandlers de totes les representacions
    QList<ToolHandler *> m_toolHandlers;

    /// Indica si s'esta en mode edició o no (creacio)
    bool m_editionMode;

    /// ToolHandler en edicio
    ToolHandler *m_connectedToolHandler;

    /// Selected Representations
    QList<ToolRepresentation *> m_selectedRepresentations;

    /// Determines whether m_selectedRepresentations has to be cleared or not.
    bool m_clearRepresentationBeforeAdd;

    /// Contenidors de primitives
    QMultiMap<int, DrawerPrimitive *> m_primitives;
    QList<DrawerPrimitive *> m_top2DPlanePrimitives;

    /// Grups de primitives. Les agrupem per nom
    QMultiMap<QString, DrawerPrimitive *> m_primitiveGroups;
};

}

#endif
