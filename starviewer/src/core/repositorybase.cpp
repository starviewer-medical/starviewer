/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "repositorybase.h"
 
namespace udg {

RepositoryBase::RepositoryBase(QObject *parent, const char *name)
 : QObject( parent )
{
    this->setObjectName( name );
}

RepositoryBase::~RepositoryBase()
{
}


};  // end namespace udg {
