/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "tool.h"
#include "logging.h"

#include <vtkInteractorStyle.h>
#include <QVTKWidget.h>

namespace udg {

Tool::Tool( QObject *parent, const char *name)
 : QObject( parent )
{
    this->setObjectName( name );
    m_interactor = 0;
    m_vtkWidget =  0;
}

Tool::Tool( vtkInteractorStyle *interactor , QObject *parent, const char *name ) 
 : QObject( parent ) 
{
    Tool( parent , name );
    setVtkInteractorStyle( interactor );
}

Tool::Tool( QVTKWidget *qvtkWidget , QObject *parent, const char *name )
 : QObject( parent ) 
{
    Tool( parent , name );
    setQVTKWidget( qvtkWidget );
}

Tool::~Tool()
{
}

void Tool::setVtkInteractorStyle( vtkInteractorStyle *interactor )
{
    DEBUG_LOG("NO MOLA!!!!");
}

void Tool::setQVTKWidget( QVTKWidget *qvtkWidget )
{
    m_vtkWidget = qvtkWidget;
}

};  // end namespace udg 
