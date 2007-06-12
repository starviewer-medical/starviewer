/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSERIESVOLUM_H
#define UDGSERIESVOLUM_H

#include <QString>
#include <QStringList>

namespace udg {

/** Permet definir una sèrie d'un estudi, i indicar el path on es troben les imatges en el disc local
@author marc
*/
class SeriesVolum
{

public:

    ///Constructor de la classe
    SeriesVolum();

    /** Afegeix el Path d'una imatge a la sèrie
     * @param string path de la imatge
     */
    void addImage( QString imagePath );

    /** Estableix el UID de l'estudi al que pertany la serie
     * @param string UID de l'estudi
     */
    void setStudyUID( QString );

    /** Estableix el UID de la sèrie al que pertany la serie
     * @param string UID de la sèrie
     */
    void setSeriesUID( QString );

    /** Estableix el Id de l'estudi al que pertany la serie
     * @param string Id de l'estudi
     */
    void setStudyId( QString );

    /** Estableix l'Id de la serie
     * @param string Id de la serie
     */
    void setSeriesId( QString );

    /** Estableix la modalitat de la serie
     * @param string modalitat de la serie
     */
    void setSeriesModality( QString );

    /** Estableix el path d'una serie
     * @param string path de la serie
     */
    void setSeriesPath( QString );

    /** retorna el UID de la serie
     * @return UID de la serie
     */
    QString getSeriesUID();

    /** retorna el UID de l'estudi
     * @return UID de l'estudi
     */
    QString getStudyUID();

    /** retorna el Id de la serie
     * @return Id de la serie
     */
    QString getSeriesId();

    /** retorna el Id de l'estudi
     * @return Id de l'estudi
     */
    QString getStudyId();

    /** retorna la modalitat de la serie
     * @return modalitat de la serie
     */
    QString getSeriesModality();

    /** retorna el path de la serie
     * @return el path de la serie
     */
    QString getSeriesPath();

    /** retorna el nombre d'imatges de la sèrie
     * @return número d'imatges de les series
     */
    int getNumberOfImages();

    /** retorna una llista amb el path de totes les imatges que conte la sèrie
      * @return vector amb el path de totes les imatges de la sèrie
      */
	QStringList getImagesPathList();

    ///Destructor de la classe
    ~SeriesVolum();

private:

    QString m_seriesUID;
    QString m_studyUID;
    QString m_studyId;
    QString m_seriesId;
    QString m_seriesModality;
    QString m_seriesPath;

    /// Llista amb el path de les seves imatges. Aquest membre substitueix a l'obsolet std::vector <std::string> m_vectorSeriesVolum;
    QStringList m_seriesImagePathList;
};

};

#endif
