/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGPERFUSIONMAPCALCULATORTHREAD_H
#define UDGPERFUSIONMAPCALCULATORTHREAD_H


#include <QThread>


namespace udg {


/**
 * Thread que implementa els mètodes de càlcul de mapes de perfusió.
 *
 * \author Grup de Gràfics i Imatge de Girona (GILab) <vismed@ima.udg.edu>
*/
class PerfusionMapCalculatorThread : public QThread {

    Q_OBJECT

public:

    PerfusionMapCalculatorThread( int id, int numberOfThreads, QObject * parent = 0 );
    ~PerfusionMapCalculatorThread();

protected:

    virtual void run();

private:

    int m_id, m_numberOfThreads;

};


}


#endif




