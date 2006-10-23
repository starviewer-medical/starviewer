/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <string>
#include "readdicomdir.h"

#include "ofstring.h"
#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcdeftag.h" //provide the information for the tags
#include "status.h"
#include "study.h"
#include "studylist.h"
#include "series.h"
#include "serieslist.h"
#include "studymask.h"
#include "image.h"
#include "imagelist.h"

namespace udg {

ReadDicomdir::ReadDicomdir()
{
    m_dicomdir = NULL;    

}

Status ReadDicomdir::open( std::string dicomdirPath )
{
    Status state;
    std::string dicomdirFilePath;

    //no existeix cap comanda per tancar un dicomdir, quan en volem obrir un de nou, l'única manera d'obrir un nou dicomdir, és a través del construtor de DcmDicomDir, passant el path per paràmetre, per això si ja existia un Dicomdir ober, fem un delete, per tancar-lo
    if ( m_dicomdir != NULL) delete m_dicomdir;

    m_dicomdirAbsolutePath = dicomdirPath;
    
    //per defecte la informació dels dicomdir es guarda en unfitxer, per obrir el dicomdir hem d'obrir aquest fitxer, que per defecte es diu DICOMDIR, per tant l'hem de concatenar amb el path del dicomdir, per poder accedir al fitxer
    dicomdirFilePath = dicomdirPath;
    dicomdirFilePath.append( "/DICOMDIR" );
    m_dicomdir = new DcmDicomDir( dicomdirFilePath.c_str() );
         
    return state.setStatus( m_dicomdir->error() );
}

//El dicomdir segueix una estructura d'abre on tenim n pacients, que tenen n estudis, que conté n series, i que conté n imatges, per llegir la informació hem d'accedir a través d'aquesta estructura d'arbre, primer llegim el primer pacient, amb el primer pacient, podem accedir el segon nivell de l'arbre, els estudis del pacient, i anar fent així fins arribar al nivell de baix de tot, les imatges,
Status ReadDicomdir::readStudies( StudyList &studyList , StudyMask studyMask )
{
    Status state;
    
    if ( m_dicomdir == NULL ) return state.setStatus( "Error: Not open dicomfile" , false , 1302 ); //FER RETORNAR STATUS AMB ERROR

    DcmDirectoryRecord *root = &( m_dicomdir->getRootRecord() );//accedim a l'estructura d'arbres del dicomdir
    DcmDirectoryRecord *patientRecord = root->getSub( 0 );//accedim al primer pacient
    OFString text;
    Study study;
    
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
Status ReadDicomdir::readSeries( std::string studyUID , SeriesList &seriesList )
{
    Status state;
    
    if ( m_dicomdir == NULL ) return state.setStatus( "Error: Not open dicomfile" , false , 1302 ); //FER

    DcmDirectoryRecord *root = &( m_dicomdir->getRootRecord() );//accedim a l'estructura d'abres del dicomdir
    DcmDirectoryRecord *patientRecord = root->getSub( 0 );//accedim al primer pacient
    DcmDirectoryRecord *studyRecord;
    OFString text;
    Series series;
    std::string studyUIDRecord , seriesPath;
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
            studyUIDRecord.insert( 0 , text.c_str() );
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
        while ( seriesRecord != NULL )
        {
            //Número de sèrie
            seriesRecord->findAndGetOFStringArray( DCM_SeriesNumber , text );
            series.setSeriesNumber( text.c_str() );

            //UID Serie
            seriesRecord->findAndGetOFStringArray( DCM_SeriesInstanceUID , text );
            series.setSeriesUID( text.c_str() );

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
            seriesPath = seriesPath.substr( 0 , seriesPath.rfind("/") + 1 );//Ignorem el nom de la primera imatge, nosaltres volem el directori de la sèrie
            series.setSeriesPath( seriesPath.c_str() );            
            
            seriesList.insert( series );//inserim a la llista de sèrie
            
            seriesRecord = studyRecord->nextSub( seriesRecord ); //accedim a la següent sèrie de l'estudi
        } 
    }   
    
    return state.setStatus( m_dicomdir->error() );
}

Status ReadDicomdir::readImages( std::string seriesUID , ImageList &imageList )
{
    Status state;
    
    if ( m_dicomdir == NULL ) return state.setStatus( "Error: Not open dicomfile" , false , 1302 ); //FER

    DcmDirectoryRecord *root = &( m_dicomdir->getRootRecord() );//accedim a l'estructura d'abres del dicomdir
    DcmDirectoryRecord *patientRecord = root->getSub( 0 );//accedim al primer pacient
    DcmDirectoryRecord *studyRecord, *seriesRecord;
    OFString text;
    Series series;
    std::string studyUIDRecord , seriesUIDRecord, imagePath;
    bool trobat = false;
    Image image;
    
    //Accedim a nivell de pacient
    while ( patientRecord != NULL && !trobat )
    {
        studyRecord = patientRecord->getSub( 0 );//indiquem que volem el primer estudi del pacient

        while (  studyRecord != NULL && !trobat )//accedim a niell estudi
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
                    image.setSeriesUID( seriesUIDRecord.c_str() );//indiquem el seriesUID
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
            imageRecord->findAndGetOFStringArray( DCM_ReferencedSOPInstanceUID , text );
            image.setSoPUID( text.c_str() );

            //Instance Number (Número d'imatge
            imageRecord->findAndGetOFStringArray( DCM_InstanceNumber , text );            
            image.setImageNumber( atoi( text.c_str() ) );       

             //Path de la imatge ens retorna el path relatiu respecte el dicomdir DirectoriEstudi/DirectoriSeries/NomImatge. Atencio retorna els directoris separats per '/', per linux s'ha de transformar a '\'
            imageRecord->findAndGetOFStringArray( DCM_ReferencedFileID , text );//obtenim el path relatiu de la imatge
            imagePath.clear();
            imagePath.insert( 0 , text.c_str() );
            imagePath = replaceBarra( imagePath );
            image.setImagePath( imagePath.c_str() );              

            imageList.insert( image );//inserim a la llista la imatge*/
            
            imageRecord = seriesRecord->nextSub( imageRecord ); //accedim a la següent imatge de la sèrie
        }         
    }   
    
    return state.setStatus( m_dicomdir->error() );    
}

std::string ReadDicomdir::getDicomdirPath()
{
    return m_dicomdirAbsolutePath;
}

//Per fer el match seguirem els criteris del PACS
bool ReadDicomdir::matchStudyMask( Study study , StudyMask studyMask )
{
    if ( !matchStudyMaskStudyId( studyMask.getStudyId() , study.getStudyId() ) ) return false;

    if ( !matchStudyMaskPatientId( studyMask.getPatientId() , study.getPatientId() ) ) return false;

    if ( !matchStudyMaskDate( studyMask.getStudyDate() , study.getStudyDate() ) ) return false;

    if ( !matchStudyMaskPatientName( studyMask.getPatientName() , study.getPatientName() ) ) return false;
    
    if ( !matchStudyMaskStudyUID( studyMask.getStudyUID() , study.getStudyUID() ) ) return false;
    
    if ( !matchStudyMaskAccessionNumber( studyMask.getAccessionNumber() , study.getAccessionNumber() ) ) return false;

    return true;
}

bool ReadDicomdir::matchStudyMaskStudyId( std::string studyMaskStudyId , std:: string studyStudyId )
{
    if ( studyMaskStudyId.length() > 0 )
    { //si hi ha màscara d'estudi Id
      //el id de l'estudi, des de la classe query screen el guardem a la màscara es amb format '*StudyID*'. Els '*' s'han de treure           
        studyMaskStudyId = upperString( studyMaskStudyId.substr( 1 , studyMaskStudyId.length() - 2 ) );
        
        if ( studyStudyId.find( studyMaskStudyId ) ==  std::string::npos )
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

bool ReadDicomdir::matchStudyMaskStudyUID( std::string studyMaskStudyUID , std:: string studyStudyUID )
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

bool ReadDicomdir::matchStudyMaskPatientId( std::string studyMaskPatientId , std:: string studyPatientId )
{
    if ( studyMaskPatientId.length() > 0 )
    { //si hi ha màscara Patient Id
      //el id del pacient, des de la classe query screen el guardem a la màscara es amb format '*PatientID*'. Els '*' s'han de treure           
        
        studyMaskPatientId = upperString( studyMaskPatientId.substr( 1 , studyMaskPatientId.length() - 2 ) );
        
        if ( studyPatientId.find( studyMaskPatientId ) ==  std::string::npos )
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

bool ReadDicomdir::matchStudyMaskDate( std::string studyMaskDate , std::string studyDate )
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
                if ( studyMaskDate.substr( 1 , 8 ) >= studyDate )
                {
                    return true;
                }
                else return false;
            }
            else if ( studyMaskDate.at( 8 ) == '-' ) // cas YYYYMMDD-
            {
                if ( studyMaskDate.substr( 0 , 8 ) <= studyDate )
                {
                    return true;
                }
                else return false;
            }
        }
        else if ( studyMaskDate.length() == 17 ) // cas YYYYMMDD-YYYYMMDD
        {
            if ( studyMaskDate.substr( 0 , 8 ) <= studyDate && 
                 studyMaskDate.substr( 9 , 8 ) >= studyDate )
            {
                return true;
            }
            else return false;
        }
        return false;
    }

    return true;
}

bool ReadDicomdir::matchStudyMaskPatientName( std::string studyMaskPatientName , std::string studyPatientName )
{
    std:: string lastPatientName , firstPatientName;

    if ( studyMaskPatientName.length() > 0 )
    { //En Pacs la màscara del nom té el següent format Cognoms*Nom*
      //Seguint els criteris del PACS la cerca es fa en wildcard, és a dir no cal que els dos string sigui igual mentre que la màscara del nom del pacient estigui continguda dins studyPatientName n'hi ha suficient
        studyMaskPatientName = upperString( studyMaskPatientName );
        lastPatientName = studyMaskPatientName.substr( 0 , studyMaskPatientName.find_first_of ( "*" ) );   

        if ( lastPatientName.length() > 0)
        {
            if ( studyPatientName.find ( lastPatientName ) == std::string::npos ) return false; //comprovem si el nom del pacient conte el cognom
        }
    
        if ( studyMaskPatientName.find_first_of( "*" ) < studyMaskPatientName.length() ) //si la màscara també contem el nom del pacient
        {
            firstPatientName = studyMaskPatientName.substr( studyMaskPatientName.find_first_of ( "*" ) + 1 , studyMaskPatientName.length() - studyMaskPatientName.find_first_of ( "*" ) -2 );  //ignorem el * de final del Nom
            
            if ( studyPatientName.find ( firstPatientName ) == std::string::npos ) return false;
        }
        
        return true;
    }

    return true;

}

bool ReadDicomdir::matchStudyMaskAccessionNumber( std::string studyMaskAccessionNumber , std:: string studyAccessionNumber )
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

std::string ReadDicomdir::upperString( std:: string original )
{
    for ( unsigned int i = 0; i < original.length(); i++ )
    {
        original[i] = toupper( original[i] );
    }

    return original;
}

std::string ReadDicomdir::replaceBarra( std::string original )
{
    std::string ret;
    
    ret = original;
    
    //string::npos es retorna quan no s'ha trobat el "\\"
     while ( ret.find( "\\" ) != std::string::npos )
     {
         ret.replace( ret.find( "\\" ) , 1 , "/" , 1 );
     }
    
    return ret;
}

ReadDicomdir::~ReadDicomdir()
{
}

}
