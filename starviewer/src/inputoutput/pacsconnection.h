/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef PACSCONNECTION
#define PACSCONNECTION
#define HAVE_CONFIG_H 1

#include <dimse.h> // provide the structure DcmDataSet
#include <dcdeftag.h> //provide the information for the tags
#include <ofcond.h> //provide the OFcondition structure and his members
#ifndef CONST
#include "const.h"
#endif

/** Aquesta classe encapusa el objecte connexió, que hauran d'utilitzar les classes per fer les seves crides al PACS. Aquesta classe segueix el patró Abstract   *Factory. Aïllem l'objecte connexió de les dctmk, d'aquesta manera si mai canvia l'objecte de connexió de les dcmtk, només haurem de tocar aquesta classe
 */
namespace udg{
class PacsConnection{

public :

	/** retorna l'objecte connexió de les dcmtk, perquè altres classes les puguin utilitzar per connectar-se amb el PACS i fer-li les crides, pertitents, com la classe QueryStudy,QuerySeries
     * @return retorna una connexió dctmk
     */
    T_ASC_Association* getPacsConnection();

	/** Inserim a l'objecte, la connexió dcmtk que utilitzarem per connectar-nos en el pacs
     * @param estableix la connexio a utilitzar
     */
    void setPacsConnection( T_ASC_Association * );

private :

    T_ASC_Association *  m_assoc;

};
};
#endif
