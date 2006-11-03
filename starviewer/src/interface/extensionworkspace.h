/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGEXTENSIONWORKSPACE_H
#define UDGEXTENSIONWORKSPACE_H

#include <QTabWidget>

// Forward declarations
class QAction;
class QToolButton;

namespace udg {

/**
@author Grup de Gràfics de Girona  ( GGG )
*/
class ExtensionWorkspace : public QTabWidget{
Q_OBJECT
public:

    ExtensionWorkspace(QWidget *parent = 0, const char *name = 0);

    ~ExtensionWorkspace();

    /// Afegeix una nova aplicació \TODO: paràmetre de recursos?, afegir un id d'aplicació per poder-la referenciar, afegir una label/nom, +altres possibles paràmetres
    void addApplication( QWidget *application , QString caption );

    /// Treu una aplicació de l'espai de mini-aplicacions \TODO: fer també per id?
    void removeApplication( QWidget *application );

    /// Obté/Assigna l'índex de la pàgina actual
    int getLastIndex() const { return m_lastIndex; }
    void setLastIndex( int index ){ m_lastIndex = index; }

public slots:

    /// Per tancar l'aplicació que està oberta en aquell moment
    void closeCurrentApplication();

private:

    /// Botó per tancar la pestanya actual
    QToolButton *m_closeTabButton;

    /// Acció associada al tancar la pestanya
    QAction *m_closeTabAction;

    /// Índex de la página actual que tractem
    int m_lastIndex;

    /// Crea les connexions de signals i slots
    void createConnections();

};

};  //  end  namespace udg

#endif
