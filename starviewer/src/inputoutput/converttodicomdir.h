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
class Study;
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

    /** Crear un dicomdir en el path especificat per paràmetre amb els estudis que hi ha a la llista
     * @param dicomdirPath directori on es guardarà el dicomdir
     * @return Indica l'estat en què finalitza el mètode
     */
    Status convert( QString dicomdirPath );

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

    /** Converteix un estudi al format littleendian
     * @param studyUID Uid de l'estudi a convertir
     * @return Indica l'estat en què finalitza el mètode
     */
    Status convertStudy ( QString studyUID );

    /** Converteix una sèrie al format littleendian
     * @param series 
     * @return Indica l'estat en què finalitza el mètode
     */
    Status convertSeries( Series series );

    /** Converteix una imatge al format littleendian
     * @param image 
     * @return Indica l'estat en què finalitza el mètode
     */
    Status convertImage( Image image );    
};

}

#endif
