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


namespace udg {

ReadDicomdir::ReadDicomdir()
{
    dicomdir = NULL;    

}

Status ReadDicomdir::open( std::string dicomdirPath )
{
    Status state;
    
    //no existeix cap comanda per tancar un dicomdir, quan en volem obrir un de nou, l'única manera d'obrir un nou dicomdir, és a través del construtor de DcmDicomDir, passant el path per paràmetre, per això si ja existia un Dicomdir ober, fem un delete, per tancar-lo
    if ( dicomdir != NULL) delete dicomdir;

    dicomdir = new DcmDicomDir( dicomdirPath.c_str() );

    return state.setStatus( dicomdir->error() );
}

//El dicomdir segueix una estructura d'abre on tenim n pacients, que tene n estudis, que conté n series, i que conté n imatges, per llegir la informació hem d'accedir a través d'aquesta estructura d'arbre, primer llegim el primer pacient, amb el primer pacient, podem accedir el segon nivell de l'arbre els estudis del pacient, i anar fent així fins arribar al nivell de baix de tot les imatges,
Status ReadDicomdir::readStudies( StudyList &studyList )
{
    Status state;
    
    DcmDirectoryRecord *root = &( dicomdir->getRootRecord() );//accedim a l'estructura d'arbres del dicomdir
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

            studyList.insert( study );        
            
            studyRecord = patientRecord->nextSub( studyRecord ); //accedim al següent estudi del pacient
        }

        patientRecord = root->nextSub( patientRecord ); //accedim al següent pacient del dicomdir 
    }
    
    return state.setStatus( dicomdir->error() );
} 

//Per trobar les sèries d'une estudi haurem de recorre tots els estudis dels pacients, que hi hagi en el dicomdir, fins que obtinguem l'estudi amb el UID sol·licitat una vegada trobat, podrem accedir a la seva informacio de la sèrie
Status ReadDicomdir::readSeries( std::string studyUID , SeriesList &seriesList )
{
    Status state;
    DcmDirectoryRecord *root = &( dicomdir->getRootRecord() );//accedim a l'estructura d'abres del dicomdir
    DcmDirectoryRecord *patientRecord = root->getSub( 0 );//accedim al primer pacient
    DcmDirectoryRecord *studyRecord;
    OFString text;
    Series series;
    std::string studyUIDRecord;
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
            
            seriesList.insert( series );//inserim a la llista de sèrie
            
            seriesRecord = studyRecord->nextSub( seriesRecord ); //accedim al següent estudi de la sèrie
        } 
    }   
    
    return state.setStatus( dicomdir->error() );
}

ReadDicomdir::~ReadDicomdir()
{
}

}
