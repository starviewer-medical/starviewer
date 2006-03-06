/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
 
#include "const.h"
#include "pacsconnection.h"

namespace udg{

/** retorna l'objecte connexió de les dcmtk, perquè altres classes les puguin utilitzar per connectar-se amb el PACS i fer-li les crides, pertitents, com la
  * classe QueryStudy,QuerySeries 
  *            @return retorna una connexió dctmk
  */
T_ASC_Association* PacsConnection:: getPacsConnection()
{
    return m_assoc;
}
/** Inserim a l'objecte, la connexió dcmtk que utilitzarem per connectar-nos en el pacs
  *
  */
void PacsConnection:: setPacsConnection(T_ASC_Association *connec)
{
    m_assoc = connec;
}

};

