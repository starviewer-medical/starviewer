/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCONVERTTODICOMDIR_H
#define UDGCONVERTTODICOMDIR_H
#include <QProgressDialog>
#include <QObject>

class QString;
class QStringList;

namespace udg {

class Status;
class Series;
class Image;

/** Converteix un estudi a DICOMDIR, invocant el mètodes i classes necessàries
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ConvertToDicomdir : public QObject 
{
public:

    ConvertToDicomdir( );

    
    /** Afegeix un estudi a la llista per convertir-se a dicomsdir
     * @param studyUID UID de l'estudi a convertir a dicomdir
     */
    void addStudy ( QString studyUID );

    /** Converteix l'estudi amb el UID passat per paràmetre a dicomDir
     * @param studyUID UID de l'estudi a convertir a DicomDir
     * @return estat del mètode
     */
    Status convert( QString studyUID );

    ~ConvertToDicomdir();

private :
    QProgressDialog *m_progress;

    QStringList m_studiesToConvert;

    QString m_dicomDirPath;
    QString m_dicomDirStudyPath;
    QString m_dicomDirSeriesPath;

    int m_study;
    int m_series;
    int m_image;

    Status convertSeries( Series series );
    Status convertImage( Image image );    
};

}

#endif
