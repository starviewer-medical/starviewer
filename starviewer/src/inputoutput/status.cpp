/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
 
 
#include "status.h"

namespace udg{

Status::Status()
{
    m_dcmtkError=false;
    m_numberError=-1;
}

/**Retorna si l'èxit de l'element cridat
 *     @retun boolea indicant l'exit de la crida
*/
bool Status:: good()
{    
	return m_success;
}

/**Retorna un text descrivint l'error o l'exit del procés
 *      @return text descriptiu de l'exit o l'error de la crida
*/
std::string Status:: text()
{
    return m_descText;
}

/**Retorna el codi d'estat
 *      @return codi d'estat 
*/
int Status:: code()
{
	return m_numberError;
}


/** per contruir l'objecte status
 *       @param OFCodition resultant d'una crida
 *       @return retorna l'objecte status
 */
Status Status:: setStatus(const OFCondition  status)
{
    OFCondition cond(status);
	
	m_descText = cond.text();
	m_success = cond.good();
	m_numberError = cond.code();	
 
    return *this;
}

/** Crea un objecte Status
  *       @param descripció de l'status
  *       @param indica si s'ha finalitzat amb èxit
  *       @param número d'erro en cas d'haver-ni
  *       @return retorna l'objecte status  
  */
Status Status:: setStatus(std::string desc,bool ok,int numError)
{
    m_descText= desc;
    m_success=ok;
    m_numberError=numError;
    
    return *this;
}

};
