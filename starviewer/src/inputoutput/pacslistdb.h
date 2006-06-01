/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGPACSLISTDB_H
#define UDGPACSLISTDB_H

#include <sqlite.h>
#include "databaseconnection.h"
#include "pacslist.h"

class string;
class PacsParameters;

namespace udg {

/** Aquesta classe implementa les accions necessaries per afegir nous pacs o modificar/consultar els paràmetres dels PACS  que tenim disponible a l'aplicació, guardats a la base de dades local
@author marc
*/

class Status;

class PacsListDB{

public:

    ///Constructor de la classe
    PacsListDB( );
    
    ///Destructor de la classe
    ~PacsListDB( );

    /** Insereix els parametres d'un pacs a la base de dades, per a poder-hi cercar imatge. En un alta el camp PacsID, és assignat automàticament per l'aplicació!
     * @param Objecte PacsParameters amb les dades del pacs
     * @return estat de l'operació  
     */
    Status insertPacs( PacsParameters *pacs );
    
    /** Retorna un objecte PacsList amb tots els Pacs que hi ha la taula PacsList odernats per AEtitle. Nomes selecciona els pacs vius, els que estan en estat de baixa no els selecciona
     * @param PacsList Conté tots els Pacs de la taula PacsList 
     * @return estat de l'operació
     */
    Status queryPacsList( PacsList & );
    
    /** Permet actualitzar la informació d'un pacs, el PacsID camp clau no es pot canviar!
     * @param Objecte PAcsParameters ambles noves dades del PACS
     * @return estat de l'operació  
     */
    Status updatePacs( PacsParameters *pacs );
    
    /** Es donarà de baixa el Pacs de l'objecte PacsParameters passat. No es dona de baixa físicament, sinó que a la bd es posa en estat de baixa, ja que a la caché podem tenir estudis que hi facin referència, De PacsParameters només cal omplir el PacsID (camp clau= per esborrar el PACS
     * @param  Objecte PacsParamets amb el PacsID del pacs a esborrar
     * @return estat de l'operació  
     */
    Status deletePacs( PacsParameters *pacs );
    
    /** Cerca la informació d'un pacs en concret. 
     * @param Conté la informació del pacs cercat
     * @param pacs a cercar 
     * @return estat de l'operació
     */
    Status queryPacs( PacsParameters *pacs , std::string AETitle );
    
    /** Cerca la informació d'un pacs en concret. 
     * @param Conté la informació del pacs cercat
     * @param pacs a cercar 
     * @return estat de l'operació
     */
    Status queryPacs( PacsParameters *pacs , int pacsID );
    
private:
    
    DatabaseConnection *m_DBConnect;    
        
    /** Construeix l'estat en que ha finaltizat l'operació sol·licitada
     * @param Estat de sqlite
     * @return retorna l'estat de l'operació
     */
    Status constructState( int );
    
    /** Comprova si el pacs existeix en estat de baixa, comprovem si el AETitle està en estat donat de baixa
     * @param Pacs a Trobar 
     * @return estat de l'operació
     */
    Status queryPacsDeleted( PacsParameters *pacs );
    
};

};

#endif
