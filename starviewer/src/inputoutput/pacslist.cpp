/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "pacslist.h"
#include <string>

namespace udg {

PacsList::PacsList()
{
}

/** Inserta un objecte PacsParameters a la llistat
  *    @param Objecte PacsParameters
  */
void PacsList::insertPacs(PacsParameters pacs)
{
    m_listPacs.push_back(pacs);
}

/** Posiciona l'iterador de la llistat a la primera posició
  */
void PacsList::firstPacs()
{
    i = m_listPacs.begin();
}

/** Apunta al següent Objecte PacsParameters
  */
void PacsList::nextPacs()
{
    i++;
}

/** cerca un pacs a la llista a través del seu AETitle
  *        @param AETitle del pacs a cercar
  */
bool PacsList::findPacs(std::string AETitle)
{

    i = m_listPacs.begin();
    if (i == m_listPacs.end()) return false;
    
    
    while (i != m_listPacs.end())
    {
        if ((*i).getAEPacs() == AETitle) break;
        else i++;
    }
    return (i != m_listPacs.end());
}

/** Retorna el número d'elements de la llista
  *    @return Retorna el número d'elements de la llista
  */
int PacsList::size()
{
    return m_listPacs.size();
}

/** Indica si hem arribal al final de la llista
  *    @return indica si s'ha arribat al final de la llista
  */
bool PacsList::end()
{
    return i == m_listPacs.end();
}

/** Retorna l'objecte pacs al que apunta l'iterador de la llista
  *        @return Objecte PacsParameters
  */
PacsParameters PacsList::getPacs()
{
    return (*i);
}

/** Buida la llista
  */
void PacsList::clear()
{
    m_listPacs.clear();
}

PacsList::~PacsList()
{
}

};
