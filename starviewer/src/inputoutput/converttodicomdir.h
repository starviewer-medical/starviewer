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
#include "const.h"

class QString;
class QStringList;

namespace udg {

class Status;
class DICOMSeries;
class Image;

/** Converteix un estudi a DICOMDIR, invocant el mètodes i classes necessàries.
  * Per crear un dicomdir, s'han de seguir les normes especificades a la IHE per PDI (portable data information) i DICOM : Aquestes normes són :
  * El nom dels directoris i imatges no pot ser de mes de 8 caràcters, i ha d'estar en majúscules
  * Les imatges no poden tenir extensió
  * S'ha de seguir l'estructura jeràrquica de directoris de Pacient/Estudi/Series/Imatges
  * La imatge ha d'estar en format littleEndian
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ConvertToDicomdir : public QObject
{
public:

    ConvertToDicomdir( );

    /** Afegeix un estudi a la llista per convertir-se a dicomsdir. Quan afageix l'estudi, l'afageix a la llista ordenats per pacient. Ja que els dicomdir s'han d'agrupar primerament per pacients
     * @param studyUID UID de l'estudi a convertir a dicomdir
     */
    void addStudy ( QString studyUID );

    /** Converteix a dicomdir en el path especificat els estudis que hi ha a la llista
     * @param dicomdirPath directori on es guardarà el dicomdir
     * @return Indica l'estat en què finalitza el mètode
     */
    Status convert( QString dicomdirPath, recordDeviceDicomDir selectedDevice );

    /** Crea un fitxer README.TXT, amb informació sobre quina institució ha generat el dicomdir per quan es grava en un cd o dvd en el path que se li especifiqui.
      * En el cas que el txt es vulgui afegir en el mateix directori arrel on hi ha el dicomdir s'haura de fer després d'haver convertir el directori en un dicomdir, si es fes abans el mètode de convertir el directori a dicomdir fallaria, perquè no sabia com tractar el README.txt
      */
    void createReadmeTxt();


    ~ConvertToDicomdir();

private :

    /*Estructura que conté la informació d'un estudi a convertir a dicomdir, és necessari guardar el Patient ID perquè segons la normativa del IHE, els estudis s'han d'agrupar per id de pacient*/
    struct StudyToConvert
        {
            QString patientId;
            QString studyUID;
        };

    QList<StudyToConvert> m_studiesToConvert;
    QProgressDialog *m_progress;

    QString m_dicomDirPath;
    QString m_dicomDirStudyPath;
    QString m_dicomDirSeriesPath;
    QString m_oldPatientId;
    QString m_dicomdirPatientPath;

    QStringList m_patientDirectories;

    int m_patient;
    int m_study;
    int m_series;
    int m_image;

    /** crea un dicomdir, al directori especificat
     * @param dicomdirPath lloc a crear el dicomdir
     * @param selectedDevice dispositiu on es crearà el dicomdir
     * @return  estat del mètode
     */
    Status createDicomdir( QString dicomdirPath, recordDeviceDicomDir selectedDevice );

    /// Copia els estudis seleccionats per passar a dicomdir, al directori desti
    Status copyStudiesToDicomdirPath();

    /** Converteix un estudi al format littleendian, i la copia al directori dicomdir
     * @param studyUID Uid de l'estudi a convertir
     * @return Indica l'estat en què finalitza el mètode
     */
    Status copyStudyToDicomdirPath( QString studyUID );

    /** Converteix una sèrie al format littleendian, i la copia al directori dicomdir
     * @param series
     * @return Indica l'estat en què finalitza el mètode
     */
    Status copySeriesToDicomdirPath( DICOMSeries series );

    /** Converteix una imatge al format littleendian, i la copia al directori dicomdir
     * @param image
     * @return Indica l'estat en què finalitza el mètode
     */
    Status copyImageToDicomdirPath( Image image );

    /// esborra els estudis creats en el dicomdir, en el cas que s'haig produít algun error, per deixar el directori on s'havia de crear el dicomdir amb l'estat original
    void deleteStudies();

};

}

#endif
