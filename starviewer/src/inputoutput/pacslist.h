/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGPACSLIST_H
#define UDGPACSLIST_H

#include "pacsparameters.h"
#include <list>

class string;

namespace udg {

/** Aquesta classe implementa una llistat d'objectes PacsParameters.
@author marc
*/
class PacsList
{
public:
    PacsList();
    
	/** Inserta un objecte PacsParameters a la llistat
     * @param Objecte PacsParameters
     */
    void insertPacs( PacsParameters );
	
	/// Apunta al següent Objecte PacsParameters
	void nextPacs();

	/// Posiciona l'iterador de la llistat a la primera posició
	void firstPacs();
    
	/** cerca un pacs a la llista a través del seu AETitle
     * @param AETitle del pacs a cercar
     */
	bool findPacs( std::string );

	/** Retorna l'objecte pacs al que apunta l'iterador de la llista
     * @return Objecte PacsParameters
     */
	PacsParameters getPacs();
    
	/** Retorna el número d'elements de la llista
     * @return Retorna el número d'elements de la llista
     */    	
	int size();

	/// Buida la llista    
	void clear();   

	/** Indica si hem arribal al final de la llista
     * @return indica si s'ha arribat al final de la llista
     */
	bool end();
    
    ~PacsList();
    
private:

    list<PacsParameters>m_listPacs;     
    list<PacsParameters>::iterator i;

};
}; //end name space udg

#endif
