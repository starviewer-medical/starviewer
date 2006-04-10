#include "studylist.h"
#include <list>
#include <unistd.h>
#include <stdio.h>
#include <iostream>

namespace udg
{
/*Tenint en compte que es podran realitzar multiples cerques en diferents pacs alhora, això implica que tindrem diferents threads executant-se
   que hauran d'inserir studis a la llista d'estudis, això fa necessari que la part d'inserir un nou estudi a la llista s'hagi de fer en execlusió
   mutua, ja que com es pot consultatr a 
 */
 
StudyList::StudyList()
{
    int init_value=1;//Només un thread alhora pot gravar a la llista
    m_semafor = (sem_t*)malloc(sizeof(sem_t));
    sem_init(m_semafor,0,init_value);
    m_iterator = m_listStudy.begin();
}    

/** This action inserts a Study at the end of the list
  *              @param  Study
  */
void StudyList::insert(Study study)
{
    sem_wait(m_semafor);
    m_listStudy.push_back(study);
    sem_post(m_semafor);
}

/**This actions put the pointer of the list at the firts study. 
  *It's very important before start to read, cals this action, else the pointer will be at the end of the list, and if we try to read,
  *we will produce a segmentation fault 
*/
void StudyList::firstStudy()
{
    m_listStudy.sort();
    m_iterator=m_listStudy.begin();
}

/** Move the pointer to the next Study
*/
void StudyList:: nextStudy()
{
    m_iterator++;
}

/** This functions returns true if the pointer of the list is at the end
  *        @return True if we have read all the studies
*/
bool StudyList::end()
{
    if (m_listStudy.size()==0)
    {
        return true;
    }
    else return m_iterator == m_listStudy.end();
}

/** Match a Study with the UID parameter, If study is matched with getStudy, can be retrieved the information of the study
  *         @UID UID of the study to match
  *         @return Return true if study is matched
  */
bool StudyList::findStudy(std::string UID)
{
    sem_wait(m_semafor);    
    m_iterator=m_listStudy.begin();
    if (m_iterator==m_listStudy.end()) return false;
    string b;
    b=(*m_iterator).getStudyUID();
    while (m_iterator != m_listStudy.end())
    {
        if ((*m_iterator).getStudyUID()==UID) break;
        else m_iterator++;
    }
    
    sem_post(m_semafor);
    return (m_iterator != m_listStudy.end());

}

/** Retorna la llista a la que esa apuntant l'iterador
  *        @return estudi a la que apunta la llista
  */
Study StudyList::getStudy()
{
    return (*m_iterator);
}

/** This function gets the number of studies in the list
  *              @return  number of studies in the list
  */
int StudyList::count()
{
    return m_listStudy.size();
}

void StudyList::clear()
{
    m_listStudy.clear();
}

}

