/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGMULTQUERYSTUDY_H
#define UDGMULTQUERYSTUDY_H

#include <QObject>

#include "studylistsingleton.h"
#include "pacslist.h"
#include "dicommask.h"

namespace udg {

class Status;
class PacsParameters;

/** Classe que permet fer diverses cerques simultànies, a diversos pacs a la vegada mitjançant la utilitzacio de threads
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class MultipleQueryStudy : public QObject
{
Q_OBJECT
public:
    ///constructor de la classe
    MultipleQueryStudy( QObject *parent = 0 );

    ///destructor de la classe
    ~MultipleQueryStudy();

    /** Ens permet indicar quina màscara utilitzarem per fer la query als PACS
     * @param DicomMask Màscara del estudis a cercar
     */
    void setMask( DicomMask );

    /** Estableix la llista de PACS als quals es farà la cerca
     * @param PacsList amb els pacs als quals es cercarà
     */
    void setPacsList( PacsList );

    /** Una vegada haguem especificat la màscara, i tots els PACS als que volem realitzar la query, aquesta acció iniciara el procés de cerca a tots els PACS
     * @return Estat del mètode
     */
    Status StartQueries();

    /** retorna un apuntador a la llist amb els estudis
     * @return  Llista amb els estudis trobats que complien amb la màscara.
     */
    StudyListSingleton* getStudyList();

signals :

    /** signal que s'emete si s'ha produit algun error alhora de connectar amb algun pacs
     * @param  pacsID del pacs amb que s'ha produit error
     */
    void errorConnectingPacs( int );

    /** signal que s'envia per indicar que hi hagut un error cercant al PACS
     * @param ID del pacs que ha produit l'error
     */
    void errorQueringStudiesPacs( int pacsID );

public slots :

    /// Slot que s'activa pel signal de QQuerStudyThread, quan un thread acaba allibera un recurs del semàfor, perquè es pugui iniciar una altre thread per continuar amb la cerca
    void threadFinished();

    /** slot que s'activa pel signal de QQueryStudThread, quan s'ha produït algun error al connectar amb el PACS
     * @param pacsID del pacs que ha produït l'error
     */
    void slotErrorConnectingPacs( int );

    /** slot que s'activa pel signal de QQueryStudThread, quan s'ha produït algun error al fer la query amb el PACS
     * @param pacsID del pacs que ha produït l'error
     */
    void slotErrorQueringStudiesPacs( int );


private :

    DicomMask m_searchMask;

    StudyListSingleton* m_studyListSingleton;
    PacsList m_pacsList;
    int m_maxThreads;//Nombre màxim de threads que es poden executar a la vegada
    // Per raons d'optimització nomes es podran tenir un límit threads alhora executant la query, per aconseguir això utilitzem un semàfor
    QSemaphore *m_semaphoreActiveThreads;

};

}

#endif
