/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "retrievethread.h"

namespace udg {

RetrieveThread::RetrieveThread()
{
}

/** Guarda la referència al thread encarregat de descarregar les imatges
  *    @param thread 
  */
void RetrieveThread::setThread(pthread_t thread)
{
    m_Thread = thread;
}

/** estableix el UID de l'estudi que es descarrega
  *    @param UID de l'estudi que es descarrega
  */
void RetrieveThread::setStudyUID(std::string studyUID)
{
    m_StudyUID = studyUID;
}

/** Indica si una vegada finalitzada la descarrega de l'estudi s'ha de visualitzar una serie de l'estudi, si el paràmetre es fals indica que nomes es tractava      * d'una descarrega, i que per tant no s'ha de visualitzar
  *    @param indica si l'estudi s'ha de visualitzar una vegada descarregat
  */
void RetrieveThread::setView(bool view)
{
    m_View = view;
}

/** Si una vegada finalitzada la descarrega s'ha de visualitzar l'estudi, s'ha d'especificar quina sèrie és la que s'ha de visualitzar
  *    @param serie a visualitzar
  */
void RetrieveThread::setDefaultSeriesUID(std::string seriesUID)
{
    m_DefaultSeriesUID = seriesUID;
}

/** Retorna la referencia al thread de la descarrega
  *     @return referencia al thread
  */
pthread_t RetrieveThread::getThread()
{
    return m_Thread;
}

/** Retorna el uid de l'estudi de la descarrega
  *    @return UID de l'estudi
  */
std::string RetrieveThread::getStudyUID()
{
    return m_StudyUID;
}

/** retorna si l'estudi una vegada descarregat s'ha de visualitzar
  *    @return cert si l'estudi s'ha de visualitzar
  */
bool RetrieveThread::getView()
{
    return m_View;
}

/** Retorna el uid de la sèrie que s'ha de visualitzar
  *    @retun UID de la sèrie
  */
std::string RetrieveThread::getDefaultSeriesUID()
{
    return m_DefaultSeriesUID;
}

RetrieveThread::~RetrieveThread()
{

}

};
