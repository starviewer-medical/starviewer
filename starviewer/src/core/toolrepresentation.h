/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTOOLREPRESENTATION_H
#define UDGTOOLREPRESENTATION_H

#include <QObject>
#include <QColor>
#include <QMultiMap>

namespace udg {

class DrawerPrimitive;
class Drawer;
class ToolHandler;
class ToolData;

/**
    Classe contenidor de ToolRepresentation
*/
class ToolRepresentation : public QObject {
Q_OBJECT
public:
    ToolRepresentation(Drawer *drawer, QObject *parent = 0);
    ~ToolRepresentation();

    bool isVisible();

    /// Selecciona / Deselecciona la representació
    bool isSelected();
    void select();
    void deselect();

    ///Mostra/Amaga la representació
    void hide();
    void show();

    /// Fa els càlculs pertinents de la Tool
    virtual void calculate() = 0;

    QList<ToolHandler *>& getToolHandlers();

    /// Retorna cert si la representació està dins els bounds, amb la corresponent vista. Fals altrament
    bool isInsideOfBounds(double bounds[6], int view);

signals:
    /// Envia signal de seleccionada
    void selected(ToolRepresentation *toolRepresentation);

    /// Mostra/Amaga els ToolHandlerWithRepresentation
    void hideToolHandlers();
    void showToolHandlers();

protected:
    /// Actualitza el viewer
    void refresh();

    /// Crea els handlers
    virtual void createHandlers() = 0;

    /// Mapeja els handlers amb els punts corresponents
    virtual QList<double *> mapHandlerToPoint(ToolHandler *toolHandler) = 0;

protected slots:
    /// Seleccionem la representació
    void selectRepresentation();

    /// Edita els vèrtexs de les DrawerPrimitive
    void movePoint(ToolHandler *handler, double *point);
    virtual void moveAllPoints(double *movement) = 0;

protected:
    /// Drawer del viewer amb el que es pintaran les primitives
    Drawer *m_drawer;

    /// Definieix si la representació és visible o no
    bool m_isVisible;

    /// Defineix si la representació està seleccionada a la capa de representacions
    bool m_isSelected;

    /// Llista amb les primitives per simplificar gestions
    QList<DrawerPrimitive *> m_primitivesList;

    /// Llista de ToolHandlers
    QList<ToolHandler *> m_toolHandlers;

    /// Map de ToolHandlerWithRepresentation per fer el mapping de Handler a punt
    QMultiMap<int, ToolHandler *> m_toolHandlersMap;

    /// Drawing representations colors
    QColor m_representationColor;
    QColor m_selectedRepresentationColor;
    QColor m_handlerColor;
    QColor m_selectedHandlerColor;

    /// Dades de la tool
    ToolData *m_toolData;
};

}

#endif
