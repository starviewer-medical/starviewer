/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGIMAGEDICOMINFORMATION_H
#define UDGIMAGEDICOMINFORMATION_H

#include <QString>

#define HAVE_CONFIG_H 1
#include <dcfilefo.h> //obre un fitxer dicom
#include "dicomtagreader.h"

namespace udg {

/** Aquesta classe retorna els tags DICOM sollicitats d'una imatge
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class Status;

class ImageDicomInformation{
public:

    /// Constructor de la classe
    ImageDicomInformation();

    /** Obre el DICOM del qual s'ha d'obtenir la informació
     *         @param ruta del fitxer
     */
    Status openDicomFile( QString filePath );

    /** Retorna el study UID de l'imatge
     *         @param study UID de l'imatge
     */
    QString getStudyUID();

    /** Retorna el ID de l'estudi de l'imatge
     *         @param study ID de l'estudi de l'imatge
     */
    QString getStudyID();

    /** Retorna el series UID de l'imatge
     *         @param series UID de l'imatge
     */
    QString getSeriesUID();

    /** Retorna el series number de l'imatge
     *         @param series number
     */
    QString getSeriesNumber();

    /** Retorna la modalitat de la sèrie
     *         @param modalitat de la sèrie
     */
    QString getSeriesModality();

    /** Retorna el nom del Procotol utilitzat
     *         @param Protocol utilitzat
     */
    QString getSeriesProtocolName();

    /** Retorna la descripció de la sèrie
     *         @param descripció de la sèrie
     */
    QString getSeriesDescription();

    /** Retorna la part del cos examinada
     *         @param part del cos examinada
     */
    QString getSeriesBodyPartExamined();

    /** Retorna l'hora en que s'ha capturat la serie
     *         @return hora en que s'ha capturat la serie
     */
    QString getSeriesTime();

    /** Retorna data en que s'ha capturat la sèrie
     *         @return data en que s'ha capturat la sèrie
     */
    QString getSeriesDate();

    /** Retorna el SOPInstanceUID d'una iamtge
     * @return SOPInstanceUID de la imatge
     */
    QString getSOPInstanceUID();

    /** Retorna el SOPClassUID de la imatge
     * @return Retorna el SOPClassUID
     */
    QString getSOPClassUID();

    /// Destructor de la classe
    ~ImageDicomInformation();

private :
    DICOMTagReader m_dicomTagReader;
};

}

#endif
