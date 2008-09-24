/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQTHREADRUNWITHEXEC_H
#define UDGQTHREADRUNWITHEXEC_H

#include <QThread>

namespace udg {

class QThreadRunWithExec : public QThread
{
Q_OBJECT
protected:
    void run() { exec(); }
};

}

#endif
