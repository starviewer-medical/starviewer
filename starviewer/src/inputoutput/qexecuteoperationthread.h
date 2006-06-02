/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQEXECUTEOPERATIONTHREAD_H
#define UDGQEXECUTEOPERATIONTHREAD_H

#include <QThread>

namespace udg {

/** Aquest classe, s'encarrega d'anar executant objectes Operation. (operacions que s'han de dur a terme). Aquesta classe crea un thread quan hi ha alguna operacio i les executa. A més també utilitza una cua, on es van guardant totes les operation pendents d'executar 
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class Operation;
class Status;

class QExecuteOperationThread :public QThread
{
Q_OBJECT

public:
    /** Constructor de la classe
      */
    QExecuteOperationThread( QObject *parent = 0 );

    /** afegeix una operacio a la cua, si la cua esta buida l'afegeix i crea el thread que l'executarà 
      *         @param Operation a executar
      */
    void queueOperation( Operation operation );
    
    /** Codi que s'executa en un nou thread, evalua l'operacio i decideix que fer
      */
    void run();
    
    /** Destructor de la classe
      */
    ~QExecuteOperationThread();

signals :
    
    /** signal que s'emet cap a QueryScreen quant s'ha descarregat la primera serie d'un estudi per a que pugui ser visualitzat
     * @param studyUID UID de l'estudi a visualitzar
     */
    void viewStudy( QString studyUID );
    
    /** signal que s'emet cap a QRetrieveScreen per indicar que l'estudi s'està descarregant
     * @param studyUID UID de l'estudi que s'està descarregant
     */    
    void setStudyRetrieving( QString studyUID );

    /** signal que s'emet cap a QRetrieveScreen per indicar que l'estudi s'ha descarregat
     * @param studyUID UID de l'estudi descarregat
     */    
    void setStudyRetrieved( QString studyUID );

    /** signal que s'emet cap a QRetrieveScreen per indicar que s'ha produït un error en la descàrrega de l'estudi
     * @param studyUID UID de l'estudi que ha produït l'error
     */    
    void setErrorRetrieving( QString studyUID );

    /** signal que s'emet cap a QRetrieveScreen per indicar que s'ha descarregat una nova imatge de l'estudi
     * @param studyUID UID de l'estudi que s'esta descarregat
     * @param número d'imatge descarrega
     */        
    void imageRetrieved( QString studyUID , int );

    /** signal que s'emet cap a QRetrieveScreen per indicar que s'ha descarregat una nova sèroe de l'estudi
     * @param studyUID UID de l'estudi que s'esta descarregat
     */       
    void seriesRetrieved( QString );
    
    ///Signal que s'emet cap a QueryScreen per indicar que no hi ha prou espai lliure per a descarregar nous estudis    
    void notEnoughFreeSpace();

    ///Signal que s'emet quant s'ha produït un error alliberant espai de la cache
    void errorFreeingCacheSpace();

    /** signal que s'emete si s'ha produit algun error alhora de connectar amb algun pacs
     * @param  pacsID del pacs amb que s'ha produit error
     */
    void errorConnectingPacs( int );
    
private slots :

    /** Si la operacio es un view, emet un signal per indicar que ja es pot obrir la primera serie de l'estudi
      *     @param studyUID de l'estudi a descarregar
      */
     void firstSeriesRetrieved( QString );

    /** Emet un singnal cap al queryscreen per indicar que s'ha descarregat una imatge
      *     @param UID de l'estudi
      *     @param número d'imatge
      */
    void imageRetrievedSlot( QString , int );
    
    /** Emet un singal per indicar que s'ha descarregat una sèrie
      *     @param studyUID de l'estudi a descarregar
      */
    void seriesRetrievedSlot( QString );
    

private :
    
    bool m_stop;//indica si el thread esta parat
    bool m_view;//indica si la operacio que s'esta realitzant és la descarrega d'un estudi per visualitzar-lo    
        
    /** Descarrega un estudi, segons els paràmetres a operation, si l'estudi s'ha de visualitzar 
      * captura els signals de l'objecte starviewersettings que es emes cada vegada que finalitza la descarrega d'una  serie
      *     @param operation a executar
      */
    void retrieveStudy(Operation operation);
    
    /** Indica si hi ha com a minim 1Gb d'espai lliure tant a la cache com al disc per descarregar el nou estudi. 
      * Si detecta,que no hi ha suficient a la cache o al disc, intenterà alliberar 2000 Mb, esborrant estudis vells, si no retornara que no hi ha suficient espai al disc
      *     @param retorna si hi ha suficient espai per descarregar l'estudi
      *     @return retorna l'estat de l'operacio
      */
    Status enoughFreeSpace( bool &enoughSpace);

};

}

#endif
