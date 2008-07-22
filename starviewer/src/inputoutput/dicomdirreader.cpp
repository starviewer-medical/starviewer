/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "dicomdirreader.h"

#include <dcdicdir.h> //llegeix el dicom dir
#include <ofstring.h>
#include <osconfig.h> /* make sure OS specific configuration is included first */
#include <dcdeftag.h> //provide the information for the tags
#include <QStringList>
#include <QDir>
#include <QFile>

#include "status.h"
#include "dicomstudy.h"
#include "dicomseries.h"
#include "dicommask.h"
#include "dicomimage.h"
#include "logging.h"

namespace udg {

DICOMDIRReader::DICOMDIRReader()
{
    m_dicomdir = NULL;
}

DICOMDIRReader::~DICOMDIRReader()
{
}

Status DICOMDIRReader::open(const QString &dicomdirFilePath)
{
    Status state;

    //no existeix cap comanda per tancar un dicomdir, quan en volem obrir un de nou, l'única manera d'obrir un nou dicomdir, és a través del construtor de DcmDicomDir, passant el path per paràmetre, per això si ja existia un Dicomdir ober, fem un delete, per tancar-lo
    if ( m_dicomdir != NULL) delete m_dicomdir;

    //Guardem el directori on es troba el dicomdir
    QFileInfo dicomdirFileInfo(dicomdirFilePath);
    m_dicomdirAbsolutePath =  dicomdirFileInfo.absolutePath();

    /* L'estàndard del dicom indica que l'estructura del dicomdir ha d'estar guardada en un fitxer anomeant "DICOMDIR". En linux per
       defecte en les unitats vfat, mostra els noms de fitxer que són shortname ( 8 o menys caràcters ) en minúscules, per tant
       quan el dicomdir estigui guardat en unitats vfat i el volguem obrir trobarem que el fitxer ones guarda la informació del
       dicomdir es dirà "dicomdir" en minúscules, per aquest motiu busquem el fitxer dicomdir tan en majúscules com minúscules
    */
    //busquem el nom del fitxer que conté les dades del dicomdir
    m_dicomdirFileName = dicomdirFileInfo.fileName();

    //Comprovem si el sistema de fitxers treballa amb nom en minúscules o majúscules
    if ( m_dicomdirFileName == m_dicomdirFileName.toUpper() )
    {
        m_dicomFilesInLowerCase = false;
    }//està montat en una vfat
    else
    {
        m_dicomFilesInLowerCase = true;//indiquem que els fitxers estan en minúscules
    }

    m_dicomdir = new DcmDicomDir( qPrintable( QDir::toNativeSeparators( dicomdirFilePath ) ) );

    return state.setStatus( m_dicomdir->error() );
}

//El dicomdir segueix una estructura d'abre on tenim n pacients, que tenen n estudis, que conté n series, i que conté n imatges, per llegir la informació hem d'accedir a través d'aquesta estructura d'arbre, primer llegim el primer pacient, amb el primer pacient, podem accedir el segon nivell de l'arbre, els estudis del pacient, i anar fent així fins arribar al nivell de baix de tot, les imatges,
Status DICOMDIRReader::readStudies( QList<DICOMStudy> &outResultsStudyList , DicomMask studyMask )
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
        study.setPatientName( QString::fromLatin1( text.c_str() ) );
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
            study.setStudyDescription( QString::fromLatin1( text.c_str() ) );

            //accession number
            studyRecord->findAndGetOFStringArray( DCM_AccessionNumber , text );
            study.setAccessionNumber( text.c_str() );

            //obtenim el UID de l'estudi
            studyRecord->findAndGetOFStringArray( DCM_StudyInstanceUID , text );
            study.setStudyUID( text.c_str() );

            //comprovem si l'estudi compleix la màscara de cerca que ens han passat
            if ( matchStudyMask( study , studyMask ) ) outResultsStudyList.append( study );

            studyRecord = patientRecord->nextSub( studyRecord ); //accedim al següent estudi del pacient
        }

        patientRecord = root->nextSub( patientRecord ); //accedim al següent pacient del dicomdir
    }

    return state.setStatus( m_dicomdir->error() );
}

//Per trobar les sèries d'une estudi haurem de recorre tots els estudis dels pacients, que hi hagi en el dicomdir, fins que obtinguem l'estudi amb el UID sol·licitat una vegada found, podrem accedir a la seva informacio de la sèrie
Status DICOMDIRReader::readSeries( QString studyUID , QString seriesUID , QList<DICOMSeries> &outResultsSeriesList )
{
    Status state;

    if ( m_dicomdir == NULL ) return state.setStatus( "Error: Not open dicomfile" , false , 1302 ); //FER

    DcmDirectoryRecord *root = &( m_dicomdir->getRootRecord() );//accedim a l'estructura d'abres del dicomdir
    DcmDirectoryRecord *patientRecord = root->getSub( 0 );//accedim al primer pacient
    DcmDirectoryRecord *studyRecord;
    OFString text;
    DICOMSeries series;
    QString studyUIDRecord , seriesPath;
    bool found = false;

    //Accedim a nivell de pacient
    while ( patientRecord != NULL && !found )
    {
        studyRecord = patientRecord->getSub( 0 );//indiquem que volem el primer estudi del pacient

        while (  studyRecord != NULL && !found )
        {
            text.clear();
            studyUIDRecord.clear();
            studyRecord->findAndGetOFStringArray( DCM_StudyInstanceUID , text );//obtenim el UID de l'estudi al qual estem posicionats
            studyUIDRecord = text.c_str();
            if ( studyUIDRecord == studyUID ) //busquem l'estudi que continguin el mateix UID
            {
                found = true;
            }
            else studyRecord = patientRecord->nextSub( studyRecord );//si no trobem accedim al seguent estudi del pacient
        }

        if ( !found ) patientRecord = root->nextSub( patientRecord ); //accedim al següent pacient
    }

    if ( found )//si hem found l'estudi amb el UID que cercàvem
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

                //Protocol Name
                seriesRecord->findAndGetOFStringArray( DCM_ProtocolName , text );
                series.setProtocolName( QString::fromLatin1( text.c_str() ) );

                //Per obtenir el directori de les series, no hi ha cap més manera que accedir. a la primera imatge de la serie i consultar-ne el directori

                DcmDirectoryRecord *imageRecord = seriesRecord->getSub( 0 );

                //Path de la imatge ens retorna el path absolut respecte el dicomdir DirectoriEstudi/DirectoriSeries/NomImatge. Atencio retorna els directoris separats per '/', per linux s'ha de transformar a '\'
                imageRecord->findAndGetOFStringArray( DCM_ReferencedFileID , text );
                seriesPath.clear();
                seriesPath.insert( 0 , text.c_str() );//Afegim la ruta de la primera imatge dins el dicomdir
                seriesPath = backSlashToSlash( seriesPath );
                seriesPath = seriesPath.mid( 0 , seriesPath.toStdString().rfind("/") + 1 );//Ignorem el nom de la primera imatge, nosaltres volem el directori de la sèrie
                series.setSeriesPath( seriesPath );

                outResultsSeriesList.append(series);//inserim a la llista de sèrie
            }
            seriesRecord = studyRecord->nextSub( seriesRecord ); //accedim a la següent sèrie de l'estudi
        }
    }

    return state.setStatus( m_dicomdir->error() );
}

Status DICOMDIRReader::readImages( QString seriesUID , QString sopInstanceUID , QList<DICOMImage> &outResultsImageList )
{
    Status state;

    if ( m_dicomdir == NULL ) return state.setStatus( "Error: Not open dicomfile" , false , 1302 ); //FER

    DcmDirectoryRecord *root = &( m_dicomdir->getRootRecord() );//accedim a l'estructura d'abres del dicomdir
    DcmDirectoryRecord *patientRecord = root->getSub( 0 );//accedim al primer pacient
    DcmDirectoryRecord *studyRecord, *seriesRecord;
    OFString text;
    DICOMSeries series;
    QString studyUIDRecord , seriesUIDRecord, imagePath;
    bool found = false;
    DICOMImage image;

    //Accedim a nivell de pacient
    while ( patientRecord != NULL && !found )
    {
        studyRecord = patientRecord->getSub( 0 );//indiquem que volem el primer estudi del pacient

        while (  studyRecord != NULL && !found )//accedim a nivell estudi
        {
            seriesRecord = studyRecord->getSub( 0 ); //seleccionem la serie de l'estudi que conté el studyUID que cercàvem
            while ( seriesRecord != NULL && !found )//accedim a nivell
            {
                //UID Serie
                text.clear();
                seriesUIDRecord.clear();
                seriesRecord->findAndGetOFStringArray( DCM_SeriesInstanceUID , text );
                seriesUIDRecord.insert( 0 , text.c_str() );
                if ( seriesUIDRecord == seriesUID ) //busquem la sèrie amb les imatges
                {
                    found = true;
                    image.setSeriesUID( seriesUIDRecord );//indiquem el seriesUID
                    studyRecord->findAndGetOFStringArray( DCM_StudyInstanceUID , text );
                    image.setStudyUID( text.c_str() );//Indiquem el studyUID de la imatge
                }
                else seriesRecord = studyRecord->nextSub( seriesRecord ); //accedim a la següent sèrie de l'estudi
            }
            studyRecord = patientRecord->nextSub( studyRecord );//si no trobem accedim al seguent estudi del pacient
        }

        if ( !found ) patientRecord = root->nextSub( patientRecord ); //accedim al següent pacient
    }

    if ( found )//si hem found la sèrie amb el UID que cercàvem
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
                imagePath =  m_dicomdirAbsolutePath + "/" + buildImageRelativePath( text.c_str() );

                image.setImagePath( imagePath );

                outResultsImageList.append( image );//inserim a la llista la imatge*/
            }

            imageRecord = seriesRecord->nextSub( imageRecord ); //accedim a la següent imatge de la sèrie
        }
    }

    return state.setStatus( m_dicomdir->error() );
}

QString DICOMDIRReader::getDicomdirFilePath()
{
    return m_dicomdirAbsolutePath + "/" + m_dicomdirFileName;
}

//TODO s'haurai de mirar si es pot fer servir les funcions de readimage, readseries i readstudy, perquè aquest mètode, són els tres anteriors mètodes en un
QStringList DICOMDIRReader::getFiles( QString studyUID )
{
    QStringList files;
    Status state;

    if ( m_dicomdir == NULL )
    {
        DEBUG_LOG("Error: Not open dicomfile");
        return files;
    }

    DcmDirectoryRecord *root = &( m_dicomdir->getRootRecord() );//accedim a l'estructura d'abres del dicomdir
    DcmDirectoryRecord *patientRecord = root->getSub( 0 );//accedim al primer pacient
    DcmDirectoryRecord *studyRecord, *seriesRecord, *imageRecord;

    QString studyUIDRecord , seriesUIDRecord, imagePath;
    bool found = false;

    // trobem primer l'estudi que volem
    while ( patientRecord != NULL && !found )
    {
        studyRecord = patientRecord->getSub( 0 );//indiquem que volem el primer estudi del pacient
        while ( studyRecord != NULL && !found )//accedim a nivell estudi
        {
            OFString text;
            studyUIDRecord.clear();
            studyRecord->findAndGetOFStringArray( DCM_StudyInstanceUID , text );
            studyUIDRecord = text.c_str();
            if( studyUIDRecord == studyUID ) //és l'estudi que volem?
            {
                found = true;
            }
            else
                studyRecord = patientRecord->nextSub( studyRecord );//si no trobem accedim al seguent estudi del pacient
        }
        if( !found )
            patientRecord = root->nextSub( patientRecord ); //accedim al següent pacient
    }

    if( found ) // si hem trobat l'uid que es demanava podem continuar amb la cerca dels arxius
    {
        found = false;
        seriesRecord = studyRecord->getSub( 0 ); //seleccionem la serie de l'estudi que conté el studyUID que cercàvem
        while ( seriesRecord != NULL && !found )//llegim totes les seves sèries
        {
            imageRecord = seriesRecord->getSub( 0 ); //seleccionem cada imatge de la series
            while ( imageRecord != NULL )
            {
                OFString text;
                //Path de la imatge ens retorna el path relatiu respecte el dicomdir DirectoriEstudi/DirectoriSeries/NomImatge. Atencio retorna els directoris separats per '\' (format windows)
                imageRecord->findAndGetOFStringArray( DCM_ReferencedFileID , text );//obtenim el path relatiu de la imatge

                files << m_dicomdirAbsolutePath + "/" + buildImageRelativePath( text.c_str() );
                imageRecord = seriesRecord->nextSub( imageRecord ); //accedim a la següent imatge de la sèrie
            }
            seriesRecord = studyRecord->nextSub( seriesRecord ); //accedim a la següent sèrie de l'estudi
        }
    }
    else
    {
        DEBUG_LOG("No s'ha trobat cap estudi amb aquest uid: " + studyUID + " al dicomdir");
    }
    return files;
}

//Per fer el match seguirem els criteris del PACS
bool DICOMDIRReader::matchStudyMask( DICOMStudy study , DicomMask studyMask )
{
    if ( !matchStudyMaskStudyId( studyMask.getStudyId() , study.getStudyId() ) ) return false;

    if ( !matchStudyMaskPatientId( studyMask.getPatientId() , study.getPatientId() ) ) return false;

    if ( !matchStudyMaskDate( studyMask.getStudyDate() , study.getStudyDate() ) ) return false;

    if ( !matchStudyMaskPatientName( studyMask.getPatientName() , study.getPatientName() ) ) return false;

    if ( !matchStudyMaskStudyUID( studyMask.getStudyUID() , study.getStudyUID() ) ) return false;

    if ( !matchStudyMaskAccessionNumber( studyMask.getAccessionNumber() , study.getAccessionNumber() ) ) return false;

    return true;
}

bool DICOMDIRReader::matchStudyMaskStudyId( QString studyMaskStudyId , QString studyStudyId )
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

bool DICOMDIRReader::matchStudyMaskStudyUID( QString studyMaskStudyUID , QString studyStudyUID )
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

bool DICOMDIRReader::matchStudyMaskPatientId( QString studyMaskPatientId , QString studyPatientId )
{
    //Si la màscara és buida rebem  * , si té valor és *ID_PACIENT*
    if ( studyMaskPatientId.length() > 1 )
    { //si hi ha màscara Patient Id

        studyMaskPatientId = studyMaskPatientId.replace( "*", "" );//treiem els "*"

        if ( !studyPatientId.contains( studyMaskPatientId , Qt::CaseInsensitive  ) ) return false;
    }

    return true;
}

bool DICOMDIRReader::matchStudyMaskDate( QString studyMaskDate , QString studyDate )
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

bool DICOMDIRReader::matchStudyMaskPatientName( QString studyMaskPatientName , QString studyPatientName )
{
    //Si la màscara és buida rebem  * , si té valor és *NOM_A_CERCAR*
    if ( studyMaskPatientName.length() > 1 )
    {//si hi ha màscara Patient Name
        studyMaskPatientName = studyMaskPatientName.replace( "*" , "" ); //treiem els "*"

        if ( !studyPatientName.contains( studyMaskPatientName , Qt::CaseInsensitive ) ) return false;
    }

    return true;
}

bool DICOMDIRReader::matchStudyMaskAccessionNumber( QString studyMaskAccessionNumber , QString studyAccessionNumber )
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

QString DICOMDIRReader::backSlashToSlash( QString original )
{
    QString ret;

    ret = original;

    while( ret.indexOf("\\") !=-1 )
        ret.replace( ret.indexOf("\\") , 1 , "/" );

    return ret;
}

QString DICOMDIRReader::buildImageRelativePath( QString imageRelativePath )
{
    /* Linux per defecte en les unitats vfat, mostra els noms de fitxer que són shortname ( 8 o menys caràcters ) en
        minúscules com que en el fitxer de dicomdir les rutes del fitxer es guarden en majúscules, m_dicomFilesInLowerCase
        és true si s'ha troba tel fitxer dicomdir en minúscules, si és consistent el dicomdir els noms de les imatges i
        rutes també serà en minúscules
    */

    if (m_dicomFilesInLowerCase)
    {
        return backSlashToSlash( imageRelativePath ).toLower();
    }
    else return backSlashToSlash( imageRelativePath );
}

}
