/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef ERRORDCMTK_H
#define ERRORDCMTK_H

/** Classe en la que es defineixen constants d'errors de dcmtk
 */
#include <QString>
#include <ofcond.h>

namespace udg{

    const OFConditionConst DcmtkNoError( 0 , 0 , OF_ok, "Normal" );

    const OFConditionConst DcmtkUnknownError( 0 , 1000 , OF_error , "Unknown Error" );
    
    const OFConditionConst DcmtkNoConnectionError( 0 , 1120 , OF_error , "No connection has been set" );

    const OFConditionConst DcmtkNoMaskError( 0 , 1121 , OF_error , "No Mask has been set" );

    const OFConditionConst DcmtkMaskInsertTagError( 0 , 1127 , OF_error , "Error inserting tag in Mask" );

    const OFConditionConst DcmtkCanNotConnectError( 0 , 1200 , OF_error , "Can't connect" );

    const OFConditionConst DcmtkMoveDestionationUnknown(0, 1300, OF_error, "Move destination unknown");

    const OFConditionConst DcmtkMoveFailureOrRefusedStatus(0, 1301, OF_error, "Movescu receive a failure or refused status");

    const OFConditionConst DcmtkMoveWarningStatus(0, 1302, OF_error, "Movescu receive a warning status");

    const OFConditionConst DcmtkMovescuUnknownError(0, 1303, OF_error, "Movescu unknown error");

    const OFConditionConst DcmtkStorescuFailureStatus(0 , 1400, OF_error, "Storescu all images failed");

    const OFConditionConst DcmkStorescuSomeImagesFailedStatus(0, 1401, OF_error, "Storescu some images failed");

    const OFConditionConst DcmkStorescuWarningStatus(0, 1402, OF_error, "Storescu receive warning status");

}; //end namespace
#endif
