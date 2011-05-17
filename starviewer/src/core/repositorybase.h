#ifndef UDGREPOSITORYBASE_H
#define UDGREPOSITORYBASE_H

#include <QObject>

namespace udg {

class RepositoryBase : public QObject {
Q_OBJECT
public:
    RepositoryBase(QObject *parent = 0);

    ~RepositoryBase();

signals:
    void changed();
};

};  // End namespace udg

#endif
