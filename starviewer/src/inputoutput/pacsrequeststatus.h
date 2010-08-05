/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gr�fics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGPACSREQUESTATUS_H
#define UDGPACSREQUESTATUS_H

namespace udg {

/** Classe que defineix els Enum amb els errors que ens poden retornar les diferents Job que es comuniquen amb el PACS
*/
class PACSRequestStatus {

public:
    /* SendSomeDICOMFilesFailed - L'enviament d'algunes imatges ha fallat    
       SendWarningForSomeImages: Per a StoreSCU indica que totes les imatges s'han enviat però per totes o alguna imatge hem rebut un warning, per exemple podem rebre un 
                     warning perquè el PACS ha guardat amb una transfer syntax diferent les imatges del que les havíem enviat.
       SendAllDICOMFilesFailed : L'enviament de totes les imatges ha fallat*/
    enum SendRequestStatus {SendOk, SendCanNotConnectToPACS, SendAllDICOMFilesFailed, SendWarningForSomeImages, SendSomeDICOMFilesFailed, SendCancelled, SendUnknowStatus, SendPACSConnectionBroken};

    /*Es defineix els tipus d'error que podem tenir al descarregar fitxers DICOM:
        RetrieveDatabaseError: Indica quan error de base de dades d'Starviewer
        MoveDestinationAETileUnknownStatus: El PACS no té registrat el nostre AETitle per permetre-li fer descàrregues
        MoveWarningStatus: Ha fallat la descàrrega d'algun dels fitxers sol·licitats
        RetrieveIncomingDICOMConnectionsPortInUse: El port per rebre les connexions entrants per rebre els fitxers està en ús */
    enum RetrieveRequestStatus {RetrieveOk, RetrieveDatabaseError, RetrieveCanNotConnectToPACS, RetrieveNoEnoughSpace, RetrieveErrorFreeingSpace, RetrievePatientInconsistent, 
        RetrieveDestinationAETileUnknown, RetrieveIncomingDICOMConnectionsPortInUse, RetrieveFailureOrRefused, RetrieveSomeDICOMFilesFailed, RetrieveCancelled, RetrieveUnknowStatus};

};

}  //  end  namespace udg

#endif
