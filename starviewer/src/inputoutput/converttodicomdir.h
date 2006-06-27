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

namespace udg {

class Status;
class Series;
class Image;

/** Converteix un estudi a DICOMDIR, invocant el mètodes i classes necessàries
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ConvertToDicomDir : public QObject 
{
public:

    ConvertToDicomDir( );

    /** Converteix l'estudi amb el UID passat per paràmetre a dicomDir
     * @param studyUID UID de l'estudi a convertir a DicomDir
     * @return estat del mètode
     */
    Status convert( QString studyUID );

    ~ConvertToDicomDir();

private :
    QProgressDialog *m_progress;

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
