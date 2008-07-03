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

#include "dicomstudy.h"
#include "dicomseries.h"
#include "dicomimage.h"
#include "dicommask.h"
#include "pacsparameters.h"

class QString;

namespace udg {

class PacsConnection;

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
    void queryStudy( PacsParameters parameters , DicomMask mask );

    /// el codi d'aquest mètode es el que s'executa en un nou thread
    void run();

    ///Retorna la llista d'estudis trobats que compleixen el criteri de cerca
    QList<DICOMStudy> getStudyList();

    ///Retorna la llista de series trobades que compleixen els criteris de cerca
    QList<DICOMSeries> getSeriesList();

    QList<DICOMImage> getImageList();

    ///Destructor de la classe
    ~QQueryStudyThread();

signals:
    /** signal que s'envia per indicar que hi hagut un error connectant al PACS
     * @param ID del pacs que ha produit l'error
     */
    void errorConnectingPacs( int pacsID );

    /** signal que s'envia per indicar que hi hagut un error cercant al PACS
     * @param ID del pacs que ha produit l'error
     */
    void errorQueringStudiesPacs( int pacsID );

private :
    PacsParameters m_param;
    DicomMask m_mask;

    QList<DICOMStudy> m_studyList;
    QList<DICOMSeries> m_seriesList;
    QList<DICOMImage> m_imageList;

};

}  //end namespace UdG

#endif
