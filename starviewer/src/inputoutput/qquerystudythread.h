/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQQueryStudyThread_H
#define UDGQQueryStudyThread_H

#include <QThread>
#include <QList>
#include <QHash>

#include "dicommask.h"
#include "pacsdevice.h"

class QString;

namespace udg {

class PacsConnection;
class Patient;
class Study;
class Series;
class Image;

/** Classe que cercar estudis en un dispositiu pacs, creant un nou thread
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QQueryStudyThread :public QThread
{
    Q_OBJECT
public:

    /// Constructor de la classe
    QQueryStudyThread( QObject *parent = 0 );

    /** Accio que executa el thread
     * @param parametres del pacs a consultar
     * @param DicomMask màscara de cerca
     */
    void queryStudy( PacsDevice parameters , DicomMask mask );

    /// el codi d'aquest mètode es el que s'executa en un nou thread
    void run();

    ///Retorna la llista d'estudis trobats que compleixen el criteri de cerca
    QList<Patient*> getPatientStudyList();

    ///Retorna la llista de series trobades que compleixen els criteris de cerca
    QList<Series*> getSeriesList();

    QList<Image*> getImageList();

    ///Retorna un Hashtable que indica per l'UID de l'estudi a quin PACS pertany l'estudi
    QHash<QString,QString> getHashTablePacsIDOfStudyInstanceUID();

    ///Destructor de la classe
    ~QQueryStudyThread();

signals:
    /** signal que s'envia per indicar que hi hagut un error connectant al PACS
     * @param ID del pacs que ha produit l'error
     */
    void errorConnectingPacs( QString pacsID );

    /** signal que s'envia per indicar que hi hagut un error cercant al PACS
     * @param ID del pacs que ha produit l'error
     */
    void errorQueringStudiesPacs( QString pacsID );

private :
    PacsDevice m_param;
    DicomMask m_mask;

    QList<Patient*> m_patientStudyList;
    QList<Series*> m_seriesList;
    QList<Image*> m_imageList;
    QHash<QString,QString> m_hashPacsIDOfStudyInstanceUID; //Fa un relació d'StudyInstanceUID amb el pacs al qual pertany

};

}  //end namespace UdG

#endif
