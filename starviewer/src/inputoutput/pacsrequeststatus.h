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
    /* SomeImagesFailedSend - L'enviament d'algunes imatges ha fallat    
       WarningSend: Per a StoreSCU indica que totes les imatges s'han enviat però per totes o alguna imatge hem rebut un warning, per exemple podem rebre un 
                     warning perquè el PACS ha guardat amb una transfer syntax diferent les imatges del que les havíem enviat.
       FailureSend : L'enviament de totes les imatges ha fallat*/
    enum SendRequestStatus {OkSend, CanNotConnectPACSToSend, FailureSend, WarningSend, SomeImagesFailedSend, CancelledSend, UnknowStatusSend, PACSConnectionBroken};

    /*Es defineix els tipus d'error que podem tenir al descarregar fitxers DICOM:
        DatabaseError: Indica quan error de base de dades d'Starviewer
        MoveDestinationAETileUnknownStatus: El PACS no té registrat el nostre AETitle per permetre-li fer descàrregues
        MoveWarningStatus: Ha fallat la descàrrega d'algun dels fitxers sol·licitats
        IncomingConnectionsPortPacsInUse: El port per rebre les connexions entrants per rebre els fitxers està en ús */
    enum RetrieveRequestStatus {OkRetrieve, DatabaseError, CanNotConnectPACSToRetrieve, NoEnoughSpaceToRetrieveDICOMFiles, ErrorFreeingSpace, PatientInconsistent, 
        RetrieveDestinationAETileUnknown, IncomingConnectionsPortPACSInUse, RetrieveFailureOrRefused, RetrieveWarning, RetrieveCancelled, RetrieveUnknow};

};

}  //  end  namespace udg

#endif
