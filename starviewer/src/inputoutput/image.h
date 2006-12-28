/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGIMAGE_H
#define UDGIMAGE_H

#include <string>

namespace udg {

/** Classe image encarregada de guardar la informació d'una imatge
@author marc
*/
class Image{
public:
    
    /// Constructor de la classe
    Image();
       
    /// destructor de la classe
    ~Image();

    /** Inserta el UID de la Imatge 
     * @param  UID de la imatge
     */  
    void setSoPUID (std::string );
    
    /** Inserta el UID de la sèrie al qual pertany la imatge
     * @param  UID de la sèrie a la que pertany la imatge
     */
    void setSeriesUID (std::string );
    
    /** Inserta el path de la imatge a l'ordinador local
     * @param path de la imatge
     */
    void setImagePath (std::string );
    
    /** Inserta el UID de la l'estudi al qual pertany la imatge
     * @param UID  UID de la l'estudi a la que pertany la imatge
     */
    void setStudyUID (std::string );
    
    /** Inserta el nom de la imatge
     * @param name [in] nom de la imatge
     */
    void setImageName (std::string );
    
    /** Inserta el número d'imatge
     * @param Número que ocupa la imatge dins la sèrie
     */
    void setImageNumber (int);
    
    /** Inserta el número de bytes que ocupa la imatge
     * @param  bytes de la imatge
     */
    void setImageSize (int);
   
    /** Retorna el UID de la Imatge 
     * @return  UID de la imatge
     */
    std::string getSoPUID();
    
    /** Retorna el UID de la sèrie al qual pertany la imatge
     * @return UID de la sèrie a la que pertany la imatge
     */
    std::string getSeriesUID();
    
    /** Retorna el path de la imatge a l'ordinador local
     * @return path de la imatge
     */ 
    std::string getImagePath ();
    
    /** Retorna el UID de l'estudi al qual pertany la imatge
     * @return UID de la l'estudi a la que pertany la imatge
     */
    std::string getStudyUID();
    
    /** Retorna el nom de la imatge
     * @return el nom de la imatge
     */
    std::string getImageName();
    
    /** retorna el número d'imatge
     * @return Retorna el número que ocupa la imatge dins la sèrie
     */
    int getImageNumber();
    
    /** retorna el número de bytes que ocupa la imatge
     * @return retorna el número de bytes de la imatge
     */
    int getImageSize();
       
private:
    //variables que guarden la informació de la imatge
    std::string m_SoPUID;
    std::string m_seriesUID;
    std::string m_imagePath;
    std::string m_studyUID;
    std::string m_imageName;
    int m_imageNumber;
    int m_imageSize;

};

};

#endif
