#ifndef UDG_PACS_CONST
#define UDG_PACS_CONST

#include <QString>

#include <ofcond.h>



/**      Codi d'errors
                0         - Exit
                1100-1199 - S'utilitza per descriure errors en la màscara de cerca
                1200-1299 - Errors relacionats amb la connexio
                1300-1399 - Errors relacionats amb l'operacio find

  */
namespace udg
{
    /*This constant is a OFCondition that used to indicat no errors*/
    const OFConditionConst CORRECT_DECLARED(          0, 0, OF_ok,      "Normal");
    const OFConditionConst CORRECT(CORRECT_DECLARED);

    const OFConditionConst correctDeclared(          0, 0, OF_ok,      "Normal");
    const OFConditionConst correct(correctDeclared);

    const OFConditionConst ERROR_DECLARED(          0,1000, OF_error,      "Unknow Error");
    const OFConditionConst ERROR(ERROR_DECLARED);

    const OFConditionConst errorDMaskPatienId(          0, 1100, OF_error,      "Error in Patient's Id Mask ");
    const OFConditionConst errorMaskPatientId(errorDMaskPatienId);

    const OFConditionConst errorDMaskPatientName(          0, 1101, OF_error,      "Error in Patient's Name Mask ");
    const OFConditionConst errorMaskPatientName(errorDMaskPatientName);

    const OFConditionConst errorDMaskPatientBirth(          0, 1102, OF_error,      "112Error in Patient's Birth Mask ");
    const OFConditionConst errorMaskPatientBirth(errorDMaskPatientBirth);

    const OFConditionConst errorDMaskPatientSex(          0, 1103, OF_error,      "Error in Patient's Sex Mask ");
    const OFConditionConst errorMaskPatientSex(errorDMaskPatientSex);

    const OFConditionConst errorDMaskStudyId(          0, 1104, OF_error,      "Error in Study's Id Mask");
    const OFConditionConst errorMaskStudyId(errorDMaskStudyId);

    const OFConditionConst errorDMaskStudyDate(          0, 1105, OF_error,      "Error in Study's Date Mask ");
    const OFConditionConst errorMaskStudyDate(errorDMaskStudyDate);

    const OFConditionConst errorDMaskStudyDescription(          0, 1106, OF_error,      "Error in Study's Description Mask ");
    const OFConditionConst errorMaskStudyDescription(errorMaskStudyDescription);

    const OFConditionConst error_DMaskStudyModality(          0, 1107, OF_error,      "Error in Study's Modalities Mask ");
    const OFConditionConst error_MaskStudyModality(error_DMaskStudyModality);

    const OFConditionConst error_DMaskStudyTime(          0, 1108, OF_error,      "Error in Study's Time Mask ");
    const OFConditionConst error_MaskStudyTime(error_DMaskStudyTime);

    const OFConditionConst error_DMaskStudyUID(          0, 1109, OF_error,      "Error in Study's UID Mask ");
    const OFConditionConst error_MaskStudyUID(error_DMaskStudyUID);

    const OFConditionConst error_DMaskInstitutionName(          0, 1110, OF_error,      "Error in Institution name Mask ");
    const OFConditionConst error_MaskInstitutionName(error_DMaskInstitutionName);

    const OFConditionConst error_DMaskAccessionNumber(          0, 1111, OF_error,      "Error in Accession Number Mask ");
    const OFConditionConst error_MaskAccessionNumber(error_DMaskAccessionNumber);

    const OFConditionConst error_DMaskLengthDate(          0, 1112, OF_error,      "Incorrect length of the date. The correct format is YYYYMMDD.");
    const OFConditionConst error_MaskLengthDate(error_DMaskLengthDate);

    const OFConditionConst error_DMaskLengthTime(          0, 1113, OF_error,      "Incorrect length of the time. The correct format is HHMM.");
    const OFConditionConst error_MaskLengthTime(error_DMaskLengthTime);

    const OFConditionConst error_DMaskSeriesUID(          0, 1114, OF_error,      "Error in Series's UID Mask ");
    const OFConditionConst error_MaskSeriesUID(error_DMaskSeriesUID);

    const OFConditionConst errorDMaskSeriesNumber(          0, 1115, OF_error,      "Error in Series Numbe Mask");
    const OFConditionConst errorMaskSeriesNumber(errorDMaskSeriesNumber);

    const OFConditionConst errorDMaskSeriesDate(          0, 1116, OF_error,      "Error in Series Date Mask ");
    const OFConditionConst errorMaskSeriesDate(errorDMaskSeriesDate);

    const OFConditionConst errorDMaskSeriesDescription(          0, 1117, OF_error,      "Error in Series Description Mask ");
    const OFConditionConst errorMaskSeriesDescription(errorMaskSeriesDescription);

    const OFConditionConst error_DMaskSeriesModality(          0, 1118, OF_error,      "Error in Series Modalities Mask ");
    const OFConditionConst error_MaskSeriesModality(error_DMaskSeriesModality);

    const OFConditionConst error_DMaskSeriesTime(          0, 1119, OF_error,      "Error in Series's Time Mask ");
    const OFConditionConst error_MaskSeriesTime(error_DMaskSeriesTime);

    const OFConditionConst error_DNoConnection(          0, 1120, OF_error,      "Not connection has been setting");
    const OFConditionConst error_NoConnection (error_DNoConnection);

    const OFConditionConst error_DNoMask(          0, 1121, OF_error,      "Not Mask has been setting");
    const OFConditionConst error_NoMask (error_DNoMask);

    const OFConditionConst error_DMaskBodyPartExaminated(          0, 1122, OF_error,      "Error in Body Part Examined Mask");
    const OFConditionConst error_MaskBodyPartExaminated (error_DMaskBodyPartExaminated);

    const OFConditionConst error_DMaskOperatorName(          0, 1123, OF_error,      "Error in Operator Name Mask");
    const OFConditionConst error_MaskOperatorName (error_DMaskOperatorName);

    const OFConditionConst error_DMaskProtocolName(          0, 1124, OF_error,      "Error in Body Protocol Name Mask");
    const OFConditionConst error_MaskProtocolName (error_DMaskProtocolName);

    const OFConditionConst errorDMaskPatientAge(          0, 1125, OF_error,      "Error in PatientAge Mask");
    const OFConditionConst errorMaskPatientAge (errorDMaskPatientAge);

    const OFConditionConst errorDMaskInstanceNumber(          0, 1126, OF_error,      "Error in ImageNumber Mask");
    const OFConditionConst error_MaskInstanceNumber (errorDMaskInstanceNumber);

    const OFConditionConst errorDMaskInsertTag(          0, 1127, OF_error,      "Error inserting tag in Mask");
    const OFConditionConst error_MaskInsertTag  (errorDMaskInsertTag );

    const OFConditionConst error_DNoConnect(          0, 1200, OF_error,      "Can't connect");
    const OFConditionConst error_NoConnect (error_DNoConnect);

    const OFConditionConst error_DNoLocalAdr(          0, 1201, OF_error,      "I can't get the local adress");
    const OFConditionConst error_NoLocalAdr (error_DNoLocalAdr);

    const OFConditionConst error_DNoFind(          0, 1300, OF_error,      "There are problems with the find operation");
    const OFConditionConst error_NoFind (error_DNoConnect);

    const int errorDictionaryNoLoaded = 1000;
    const int errorOpeningDicomFile = 1001;
    const int errorOutofMemory = 1002;
    const int errorDicomFileNotFound = 1003;
    const int errorOpeningNewImage = 1004;
    const int errorScalingImage = 1005;
    const int errorWritingNewImage = 1006;


    //Prioritats pers les operacions de la classe Operation
    const int operationPriorityLow = 99;
    const int operationPriorityMedium = 50;
    const int operationPriorityHigh = 0;

    //Defininim les operacions possibles de la classe Operation
    const int operationUnknow = 0;
    const int operationRetrieve = 1;
    const int operationPrint = 2;
    const int operationMove = 3;
    const int operationView = 4;

    enum recordDeviceDicomDir { harddisk , cd , dvd , usb };
}


/**********************************************************************************************************************************************************/


#endif
