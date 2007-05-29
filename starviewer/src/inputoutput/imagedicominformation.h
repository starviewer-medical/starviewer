/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGIMAGEDICOMINFORMATION_H
#define UDGIMAGEDICOMINFORMATION_H

#define HAVE_CONFIG_H 1
#include <dcfilefo.h> //obre un fitxer dicom

namespace udg {

/** Aquesta classe retorna els tags DICOM sollicitats d'una imatge
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class Status;
class string;

class ImageDicomInformation{
public:

    /// Constructor de la classe
    ImageDicomInformation();

    /** Obre el DICOM del qual s'ha d'obtenir la informació
     *         @param ruta del fitxer
     */
    Status openDicomFile( std::string filePath );

    /** Retorna el study UID de l'imatge
     *         @param study UID de l'imatge
     */
    std::string getStudyUID();

    /** Retorna el ID de l'estudi de l'imatge
     *         @param study ID de l'estudi de l'imatge
     */
    std::string getStudyID();

    /** Retorna el series UID de l'imatge
     *         @param series UID de l'imatge
     */
    std::string getSeriesUID();

    /** Retorna el series number de l'imatge
     *         @param series number
     */
    std::string getSeriesNumber();

    /** Retorna la modalitat de la sèrie
     *         @param modalitat de la sèrie
     */
    std::string getSeriesModality();

    /** Retorna el nom del Procotol utilitzat
     *         @param Protocol utilitzat
     */
    std::string getSeriesProtocolName();

    /** Retorna la descripció de la sèrie
     *         @param descripció de la sèrie
     */
    std::string getSeriesDescription();

    /** Retorna la part del cos examinada
     *         @param part del cos examinada
     */
    std::string getSeriesBodyPartExamined();

    /** Retorna l'hora en que s'ha capturat la serie
     *         @return hora en que s'ha capturat la serie
     */
    std::string getSeriesTime();

    /** Retorna data en que s'ha capturat la sèrie
     *         @return data en que s'ha capturat la sèrie
     */
    std::string getSeriesDate();

    /** Retorna el SOPInstanceUID d'una iamtge
     * @return SOPInstanceUID de la imatge
     */
    std::string getSOPInstanceUID();

    /** Retorna el SOPClassUID de la imatge
     * @return Retorna el SOPClassUID
     */
    std::string getSOPClassUID();

    /// Destructor de la classe
    ~ImageDicomInformation();

private :

    DcmFileFormat *m_dicomFile;
    DcmDataset *m_dataset;
};

}

#endif
