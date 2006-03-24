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
    if (m_dcmtkError)
    {
        return (*i).good();
    }
    else return m_success;
}

/**Retorna un text descrivint l'error o l'exit del procés
 *      @return text descriptiu de l'exit o l'error de la crida
*/
std::string Status:: text()
{
    if (m_dcmtkError)
    {
        return std::string((*i).text());
    }
    return m_descText;
}

/**Retorna el codi d'estat
 *      @return codi d'estat 
*/
int Status:: code()
{
    if (m_dcmtkError)
    {
        return (*i).code();
    }
    else return m_numberError;
}


/** per contruir l'objecte status
 *       @param OFCodition resultant d'una crida
 *       @return retorna l'objecte status
 */
Status Status:: setStatus(const OFCondition  status)
{
    m_dcmtkError=true;
    
    OFCondition cond(status);
    m_listStatus.push_back(cond);
    i=m_listStatus.begin();
    
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
    m_dcmtkError=false;
    
    m_descText= desc;
    m_success=ok;
    m_numberError=numError;
    
    return *this;

}

/** Posiciona la llista al primer element
  */
void Status::firstStatus()
{
    if (m_dcmtkError)
    {
        i=m_listStatus.begin();
    }
}

/** Apunta al seguent Status
 */
void Status::nextStatus()
{
    if (m_dcmtkError)
    {
        if (i!=m_listStatus.end()) i++;
    }
}

/** Indica si s'ha arribat al fianl de la llista
  */
bool Status::end()
{
    if (m_dcmtkError)
    {
        return i==m_listStatus.end();
    }
    else return true;
}

};
