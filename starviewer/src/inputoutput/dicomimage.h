/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMIMAGE_H
#define UDGDICOMIMAGE_H

#include <QString>

#define HAVE_CONFIG_H 1
#include <dimse.h> // provide the structure DcmDataSet
#include <dcdeftag.h> //provide the information for the tags

namespace udg {

/** Classe image encarregada de guardar la informació d'una imatge
@author marc
*/
class DICOMImage{
public:

    /// Constructor de la classe
    DICOMImage();

    ///Constructor a partir d'un dataset provinent d'un objecte DICOM, no emplene el camps ImageFileName, ImagePath i ImageSize
    DICOMImage(DcmDataset *imageDataset);

    /// destructor de la classe
    ~DICOMImage();

    /** Inserta el UID de la Imatge
     * @param  UID de la imatge
     */
    void setSOPInstanceUID( QString );

    /** Inserta el UID de la sèrie al qual pertany la imatge
     * @param  UID de la sèrie a la que pertany la imatge
     */
    void setSeriesUID (QString );

    /** Inserta el path de la imatge a l'ordinador local
     * @param path de la imatge
     */
    void setImagePath (QString );

    /** Inserta el UID de la l'estudi al qual pertany la imatge
     * @param UID  UID de la l'estudi a la que pertany la imatge
     */
    void setStudyUID (QString );

    /** Inserta el nom de la imatge
     * @param name [in] nom de la imatge
     */
    void setImageName (QString );

    /** Inserta el número d'imatge
     * @param Número que ocupa la imatge dins la sèrie
     */
    void setImageNumber (int);

    /** Inserta el número de bytes que ocupa la imatge
     * @param  bytes de la imatge
     */
    void setImageSize (int);

    /** estableix el PACS al que pertany l'estudi
     * @param AETitle AETitle al que pertany la imatge
     */
    void setPacsAETitle( QString AETitle );

    /** Retorna el UID de la Imatge
     * @return  UID de la imatge
     */
    QString getSOPInstanceUID();

    /** Retorna el UID de la sèrie al qual pertany la imatge
     * @return UID de la sèrie a la que pertany la imatge
     */
    QString getSeriesUID();

    /** Retorna el path de la imatge a l'ordinador local
     * @return path de la imatge
     */
    QString getImagePath ();

    /** Retorna el UID de l'estudi al qual pertany la imatge
     * @return UID de la l'estudi a la que pertany la imatge
     */
    QString getStudyUID();

    /** Retorna el nom de la imatge
     * @return el nom de la imatge
     */
    QString getImageName();

    /** retorna el número d'imatge
     * @return Retorna el número que ocupa la imatge dins la sèrie
     */
    int getImageNumber();

    /** retorna el número de bytes que ocupa la imatge
     * @return retorna el número de bytes de la imatge
     */
    int getImageSize();

    /** retorna el AETitle de la imatge
     * @return AETitle de la imatge
     */
    QString getPacsAETitle();

private:
    //variables que guarden la informació de la imatge
    QString m_SoPUID;
    QString m_seriesUID;
    QString m_imagePath;
    QString m_studyUID;
    QString m_imageName;
    QString m_pacsAETitle;
    int m_imageNumber;
    int m_imageSize;

};

};

#endif
