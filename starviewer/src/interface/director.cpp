/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

 
#include "director.h"
#include "qapplicationmainwindow.h"

namespace udg{

Director::Director( udg::QApplicationMainWindow* app,QObject *parent, const char *name )
 : QObject(parent)
{
    this->setObjectName( name );
    m_applicationWindow = app;
}

Director::~Director()
{
}

} // end namespace udg
