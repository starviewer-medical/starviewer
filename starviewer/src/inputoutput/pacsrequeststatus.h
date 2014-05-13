/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/


#ifndef UDGPACSREQUESTATUS_H
#define UDGPACSREQUESTATUS_H

namespace udg {

/**
    Classe que defineix els Enum amb els errors que ens poden retornar les diferents Job que es comuniquen amb el PACS
  */
class PACSRequestStatus {

public:
    /// SendSomeDICOMFilesFailed - L'enviament d'algunes imatges ha fallat
    /// SendWarningForSomeImages: Per a StoreSCU indica que totes les imatges s'han enviat però per totes o alguna imatge hem rebut un warning, per exemple
    ///                           podem rebre un warning perquè el PACS ha guardat amb una transfer syntax diferent les imatges del que les havíem enviat.
    /// SendAllDICOMFilesFailed : L'enviament de totes les imatges ha fallat
    enum SendRequestStatus { SendOk, SendCanNotConnectToPACS, SendAllDICOMFilesFailed, SendWarningForSomeImages, SendSomeDICOMFilesFailed, SendCancelled,
                             SendUnknowStatus, SendPACSConnectionBroken };

    /// Es defineix els tipus d'error que podem tenir al descarregar fitxers DICOM:
    ///  RetrieveDatabaseError: Indica quan error de base de dades d'Starviewer
    ///  MoveDestinationAETileUnknownStatus: El PACS no té registrat el nostre AETitle per permetre-li fer descàrregues
    ///  MoveWarningStatus: Ha fallat la descàrrega d'algun dels fitxers sol·licitats
    ///  RetrieveIncomingDICOMConnectionsPortInUse: El port per rebre les connexions entrants per rebre els fitxers està en ús
    enum RetrieveRequestStatus { RetrieveOk, RetrieveDatabaseError, RetrieveCanNotConnectToPACS, RetrieveNoEnoughSpace, RetrieveErrorFreeingSpace,
                                 RetrievePatientInconsistent, RetrieveDestinationAETileUnknown, RetrieveIncomingDICOMConnectionsPortInUse,
                                 RetrieveFailureOrRefused, RetrieveSomeDICOMFilesFailed, RetrieveCancelled, RetrieveUnknowStatus };

    /// Errors que es poden produir al fer Queries al PACS
    enum QueryRequestStatus { QueryOk, QueryCanNotConnectToPACS, QueryFailedOrRefused, QueryCancelled, QueryUnknowStatus };

};

}  //  end  namespace udg

#endif
