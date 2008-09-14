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
      *@param Operation a executar. Si l'operació de moure imatges cap un PACS, a la màscara de l'estudi de l'operació només caldrà especificar el studyUID
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
     * @param seriesUID de la imatge a visualitzar
     * @param imageUID de l'imatge a visualitzar
     */
    void viewStudy( QString studyUID , QString seriesUID , QString imageUID );

    /** signal que s'emet cap a QRetrieveScreen per indicar que l'estudi s'està descarregant
     * @param studyUID UID de l'estudi que s'està descarregant
     */
    void setOperating( QString studyUID );

    /** signal que s'emet cap a QRetrieveScreen per indicar que la operacio a finalitzatt
     * @param studyUID UID de l'estudi descarregat
     */
    void setOperationFinished( QString studyUID );

    /** signal que s'emet cap a QueryScreen per indicar que l'estudi s'ha descarregat
     * @param studyUID UID de l'estudi descarregat
     */
    void setRetrieveFinished( QString studyUID );

    /** signal que s'emet cap a QRetrieveScreen per indicar que s'ha produït un error en la descàrrega de l'estudi
     * @param studyUID UID de l'estudi que ha produït l'error
     */
    void setErrorOperation( QString studyUID );

    /** signal que s'emet cap a QRetrieveScreen per indicar que s'ha descarregat una nova imatge de l'estudi
     * @param studyUID UID de l'estudi que s'està descarregant
     * @param número d'imatge descarrega
     */
    void imageCommit( QString studyUID , int );

    /// Signal que s'emet quan canvia el nombre d'imatges que s'han descarregats del study que s'està processant actualment.
    /// L'study actual es pot saber a partir del signal setOperating
    void currentProcessingStudyImagesRetrievedChanged(int imagesRetrieved);

    /** signal que s'emet cap a QRetrieveScreen per indicar que s'ha descarregat una nova sèroe de l'estudi
     * @param studyUID UID de l'estudi que s'esta descarregat
     */
    void seriesCommit( QString );

    /** signal que s'emete si s'ha produit algun error alhora de connectar amb algun pacs
     * @param  pacsID del pacs amb que s'ha produit error
     */
    void errorConnectingPacs( int );

    /** signal que s'emet quan s'enqua una nova operació
     * @param newOperation operació encuada
     */
    void newOperation( Operation *newOperation );

    ///ha acabat la descàrrega
    void retrieveFinished();

    ///Signal que s'emet quan s'ha produït un error a l'operació de descàrrega
    void abort();

private :

    bool m_stop;//indica si el thread esta parat

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


    /** Mou un estudi descarregat al pacs especificat
     * @param operation paràmetres de l'operació que s'ha de dur a terme. Si � un Move a la màscara només cal especificar el studyUID
     * @return estat del mètode
     */
    void moveStudy( Operation operation );

};

}

#endif
