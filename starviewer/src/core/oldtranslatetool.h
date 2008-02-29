/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGOLDTRANSLATETOOL_H
#define UDGOLDTRANSLATETOOL_H

#include "oldtool.h"

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class OldTranslateTool : public OldTool
{
Q_OBJECT
public:
    enum { NONE , TRANSLATING };
    OldTranslateTool( QViewer *viewer, QObject *parent = 0 );
    ~OldTranslateTool();

    void handleEvent( unsigned long eventID );

private:
    /// realitza la feina de desplaçament
    void pan();

/// \TODO potser aquests mètodes slots passen a ser públics
private slots:
    /// Comença el translate
    void startTranslate();

    /// Calcula el nou translate
    void doTranslate();

    /// Atura l'estat de translate
    void endTranslate();

private:
    /// Viewer sobre el qual s'actua
    QViewer *m_viewer;
};

}

#endif
