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
class vtkInteractorStyle;
class QVTKWidget;

namespace udg {

/**
Classe base per a totes les classes de tools

@author Grup de Gràfics de Girona  ( GGG )
*/

class Tool : public QObject{
Q_OBJECT
public:
    Tool( QObject *parent = 0, const char *name = 0 );
    Tool( vtkInteractorStyle *interactor , QObject *parent = 0, const char *name = 0 );
    Tool( QVTKWidget *qvtkWidget , QObject *parent = 0, const char *name = 0 );
    ~Tool();

    /// Assigna l'interactor style de vtk que manipularem
    virtual void setVtkInteractorStyle( vtkInteractorStyle *interactor );

    ///
    void setQVTKWidget( QVTKWidget *qvtkWidget );
    
protected:
    /// Serivirà per gestionar els events
    vtkInteractorStyle *m_interactor;

    ///
    QVTKWidget *m_vtkWidget;

}; 

};  //  end  namespace udg 

#endif

