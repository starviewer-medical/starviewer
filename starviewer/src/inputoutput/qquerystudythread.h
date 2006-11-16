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

/** Classe que cercar estudis en un dispositiu pacs, creant un nou thread
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QQueryStudyThread :public QThread
{
    Q_OBJECT
public:

    /// Constructor de la classe
    QQueryStudyThread( QObject *parent = 0 );
    
    /** Accio que executa el thread
     * @param parametres del pacs a consultar
     * @param màscara de cerca
     */
    void queryStudy( PacsParameters parameters , StudyMask mask );
    
    /// el codi d'aquest mètode es el que s'executa en un nou thread
    void run();
    
    ///Destructor de la classe
    ~QQueryStudyThread();
 
signals:
    
    /** signal que s'envia per indicar que hi hagut un error connectant al PACS
     * @param ID del pacs que ha produit l'error 
     */
    void errorConnectingPacs( int pacsID );

    /** signal que s'envia per indicar que hi hagut un error cercant al PACS
     * @param ID del pacs que ha produit l'error 
     */    
    void errorQueringStudiesPacs( int pacsID );

protected :

private :

    /** Crea un string pel log, indicat que el thread s'ha creat, i a quin pacs atacara aquest thread
     * @return missatge de log
     */
    QString infoLogInitialitzedThread();

    PacsParameters m_param;
    StudyMask m_mask;
};

}  //end namespace UdG

#endif
