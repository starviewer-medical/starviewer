/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
 
#include "const.h"
#include "pacsconnection.h"

namespace udg{

T_ASC_Association* PacsConnection:: getPacsConnection()
{
    return m_assoc;
}

void PacsConnection:: setPacsConnection(T_ASC_Association *connec)
{
    m_assoc = connec;
}

};

