/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQQueryStudyThread_H
#define UDGQQueryStudyThread_H
#include <QThread>

#include "studymask.h"
#include "pacsparameters.h"

namespace udg {

class StudyMask;
class PacsParameters;
/** Classe que cercar estudis en un dispositiu pacs, creant un nou thread
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QQueryStudyThread :public QThread{
    Q_OBJECT
public:


    QQueryStudyThread(QObject *parent = 0);
    
    void queryStudy(PacsParameters parameters,StudyMask mask);
    void run();
    ~QQueryStudyThread();
 

signals:
    
    void errorConnectingPacs( int );

protected :

private :

    PacsParameters m_param;
    StudyMask m_mask;
    

};

}

#endif
