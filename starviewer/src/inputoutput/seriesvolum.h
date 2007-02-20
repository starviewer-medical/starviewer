/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSERIESVOLUM_H
#define UDGSERIESVOLUM_H

#include <vector>

class string;

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
    void addImage( std::string );

    /** Estableix el UID de l'estudi al que pertany la serie
     * @param string UID de l'estudi
     */
    void setStudyUID( std::string );

    /** Estableix el UID de la sèrie al que pertany la serie
     * @param string UID de la sèrie
     */
    void setSeriesUID( std::string );

    /** Estableix el Id de l'estudi al que pertany la serie
     * @param string Id de l'estudi
     */
    void setStudyId( std::string );

    /** Estableix l'Id de la serie
     * @param string Id de la serie
     */
    void setSeriesId( std::string );

    /** Estableix la modalitat de la serie
     * @param string modalitat de la serie
     */
    void setSeriesModality( std::string );

    /** Estableix el path d'una serie
     * @param string path de la serie
     */
    void setSeriesPath( std::string );

    /** retorna el UID de la serie
     * @return UID de la serie
     */
    std::string getSeriesUID();

    /** retorna el UID de l'estudi
     * @return UID de l'estudi
     */
    std::string getStudyUID();

    /** retorna el Id de la serie
     * @return Id de la serie
     */
    std::string getSeriesId();

    /** retorna el Id de l'estudi
     * @return Id de l'estudi
     */
    std::string getStudyId();

    /** retorna la modalitat de la serie
     * @return modalitat de la serie
     */
    std::string getSeriesModality();

    /** retorna el path de la serie
     * @return el path de la serie
     */
    std::string getSeriesPath();

    /** retorna el nombre d'imatges de la sèrie
     * @return número d'imatges de les series
     */
    int getNumberOfImages();

    /** retorna un vector amb el path de totes les imatges que conte la sèrie
      * @return vector amb el path de totes les imatges de la sèrie
      */
	std::vector <std::string>getVectorImagePath();

    ///Destructor de la classe
    ~SeriesVolum();

private:

    std::string m_seriesUID;
    std::string m_studyUID;
    std::string m_studyId;
    std::string m_seriesId;
    std::string m_seriesModality;
    std::string m_seriesPath;

    std::vector <std::string> m_vectorSeriesVolum;
};

};

#endif
