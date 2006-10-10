/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTOOL_H
#define UDGTOOL_H

#include <QObject>

// FWD declarations
class QAction;

namespace udg {

class QViewer;

/**
Classe base per a totes les classes de tools

@author Grup de Gràfics de Girona  ( GGG )
*/

class Tool : public QObject{
Q_OBJECT
public:
    /// Enumeració d'events
    enum EventID{ LeftButtonClick , LeftButtonRelease, RightButtonClick , RightButtonRelease , MiddleButtonClick , MiddleButtonRelease , MouseWheelForward , MouseWheelBackward , MouseMove };

    /// Retorna la QAction de la tool. Cada tool específica es crea la seva.
    QAction *getAction(){ return m_action; };

    /// Retorna el nom que identifica la tool. \TODO Es pot fer servir l'ObjectName del propi QObject o posar un nou membre que desi el nom de la tool
    virtual QString getToolName(){ return "none"; };
    
public slots:
    /// Decideix què s'ha de fer per cada event rebut. Mètode virtual pur que es re-implementa obligatòriament en cada classe filla.
    virtual void handleEvent( unsigned long eventID ) = 0;

protected:
    /// Mètode virtual re-implementat a cada sub-classe que crearà i configurarà la QAction a mida
    virtual void createAction(){};
    
    /// Per controlar l'estat de la tool
    int m_state;
    
    /// QAction associada a la tool. La QAction ha de ser static, ja que per exemple, si en una extensió tenim 3 finestres diferents que fan servir la mateixa tool, tindrem tres instàncies de tool que modficaran cada finestra, però només una sola tool que val igual per les tres. És a dir, donem un accés unificat (enable,disable,etc), però una interacció individual
    /*static*/ QAction *m_action;

}; 

};  //  end  namespace udg 

#endif

