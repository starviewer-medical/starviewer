/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 

#include <QString>
#include <time.h>
#include <string>

#include "status.h"
#include "cachepacs.h"
#include "cachepool.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "studymask.h"
#include "seriesmask.h"
#include "imagemask.h"
#include "logging.h"

namespace udg {

CachePacs::CachePacs()
{
   m_DBConnect = DatabaseConnection::getDatabaseConnection();
}

Status CachePacs::constructState( int numState )
{
//A www.sqlite.org/c_interface.html hi ha al codificacio dels estats que retorna el sqlite
    Status state;
	QString logMessage, codeError;

    switch(numState)
    {//aqui tractem els errors que ens poden afectar de manera més directe, i els quals l'usuari pot intentar solucionbar                         
        case SQLITE_OK :        state.setStatus( "Normal" , true , 0 );
                                break;
        case SQLITE_ERROR :     state.setStatus( "Database is corrupted or SQL error syntax " , false , 2001 );
                                break;
        case SQLITE_BUSY :      state.setStatus( "Database is locked" , false , 2006 );
                                break;
        case SQLITE_CORRUPT :   state.setStatus( "Database corrupted" , false , 2011 );
                                break;
        case SQLITE_CONSTRAINT: state.setStatus( "The new register is duplicated" , false , 2019 );
                                break;
        case 50 :               state.setStatus( "Not connected to database" , false , 2050 );
                                break;
      //aquests errors en principi no es poden donar, pq l'aplicació no altera cap element de l'estructura, si es produeix algun
      //Error d'aquests en principi serà perquè la bdd està corrupte o problemes interns del SQLITE, fent Numerror-2000 de l'estat
      //a la pàgina de www.sqlite.org podrem saber de quin error es tracta.
        default :               state.setStatus( "SQLITE internal error" , false , 2000 + numState ); 
                                break;
    }

	if (  numState != SQLITE_OK )
	{
		logMessage = "Error a la cache número " + codeError.setNum( numState , 10 );
		ERROR_LOG( logMessage.toAscii().constData() );
	}

   return state;
}

/*********************************************************************************************************                                                MANTENIMENT DE LA CACHE                                     
 *********************************************************************************************************/

Status CachePacs::compactCachePacs()
{
    int i;
    Status state;
    std::string sql;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
    }
    
    sql.insert( 0 , "vacuum" );//amb l'acció vacuum es compacta la base de dades
    
    m_DBConnect->getLock();
    i = sqlite_exec_printf( m_DBConnect->getConnection(),sql.c_str() , 0 , 0 , 0 );
    m_DBConnect->releaseLock();
                                
    state = constructState( i );

    return state;
}

CachePacs::~CachePacs()
{
}

};
