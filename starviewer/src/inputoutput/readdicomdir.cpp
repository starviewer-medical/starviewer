/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "readdicomdir.h"

#include "ofstring.h"
#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcdeftag.h" //provide the information for the tags
#include "status.h"
#include "dicomstudy.h"
#include "studylist.h"
#include "series.h"
#include "serieslist.h"
#include "dicommask.h"
#include "image.h"
#include "imagelist.h"

namespace udg {

ReadDicomdir::ReadDicomdir()
{
    m_dicomdir = NULL;

}

Status ReadDicomdir::open( QString dicomdirPath )
{
    Status state;
    QString dicomdirFilePath;

    //no existeix cap comanda per tancar un dicomdir, quan en volem obrir un de nou, l'única manera d'obrir un nou dicomdir, és a través del construtor de DcmDicomDir, passant el path per paràmetre, per això si ja existia un Dicomdir ober, fem un delete, per tancar-lo
    if ( m_dicomdir != NULL) delete m_dicomdir;

    m_dicomdirAbsolutePath = dicomdirPath;

    //per defecte la informació dels dicomdir es guarda en unfitxer, per obrir el dicomdir hem d'obrir aquest fitxer, que per defecte es diu DICOMDIR, per tant l'hem de concatenar amb el path del dicomdir, per poder accedir al fitxer
    dicomdirFilePath = dicomdirPath;
    dicomdirFilePath.append( "/DICOMDIR" );
    m_dicomdir = new DcmDicomDir( qPrintable(dicomdirFilePath) );

    return state.setStatus( m_dicomdir->error() );
}

//El dicomdir segueix una estructura d'abre on tenim n pacients, que tenen n estudis, que conté n series, i que conté n imatges, per llegir la informació hem d'accedir a través d'aquesta estructura d'arbre, primer llegim el primer pacient, amb el primer pacient, podem accedir el segon nivell de l'arbre, els estudis del pacient, i anar fent així fins arribar al nivell de baix de tot, les imatges,
Status ReadDicomdir::readStudies( StudyList &studyList , DicomMask studyMask )
{
    Status state;

    if ( m_dicomdir == NULL ) return state.setStatus( "Error: Not open dicomfile" , false , 1302 ); //FER RETORNAR STATUS AMB ERROR

    DcmDirectoryRecord *root = &( m_dicomdir->getRootRecord() );//accedim a l'estructura d'arbres del dicomdir
    DcmDirectoryRecord *patientRecord = root->getSub( 0 );//accedim al primer pacient
    OFString text;
    DICOMStudy study;

    //En aquest primer while accedim al patient Record a nivell de dades de pacient
    while ( patientRecord != NULL )
    {
        //Nom pacient
        patientRecord->findAndGetOFStringArray( DCM_PatientsName , text );
        study.setPatientName( text.c_str() );
        //Id pacient
        patientRecord->findAndGetOFStringArray( DCM_PatientID , text );
        study.setPatientId( text.c_str() );

        DcmDirectoryRecord *studyRecord = patientRecord->getSub( 0 );//indiquem que volem el primer estudi del pacient

        //en aquest while accedim a les dades de l'estudi
        while ( studyRecord != NULL )
        {
            //Id estudi
            studyRecord->findAndGetOFStringArray( DCM_StudyID , text );
            study.setStudyId( text.c_str() );

            //Hora estudi
            studyRecord->findAndGetOFStringArray( DCM_StudyTime , text );
            study.setStudyTime( text.c_str() );

            //Data estudi
            studyRecord->findAndGetOFStringArray( DCM_StudyDate , text );
            study.setStudyDate( text.c_str() );

            //Descripció estudi
            studyRecord->findAndGetOFStringArray( DCM_StudyDescription , text );
            study.setStudyDescription( text.c_str() );

            //accession number
            studyRecord->findAndGetOFStringArray( DCM_AccessionNumber , text );
            study.setAccessionNumber( text.c_str() );

            //obtenim el UID de l'estudi
            studyRecord->findAndGetOFStringArray( DCM_StudyInstanceUID , text );
            study.setStudyUID( text.c_str() );


            if ( matchStudyMask( study , studyMask ) ) //comprovem si l'estudi compleix la màscara de cerca que ens han passat
            {
                studyList.insert( study );
            }

            studyRecord = patientRecord->nextSub( studyRecord ); //accedim al següent estudi del pacient
        }

        patientRecord = root->nextSub( patientRecord ); //accedim al següent pacient del dicomdir
    }

    return state.setStatus( m_dicomdir->error() );
}

//Per trobar les sèries d'une estudi haurem de recorre tots els estudis dels pacients, que hi hagi en el dicomdir, fins que obtinguem l'estudi amb el UID sol·licitat una vegada trobat, podrem accedir a la seva informacio de la sèrie
Status ReadDicomdir::readSeries( QString studyUID , QString seriesUID , SeriesList &seriesList )
{
    Status state;

    if ( m_dicomdir == NULL ) return state.setStatus( "Error: Not open dicomfile" , false , 1302 ); //FER

    DcmDirectoryRecord *root = &( m_dicomdir->getRootRecord() );//accedim a l'estructura d'abres del dicomdir
    DcmDirectoryRecord *patientRecord = root->getSub( 0 );//accedim al primer pacient
    DcmDirectoryRecord *studyRecord;
    OFString text;
    Series series;
    QString studyUIDRecord , seriesPath;
    bool trobat = false;

    //Accedim a nivell de pacient
    while ( patientRecord != NULL && !trobat )
    {
        studyRecord = patientRecord->getSub( 0 );//indiquem que volem el primer estudi del pacient

        while (  studyRecord != NULL && !trobat )
        {
            text.clear();
            studyUIDRecord.clear();
            studyRecord->findAndGetOFStringArray( DCM_StudyInstanceUID , text );//obtenim el UID de l'estudi al qual estem posicionats
            studyUIDRecord = text.c_str();
            if ( studyUIDRecord == studyUID ) //busquem l'estudi que continguin el mateix UID
            {
                trobat = true;
            }
            else studyRecord = patientRecord->nextSub( studyRecord );//si no trobem accedim al seguent estudi del pacient
        }

        if ( !trobat ) patientRecord = root->nextSub( patientRecord ); //accedim al següent pacient
    }

    if ( trobat )//si hem trobat l'estudi amb el UID que cercàvem
    {
        DcmDirectoryRecord *seriesRecord = studyRecord->getSub( 0 ); //seleccionem la serie de l'estudi que conté el studyUID que cercàvem

        series.setStudyUID( studyUID );

        while ( seriesRecord != NULL )
        {
            //UID Serie
            seriesRecord->findAndGetOFStringArray( DCM_SeriesInstanceUID , text );
            series.setSeriesUID( text.c_str() );

            if ( series.getSeriesUID() == seriesUID || seriesUID.length() == 0)
            {
                //Número de sèrie
                seriesRecord->findAndGetOFStringArray( DCM_SeriesNumber , text );
                series.setSeriesNumber( text.c_str() );

                //Modalitat sèrie
                seriesRecord->findAndGetOFStringArray( DCM_Modality , text );
                series.setSeriesModality( text.c_str() );

                //Per obtenir el directori de les series, no hi ha cap més manera que accedir. a la primera imatge de la serie i consultar-ne el directori

                DcmDirectoryRecord *imageRecord = seriesRecord->getSub( 0 );

                //Path de la imatge ens retorna el path absolut respecte el dicomdir DirectoriEstudi/DirectoriSeries/NomImatge. Atencio retorna els directoris separats per '/', per linux s'ha de transformar a '\'
                imageRecord->findAndGetOFStringArray( DCM_ReferencedFileID , text );
                seriesPath.clear();
                seriesPath.insert( 0 , text.c_str() );//Afegim la ruta de la primera imatge dins el dicomdir
                seriesPath = replaceBarra( seriesPath );
                seriesPath = seriesPath.mid( 0 , seriesPath.toStdString().rfind("/") + 1 );//Ignorem el nom de la primera imatge, nosaltres volem el directori de la sèrie
                series.setSeriesPath( seriesPath );

                seriesList.insert( series );//inserim a la llista de sèrie
            }
            seriesRecord = studyRecord->nextSub( seriesRecord ); //accedim a la següent sèrie de l'estudi
        }
    }

    return state.setStatus( m_dicomdir->error() );
}

Status ReadDicomdir::readImages( QString seriesUID , QString sopInstanceUID , ImageList &imageList )
{
    Status state;

    if ( m_dicomdir == NULL ) return state.setStatus( "Error: Not open dicomfile" , false , 1302 ); //FER

    DcmDirectoryRecord *root = &( m_dicomdir->getRootRecord() );//accedim a l'estructura d'abres del dicomdir
    DcmDirectoryRecord *patientRecord = root->getSub( 0 );//accedim al primer pacient
    DcmDirectoryRecord *studyRecord, *seriesRecord;
    OFString text;
    Series series;
    QString studyUIDRecord , seriesUIDRecord, imagePath;
    bool trobat = false;
    Image image;

    //Accedim a nivell de pacient
    while ( patientRecord != NULL && !trobat )
    {
        studyRecord = patientRecord->getSub( 0 );//indiquem que volem el primer estudi del pacient

        while (  studyRecord != NULL && !trobat )//accedim a nivell estudi
        {
            seriesRecord = studyRecord->getSub( 0 ); //seleccionem la serie de l'estudi que conté el studyUID que cercàvem
            while ( seriesRecord != NULL && !trobat )//accedim a nivell
            {
                //UID Serie
                text.clear();
                seriesUIDRecord.clear();
                seriesRecord->findAndGetOFStringArray( DCM_SeriesInstanceUID , text );
                seriesUIDRecord.insert( 0 , text.c_str() );
                if ( seriesUIDRecord == seriesUID ) //busquem la sèrie amb les imatges
                {
                    trobat = true;
                    image.setSeriesUID( seriesUIDRecord );//indiquem el seriesUID
                    studyRecord->findAndGetOFStringArray( DCM_StudyInstanceUID , text );
                    image.setStudyUID( text.c_str() );//Indiquem el studyUID de la imatge
                }
                else seriesRecord = studyRecord->nextSub( seriesRecord ); //accedim a la següent sèrie de l'estudi
            }
            studyRecord = patientRecord->nextSub( studyRecord );//si no trobem accedim al seguent estudi del pacient
        }

        if ( !trobat ) patientRecord = root->nextSub( patientRecord ); //accedim al següent pacient
    }

    if ( trobat )//si hem trobat la sèrie amb el UID que cercàvem
    {
        DcmDirectoryRecord *imageRecord = seriesRecord->getSub( 0 ); //seleccionem la serie de l'estudi que conté el studyUID que cercàvem

        while ( imageRecord != NULL )
        {
            //SopUid Image
            imageRecord->findAndGetOFStringArray( DCM_ReferencedSOPInstanceUIDInFile , text );
            image.setSOPInstanceUID( text.c_str() );

            if ( sopInstanceUID.length() == 0 || sopInstanceUID == image.getSOPInstanceUID() )
            {
                //Instance Number (Número d'imatge
                imageRecord->findAndGetOFStringArray( DCM_InstanceNumber , text );
                image.setImageNumber( atoi( text.c_str() ) );

                //Path de la imatge ens retorna el path relatiu respecte el dicomdir DirectoriEstudi/DirectoriSeries/NomImatge. Atencio retorna els directoris separats per '/', per linux s'ha de transformar a '\'
                imageRecord->findAndGetOFStringArray( DCM_ReferencedFileID , text );//obtenim el path relatiu de la imatge
                imagePath.clear();
                //creem el path absolut
                imagePath.insert( 0 , m_dicomdirAbsolutePath );
                imagePath.append( "/" ),
                imagePath.append( replaceBarra ( text.c_str() ) );
                image.setImagePath( imagePath );

                imageList.insert( image );//inserim a la llista la imatge*/
            }

            imageRecord = seriesRecord->nextSub( imageRecord ); //accedim a la següent imatge de la sèrie
        }
    }

    return state.setStatus( m_dicomdir->error() );
}

QString ReadDicomdir::getDicomdirPath()
{
    return m_dicomdirAbsolutePath;
}

//Per fer el match seguirem els criteris del PACS
bool ReadDicomdir::matchStudyMask( DICOMStudy study , DicomMask studyMask )
{
    if ( !matchStudyMaskStudyId( studyMask.getStudyId() , study.getStudyId() ) ) return false;

    if ( !matchStudyMaskPatientId( studyMask.getPatientId() , study.getPatientId() ) ) return false;

    if ( !matchStudyMaskDate( studyMask.getStudyDate() , study.getStudyDate() ) ) return false;

    if ( !matchStudyMaskPatientName( studyMask.getPatientName() , study.getPatientName() ) ) return false;

    if ( !matchStudyMaskStudyUID( studyMask.getStudyUID() , study.getStudyUID() ) ) return false;

    if ( !matchStudyMaskAccessionNumber( studyMask.getAccessionNumber() , study.getAccessionNumber() ) ) return false;

    return true;
}

bool ReadDicomdir::matchStudyMaskStudyId( QString studyMaskStudyId , QString studyStudyId )
{
    if ( studyMaskStudyId.length() > 0 )
    { //si hi ha màscara d'estudi Id
      //el id de l'estudi, des de la classe query screen el guardem a la màscara es amb format '*StudyID*'. Els '*' s'han de treure
        studyMaskStudyId = studyMaskStudyId.toUpper();
        studyStudyId = studyStudyId.toUpper();

        if ( !studyStudyId.contains( studyMaskStudyId ) )
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    return true;
}

bool ReadDicomdir::matchStudyMaskStudyUID( QString studyMaskStudyUID , QString studyStudyUID )
{
    if ( studyMaskStudyUID.length() > 0 )
    { //si hi ha màscara d'estudi UID
      //en el cas del StudiUID seguim criteri del pacs, només faran match els UID que concordin amb el de la màscara, no podem fer wildcard
        if ( studyStudyUID == studyMaskStudyUID )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool ReadDicomdir::matchStudyMaskPatientId( QString studyMaskPatientId , QString studyPatientId )
{
    if ( studyMaskPatientId.length() > 0 )
    { //si hi ha màscara Patient Id
      //el id del pacient, des de la classe query screen el guardem a la màscara es amb format '*PatientID*'. Els '*' s'han de treure

        studyMaskPatientId = studyMaskPatientId.toUpper();
        studyPatientId = studyPatientId.toUpper();

        if ( studyPatientId.contains( studyMaskPatientId ) )
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    return true;
}

bool ReadDicomdir::matchStudyMaskDate( QString studyMaskDate , QString studyDate )
{
    if ( studyMaskDate.length() > 0 )
    { //Si hi ha màscara de data
      //la màscara de la data per DICOM segueix els formats :
      // -  "YYYYMMDD-YYYYMMDD", per indicar un rang de dades
      // - "-YYYYMMDD" per buscar estudis amb la data més petita o igual
      // - "YYYYMMDD-" per buscar estudis amb la data més gran o igual
      // - "YYYYMMDD" per buscar estudis d'aquella data
      // Hurem de mirar quin d'aquest formats és la nostre màscara

        if (  studyMaskDate.length() == 8 ) // cas YYYYMMDDD
        {
            if ( studyMaskDate == studyDate )
            {
                return true;
            }
            else return false;
        }
        else if (  studyMaskDate.length() == 9 )
        {
            if (  studyMaskDate.at( 0 ) == '-' ) // cas -YYYYMMDD
            {
                if ( studyMaskDate.mid( 1 , 8 ) >= studyDate )
                {
                    return true;
                }
                else return false;
            }
            else if ( studyMaskDate.at( 8 ) == '-' ) // cas YYYYMMDD-
            {
                if ( studyMaskDate.mid( 0 , 8 ) <= studyDate )
                {
                    return true;
                }
                else return false;
            }
        }
        else if ( studyMaskDate.length() == 17 ) // cas YYYYMMDD-YYYYMMDD
        {
            if ( studyMaskDate.mid( 0 , 8 ) <= studyDate &&
                 studyMaskDate.mid( 9 , 8 ) >= studyDate )
            {
                return true;
            }
            else return false;
        }
        return false;
    }

    return true;
}

bool ReadDicomdir::matchStudyMaskPatientName( QString studyMaskPatientName , QString studyPatientName )
{
    QString lastPatientName , firstPatientName;

    if ( studyMaskPatientName.length() > 0 )
    {
      //Seguint els criteris del PACS la cerca es fa en wildcard, és a dir no cal que els dos string sigui igual mentre que la màscara del nom del pacient estigui continguda dins studyPatientName n'hi ha suficient
        studyMaskPatientName = studyMaskPatientName.toUpper();
        studyPatientName = studyPatientName.toUpper();

        if ( studyPatientName.contains( studyMaskPatientName ) )
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    return true;

}

bool ReadDicomdir::matchStudyMaskAccessionNumber( QString studyMaskAccessionNumber , QString studyAccessionNumber )
{
    if ( studyMaskAccessionNumber.length() > 0 )
    { //si hi ha màscara AccessioNumber

        if ( studyAccessionNumber == studyMaskAccessionNumber )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    return true;
}

QString ReadDicomdir::replaceBarra( QString original )
{
    QString ret;

    ret = original;

    while( ret.indexOf("\\") !=-1 )
        ret.replace( ret.indexOf("\\") , 1 , "/" );

    return ret;
}

ReadDicomdir::~ReadDicomdir()
{
}

}
