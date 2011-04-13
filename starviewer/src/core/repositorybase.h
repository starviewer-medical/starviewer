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
    RepositoryBase(QObject *parent = 0);

    ~RepositoryBase();

signals:
    void changed();
};

};  //  end  namespace udg

#endif
