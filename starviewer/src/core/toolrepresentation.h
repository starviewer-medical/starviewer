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

#ifndef TOOLREPRESENTATION_H
#define TOOLREPRESENTATION_H

#include <QObject>

namespace udg {

class QViewer;
class DrawerPrimitive;
class Drawer;

/**
    Classe contenidor de ToolRepresentation
  */
class ToolRepresentation : public QObject {
Q_OBJECT
public:
    ToolRepresentation(Drawer *drawer, QObject *parent = 0);
    ~ToolRepresentation();

   /// Rep events des de Tool per dibuixar-se
   /// @param eventID tipus d'event
   /// @param posX posició X de l'event
   /// @param posY posició Y de l'event
    virtual void handleEvent(long unsigned eventID, double posX, double posY);

signals:
    void finished();

protected:
    /// Actualitza el viewer
    void refresh();

protected:
    /// Drawer del viewer amb el que es pintaran les primitives
    Drawer *m_drawer;

    /// Llista de primitives a pintar
    QList<DrawerPrimitive*> m_primitivesList;
};

}

#endif
