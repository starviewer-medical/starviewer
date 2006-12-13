/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "imagemask.h"
#include "status.h"
#include <string>

namespace udg{

ImageMask::ImageMask()
{
    m_imageMask = new DcmDataset;
    retrieveLevel();

    //afegim els camps obligatoris
    setStudyUID( "" );
    setSeriesUID( "" );

}

//Per cada cerca a la màscara s'ha d'espeficiar el nivell al que anirem a buscar les dades, en aquest cas s'especifica image
void ImageMask:: retrieveLevel()
{
    char val[15];

    DcmElement *elem = newDicomElement( DCM_QueryRetrieveLevel );

    strcpy( val , "IMAGE" );
    elem->putString( val );

    m_imageMask->insert( elem , OFTrue );
    if ( m_imageMask->error() != EC_Normal )
    {
        printf("cannot insert tag: ");
    }

}

Status ImageMask:: setSeriesUID( std::string seriesUID )
{
    Status state;

    DcmElement *elem = newDicomElement( DCM_SeriesInstanceUID );

    elem->putString( seriesUID.c_str() );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskSeriesUID );
    }

    //insert the tag SERIES UID in the search mask
     m_imageMask->insert( elem , OFTrue);
    if ( m_imageMask->error() != EC_Normal )
    {
        return state.setStatus( error_MaskSeriesUID );
    }

    return state.setStatus( correct );
}

Status ImageMask:: setStudyUID( std::string studyUID )
{
    Status state;

    DcmElement *elem = newDicomElement( DCM_StudyInstanceUID );

    elem->putString( studyUID.c_str() );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskStudyUID );
    }

    //insert the tag STUDY UID in the search mask
    m_imageMask->insert( elem , OFTrue );
    if ( m_imageMask->error() != EC_Normal )
    {
        return state.setStatus( error_MaskStudyUID );
    }

    return state.setStatus(correct);
}

Status ImageMask:: setImageNumber( std::string imgNum )
{
    Status state;

    DcmElement *elem = newDicomElement( DCM_InstanceNumber );

    elem->putString( imgNum.c_str() );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskInstanceNumber );
    }

    //insert the tag SERIES UID in the search mask
    m_imageMask->insert( elem , OFTrue );
    if ( m_imageMask->error() != EC_Normal )
    {
        return state.setStatus( error_MaskInstanceNumber );
    }
    return state.setStatus( correct );
}

std::string ImageMask::getStudyUID()
{
    const char * UID=NULL;
    std::string studyUID;

    DcmTagKey studyUIDTagKey ( DCM_StudyInstanceUID );
    OFCondition ec;
    ec = m_imageMask->findAndGetString( studyUIDTagKey , UID , OFFalse );

    if ( UID != NULL ) studyUID.insert( 0 , UID );

    return studyUID;
}

std::string ImageMask::getSeriesUID()
{
    const char * UID = NULL;
    std::string seriesUID;

    DcmTagKey seriesUIDTagKey ( DCM_SeriesInstanceUID );
    OFCondition ec;
    ec = m_imageMask->findAndGetString( seriesUIDTagKey , UID , OFFalse );

    if ( UID != NULL ) seriesUID.insert( 0 , UID );

    return seriesUID;
}

std::string ImageMask::getImageNumber()
{
    const char * number = NULL;
    std::string imageNumber;

    DcmTagKey instanceNumberTagKey ( DCM_InstanceNumber );
    OFCondition ec;
    ec = m_imageMask->findAndGetString( instanceNumberTagKey , number , OFFalse );

    if ( number != NULL ) imageNumber.insert( 0 , number );

    return imageNumber;
}

DcmDataset* ImageMask::getImageMask()
{
    return m_imageMask;
}

};

