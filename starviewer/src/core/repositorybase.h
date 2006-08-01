/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGREPOSITORYBASE_H
#define UDGREPOSITORYBASE_H

#include <QObject>

namespace udg {

/**
@author Grup de Gràfics de Girona  ( GGG )
*/
class RepositoryBase  : public QObject{
Q_OBJECT
public:
    RepositoryBase(QObject *parent = 0, const char *name = 0);
    
    ~RepositoryBase();
};

};  //  end  namespace udg 

#endif
