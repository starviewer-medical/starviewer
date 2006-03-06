/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "image.h"

namespace udg {

/** Constructor de la classe
 */
Image::Image()
{
}

/** Inserta el UID de la Imatge 
  *        @param  UID de la imatge
  */
void Image::setSoPUID(std::string UID)
{
    m_SoPUID.erase();
    m_SoPUID.insert(0,UID);
}

/** Inserta el path de la imatge a l'ordinador local
  *        @param path de la imatge
  */    
void Image::setImagePath(std::string path)
{
    m_imagePath.erase();
    m_imagePath.insert(0,path);
}

/** Inserta el UID de la sèrie al qual pertany la imatge
  *        @param  UID de la sèrie a la que pertany la imatge
  */
void Image::setSeriesUID(std::string UID)
{
    m_seriesUID.erase();
    m_seriesUID.insert(0,UID);
}

/** Inserta el nom de la imatge
  *        @param name [in] nom de la imatge
  */
void Image::setImageName(std::string name)
{
    m_imageName.erase();
    m_imageName.insert(0,name);
}

/** Inserta el UID de la l'estudi al qual pertany la imatge
  *        @param UID  UID de la l'estudi a la que pertany la imatge
  */
void Image::setStudyUID(std::string UID)
{
    m_studyUID.erase();
    m_studyUID.insert(0,UID);
}

/** Inserta el número d'imatge
  *        @param Número que ocupa la imatge dins la sèrie
  */
void Image::setImageNumber(int imageNum)
{
    m_imageNumber=imageNum;
}

/** Inserta el número de bytes que ocupa la imatge
  *        @param  bytes de la imatge
  */
void Image::setImageSize(int bytes)
{
    m_imageSize=bytes;
}

/** Retorna el UID de la Imatge 
  *        @return  UID de la imatge
  */
std::string Image::getSoPUID()
{
    return m_SoPUID;
}

/** Retorna el path de la imatge a l'ordinador local
  *        @return path de la imatge
  */    
std::string Image::getImagePath()
{
    return m_imagePath;
}

/** Retorna el UID de la sèrie al qual pertany la imatge
  *        @return UID de la sèrie a la que pertany la imatge
  */
std::string Image::getSeriesUID()
{
    return m_seriesUID;
}

/** Retorna el nom de la imatge
  *        @return el nom de la imatge
  */
std::string Image::getImageName()
{
    return m_imageName;
}

/** Retorna el UID de l'estudi al qual pertany la imatge
  *        @return UID de la l'estudi a la que pertany la imatge
  */
std::string Image::getStudyUID()
{
    return m_studyUID;
}


/** retorna el número d'imatge
  *        @return Retorna el número que ocupa la imatge dins la sèrie
  */
int Image::getImageNumber()
{
    return m_imageNumber;
}


/** retorna el número de bytes que ocupa la imatge
  *        @return retorna el número de bytes de la imatge
  */
int Image::getImageSize()
{
    return m_imageSize;
}

/** Destructor de la Classe
  */
Image::~Image()
{
}


};
