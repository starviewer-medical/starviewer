/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTOOL_H
#define UDGTOOL_H

#include <qobject.h>


namespace udg {

// Forward declarations
class QViewer;

/**
Classe base per a totes les classes de tools

@author Grup de Gràfics de Girona  ( GGG )
*/

class Tool : public QObject{
Q_OBJECT
public:
    Tool( QViewer *viewer, QObject *parent = 0, const char *name = 0);
    ~Tool();
    /// Aquí definim tots els tipus d'events que podem rebre
    enum EventIdType{ None , LeftButtonDown , LeftButtonUp , RightButtonDown , RightButtonUp , MiddleButtonDown , MiddleButtonUp , WheelForward , WheelBackward , KeyDown , KeyUp , MouseMove };
    
    /// Fa la feina predeterminada segons l'estat en el que es trobi i l'event rebut
    virtual void dispatchEvent( EventIdType event ) = 0;
    /// Fa la feina predeterminada segons l'estat en el que es trobi i l'event rebut
    //virtual void dispatchEvent( unsigned long event ){ dispatchEvent( vtkCommandEventToToolEvent(event)); };
    
    /// Passa un vtkCommand a EventIdType
    static EventIdType vtkCommandEventToToolEvent( unsigned long event );
protected:
    /// El visor sobre el qual actua
    QViewer *m_viewer;
}; 

};  //  end  namespace udg 

#endif

