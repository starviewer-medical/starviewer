/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "pacslistdb.h"
#include <string>

namespace udg {

PacsListDB::PacsListDB()
{
    m_DBConnect = DatabaseConnection::getDatabaseConnection();
}

/**  Construeix l'estat en que ha finaltizat l'operació sol·licitada
  *            @param Estat de sqlite
  *            @return retorna l'estat de l'operació
  */
Status PacsListDB::constructState(int numState)
{
//A www.sqlite.org/c_interface.html hi ha al codificacio dels estats que retorna el sqlite
    Status state;
    
    switch(numState)
    {//aqui tractem els errors que ens poden afectar de manera més directe, i els quals l'usuari pot intentar solucionbar                         
        case SQLITE_OK :        state.setStatus("Normal",true,0);
                                break;
        case SQLITE_ERROR :     state.setStatus("Database not found",false,2001);
                                break;
        case SQLITE_CORRUPT :   state.setStatus("Database corrupted",false,2011);
                                break;
        case SQLITE_CONSTRAINT: state.setStatus("Constraint Violation",false,2019);
                                break;
        case 99 :               state.setStatus("Data Not Found",false,2099);
                                break;
      //aquests errors en principi no es poden donar, pq l'aplicació no altera cap element de l'estructura, si es produeix algun
      //Error d'aquests en principi serà perquè la bdd està corrupte o problemes interns del SQLITE, fent Numerror-2000 de l'estat
      //a la pàgina de www.sqlite.org podrem saber de quin error es tracta.
        default :               state.setStatus("SQLITE internal error",false,2000+numState); 
                                break;
    }
   
   return state;
}

/**  Insereix els parametres d'un pacs a la base de dades, per a poder-hi cercar imatges
  *  En un alta el camp PacsID, és assignat automàticament per l'aplicació!
  *        @param Objecte PacsParameters amb les dades del pacs
  *        @return estat de l'operació  
  */
Status PacsListDB::insertPacs(PacsParameters *pacs)
{
    Status state,stateQuery;
    int i;
    std::string sql;
    
    //hem de comprovar que el pacs no existis ja abans! i ara estigui donat de baixa
    stateQuery = queryPacsDeleted(pacs);
    
    if (stateQuery.code() == 2099)
    {//El pacs no estava en està estat de baixa
        //El PACSid s'autoincrementa sol amb max(PACSID)+1
        sql.insert(0,"Insert into PacsList ");
        sql.append("(AETitle,Server,Port,Inst,Loc,Desc,Def,PacsID,Del) ");
        sql.append("Values (%Q,%Q,%Q,%Q,%Q,%Q,%Q,(select max(PacsID) from PacsList)+1,'N')");
    
        m_DBConnect->getLock();
        i=sqlite_exec_printf(m_DBConnect->getConnection(),sql.c_str(),0,0,0,
                        pacs->getAEPacs().c_str(),
                        pacs->getPacsAdr().c_str(),
                        pacs->getPacsPort().c_str(),
                        pacs->getInstitution().c_str(),
                        pacs->getLocation().c_str(),
                        pacs->getDescription().c_str(),
                        pacs->getDefault().c_str());

        m_DBConnect->releaseLock();
        state = constructState(i);
    }
    else if (stateQuery.code() == 0)
    {//existeix un PAcs amb el mateix AEtitle, actualitzem les dades i el donem d'alta
    
        state = updatePacs(pacs);
    }
    else return stateQuery;

    return state;                    
}

/**  Permet actualitzar la informació d'un pacs, el PacsID camp clau no es pot canviar!
  *         @param Objecte PAcsParameters ambles noves dades del PACS
  *        @return estat de l'operació  
  */
Status PacsListDB::updatePacs(PacsParameters *pacs)
{
    Status state;
    int i;
    std::string sql;
   
    sql.insert(0,"Update PacsList ");
    sql.append("set AETitle = %Q, ");
    sql.append("Server = %Q, ");
    sql.append("Port = %Q, ");
    sql.append("Inst = %Q, ");
    sql.append("Loc = %Q, ");
    sql.append("Desc = %Q, ");
    sql.append("Def = %Q, ");
    sql.append("Del = %Q ");
    sql.append (" where PacsID = %i");
    
    m_DBConnect->getLock();
    i = sqlite_exec_printf(m_DBConnect->getConnection(),sql.c_str(),0,0,0,
                    pacs->getAEPacs().c_str(),
                    pacs->getPacsAdr().c_str(),
                    pacs->getPacsPort().c_str(),
                    pacs->getInstitution().c_str(),
                    pacs->getLocation().c_str(),
                    pacs->getDescription().c_str(),
                    pacs->getDefault().c_str(),
                    "N",
                    pacs->getPacsID()
                    );

                   
    m_DBConnect->releaseLock();
    
    state = constructState(i);
    return state;                    
}

/** Retorna un objecte PacsList amb tots els Pacs que hi ha la taula PacsList odernats per AEtitle. Nomes selecciona els pacs vius, els que estan en estat
  * de baixa no els selecciona
  *        @param PacsList Conté tots els Pacs de la taula PacsList 
  *        @return estat de l'operació
  */
Status PacsListDB::queryPacsList(PacsList &list)
{
    int col,rows,i = 0,estat;
    PacsParameters pacs;
    std::string sql;

    char **resposta = NULL,**error = NULL;
    Status state;

    sql.insert(0,"select AETitle, Server, Port, Inst, Loc, Desc, Def,PacsID ");
    sql.append("from PacsList ");
    sql.append("where del = 'N' ");
    sql.append("order by AETitle");
    
    m_DBConnect->getLock();
    estat = sqlite_get_table(m_DBConnect->getConnection(),sql.c_str(),&resposta,&rows,&col,error); //connexio a la bdd,sentencia sql,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();
    state=constructState(estat);
    
    if (!state.good()) return state;
    
    i = 1;//ignorem les capçaleres
    while (i <= rows)
    {   
        pacs.setAEPacs(resposta[0 + i*col]);
        pacs.setPacsAdr(resposta[1 + i*col]);
        pacs.setPacsPort(resposta[2 + i*col]);
        pacs.setInstitution(resposta[3 + i*col]);
        pacs.setLocation(resposta[4 + i*col]);
        pacs.setDescription(resposta[5 + i*col]);
        pacs.setDefault(resposta[6 + i*col]);
        pacs.setPacsID(atoi(resposta[7 + i*col]));
        
        i++;
        list.insertPacs(pacs);
    }
    
    return state;
}

/**Cerca la informació d'un pacs en concret. 
  *        @param [out] Conté la informació del pacs cercat
  *        @param pacs a cercar 
  *        @return estat de l'operació
  */
Status PacsListDB::queryPacs(PacsParameters *pacs,std::string AETitle)
{

    int col,rows = 0,i = 0,estat;
    std::string sql;
    char id[6];

    char **resposta = NULL,**error = NULL;
    Status state;

    sql.insert(0,"select AETitle, Server, Port, Inst, Loc, Desc, Def,PacsID ");
    sql.append("from PacsList ");
    if (pacs->getPacsID()>0)
    {
        sprintf(id,"%i",pacs->getPacsID());
        sql.append(" where PacsID = ");
        sql.append(id);
    }
    else 
    {
        sql.append(" where AEtitle = '");
        sql.append(AETitle.c_str());
        sql.append("'");
    }

    
    m_DBConnect->getLock();
    estat = sqlite_get_table(m_DBConnect->getConnection(),sql.c_str(),&resposta,&rows,&col,error); //connexio a la bdd,sentencia sql,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();
    
    //sqlite no té estat per indica que no s'ha trobat dades, li assigno jo aquest estat!!
    if (rows == 0 && estat == 0) estat = 99;
    state=constructState(estat);
    
    if (!state.good()) return state;
    if (rows > 0)
    {
        i = 1;//ignorem les capçaleres
        pacs->setAEPacs(resposta[0 + i*col]);
        pacs->setPacsAdr(resposta[1 + i*col]);
        pacs->setPacsPort(resposta[2 + i*col]);
        pacs->setInstitution(resposta[3 + i*col]);
        pacs->setLocation(resposta[4 + i*col]);
        pacs->setDescription(resposta[5 + i*col]);
        pacs->setDefault(resposta[6 + i*col]);
        pacs->setPacsID(atoi(resposta[7 + i*col]));    
    }
    return state;

}

/** Es donarà de baixa el Pacs de l'objecte PacsParameters passat. No es dona de baixa físicament, sinó que a la bd es posa en estat de baixa,
  * ja que a la caché podem tenir estudis que hi facin referència
  * De PacsParameters només cal omplir el PacsID (camp clau= per esborrar el PACS
  *        @param  Objecte PacsParamets amb el PacsID del pacs a esborrar
  *        @return estat de l'operació  
  */
Status PacsListDB::deletePacs(PacsParameters *pacs)
{
    Status state;
    int i;
    std::string sql;
   
    sql.insert(0,"update PacsList  set Del = 'S'");
    sql.append (" where PacsID = %i");
    
    m_DBConnect->getLock();
    i = sqlite_exec_printf(m_DBConnect->getConnection(),sql.c_str(),0,0,0,
                    pacs->getPacsID()
                    );

          
    m_DBConnect->releaseLock();
        
    state = constructState(i);
    return state;              
}

/** Comprova si el pacs existeix en estat de baixa, comprovem si el AETitle està en estat donat de baixa
  *        @param PacsList [in/out] Conté tots els Pacs de la taula PacsList 
  *        @return estat de l'operació
  */
Status PacsListDB::queryPacsDeleted(PacsParameters *pacs)
{
    int col,rows = 0,estat;
    std::string sql;
    char **resposta = NULL,**error = NULL;
    Status state;

    sql.insert(0,"select PacsID ");
    sql.append("from PacsList ");
    sql.append(" where AEtitle = '");
    sql.append(pacs->getAEPacs());
    sql.append("' and Del = 'S'");
    
    m_DBConnect->getLock();
    estat=sqlite_get_table(m_DBConnect->getConnection(),sql.c_str(),&resposta,&rows,&col,error); //connexio a la bdd,sentencia sql,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();
    
    //sqlite no té estat per indica que no s'ha trobat dades, li assigno jo aquest estat!!
    if (rows == 0 && estat == 0) estat = 99;
    
    if (rows > 0) pacs->setPacsID(atoi(resposta[1]));
    state=constructState(estat);
    
    return state;

}

PacsListDB::~PacsListDB()
{

}

};
