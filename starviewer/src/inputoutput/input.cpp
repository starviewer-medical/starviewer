/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
 
#ifndef UDGINPUT_CPP
#define UDGINPUT_CPP

#include "input.h"
#include <iostream>
#include <itkSpatialOrientation.h>
#include <itkOrientImageFilter.h>
#include <itkMetaDataDictionary.h>
#include <itkMetaDataObject.h>

namespace udg {


Input::Input()
{
    m_reader = ReaderType::New(); 
    m_seriesReader = SeriesReaderType::New(); 
    m_volumeData = new Volume;
    
    m_gdcmIO = ImageIOType::New();
    m_namesGenerator = NamesGeneratorType::New();
    
   /* typedef itk::QtSignalAdaptor SignalAdaptorType;
    SignalAdaptorType m_progressSignalAdaptor;
   */ 
     itk::QtSignalAdaptor *m_progressSignalAdaptor = new itk::QtSignalAdaptor;
//   Connect the adaptor as an observer of a Filter's event
   m_seriesReader->AddObserver( itk::ProgressEvent(),  m_progressSignalAdaptor->GetCommand() );
// 
//  Connect the adaptor's Signal to the Qt Widget Slot
   connect( m_progressSignalAdaptor, SIGNAL( Signal() ), this, SLOT( slotProgress() ) );
}

Input::~Input()
{
}
 
bool Input::openFile( const char * fileName )
{ 
    ProgressCommand::Pointer observer = ProgressCommand::New();
    m_reader->AddObserver( itk::ProgressEvent(), observer );
    
    bool ok = true;
    
    m_reader->SetFileName( fileName );
    emit progress(0);
    try
    {
        m_reader->Update();
    }
    catch ( itk::ExceptionObject & e )
    {
        std::cerr << "Exception in file reader " << std::endl;
        std::cerr << e << std::endl;
        ok = false;
        emit progress(-1); // això podria indicar excepció
    }
    if ( ok )
    {
        m_volumeData->setData( m_reader->GetOutput() );
//         adjustOrientation();
        emit progress(100);
    }
    return ok;
}

bool Input::readSeries( const char *dirPath )
{
//     SeriesProgressCommand::Pointer observer = SeriesProgressCommand::New();
//     m_seriesReader->AddObserver( itk::ProgressEvent(), observer );

    bool ok = true;
    
    m_namesGenerator->SetInputDirectory( dirPath );
    
    const SeriesReaderType::FileNamesContainer &filenames = m_namesGenerator->GetInputFileNames();
    // això és necessari per després poder demanar-li el diccionari de meta-dades i obtenir els tags del DICOM
    m_seriesReader->SetImageIO( m_gdcmIO );  
    m_seriesReader->SetFileNames( filenames );
    
    emit progress(0);
    
    try
    {
        m_seriesReader->Update();
    }
    catch ( itk::ExceptionObject & e )
    {
        std::cerr << "Exception in series file reader " << std::endl;
        std::cerr << e << std::endl;
        ok = false;
        emit progress(-1); // això podria indicar excepció
    }
    if ( ok )
    {
        m_volumeData->setData( m_seriesReader->GetOutput() );
        m_volumeData->getItkData()->SetMetaDataDictionary( m_gdcmIO->GetMetaDataDictionary() );
//         adjustOrientation();
        emit progress(100);
        
//         this->printTag( "0018|0080" , "Repetition Time" );
//         this->printTag( "0018|0081" , "Echo Time" );
//         this->printTag( "0018|0082" , "Inversion Time" );
//         this->printTag( "0018|0088" , "Spacing between slices" );
//         this->printTag( "0018|9019" , "Tag Angle First Axis" );
//         this->printTag( "0020|0020" , "Patient Orientation" );
//         this->printTag( "0018|9317" , "Reconstruction Field of View" );
//         this->printTag( "0018|1147" , "Field of View Shape" );
//         this->printTag( "0018|7030" , "Field of View Origin" );
//         this->printTag( "0018|7032" , "Field of View Rotation" );
//         this->printTag( "0018|7034" , "Field of View Horizontal Flip" );
//         this->printTag( "0018|9318" , "Reconstruction Target Center Patient" );
//         this->printTag( "0018|9319" , "Reconstruction Angle" );
//         this->printTag( "0018|9320" , "Image Filter" );
//         this->printTag( "0018|9321" , "CT Exposure Sequence" );
//         this->printTag( "0018|9322" , "Reconstruction Pixel Spacing" );
//         this->printTag( "0020|0022" , "Overlay Number" );
//         this->printTag( "0020|0026" , "LUT Number" );
//         this->printTag( "2010|0040" , "Film orientation" );

        this->printTag( "0010|0010" , "Patient's name" );
        this->printTag( "0020|0037" , "Direction Cosines" );
        this->printTag( "0010|0020" , "Patient ID" );
        this->printTag( "0010|0030" , "Patient's Birth Date" );
        this->printTag( "0010|0040" , "Patient's Sex" );
        this->printTag( "0008|0060" , "Modality" );
        this->printTag( "0008|0050" , "Accession number" );
        this->printTag( "0008|0020" , "Study Date" );
        this->printTag( "0008|0030" , "Study Time" );
        this->printTag( "0020|0010" , "Study ID" );
        this->printTag( "0008|0061" , "Modalities in study" );
        this->printTag( "0020|000D" , "Study Instance UID" );
        this->printTag( "0008|1030" , "Study Description" );
        this->printTag( "0010|1010" , "Patient's age" );
        this->printTag( "0010|1020" , "Patient's size" );
        this->printTag( "0020|0011" , "Series Number" );
        this->printTag( "0020|000E" , "Series instance UID" );
        this->printTag( "0020|0013" , "Instance Number" );
        this->printTag( "0008|0080" , "Institution Name" );
        this->printTag( "0020|0030" , "Image Position (RET)" );
        this->printTag( "0020|0032" , "Image Position Patient" );
        this->printTag( "0020|0035" , "Image Orientation (RET)" );
        this->printTag( "0020|0037" , "Image Orientation (Patient)" );
        this->printTag( "3002|0010" , "Image Orientation" );

//         this->printTag( "000|000" , "" );
//         this->printTag( "000|000" , "" );
//         this->printTag( "000|000" , "" );
//         this->printTag( "000|000" , "" );
//         this->printTag( "000|000" , "" );
//         this->printTag( "000|000" , "" );
//         this->printTag( "000|000" , "" );
//         this->printTag( "000|000" , "" );
//         this->printTag( "000|000" , "" );

    }
    
    return ok;
}

void Input::printTag( std::string tag , std::string name )
{
    typedef itk::MetaDataDictionary   DictionaryType;
//     const  DictionaryType & dictionary = m_gdcmIO->GetMetaDataDictionary();
    const  DictionaryType & dictionary = m_volumeData->getItkData()->GetMetaDataDictionary();

    typedef itk::MetaDataObject< std::string > MetaDataStringType;

    DictionaryType::ConstIterator itr = dictionary.Begin();
    DictionaryType::ConstIterator end = dictionary.End();
    
    DictionaryType::ConstIterator tagItr = dictionary.Find( tag );
    
    if( tagItr == end )
    {
        std::cerr << "Tag " << tag << " ( " << name << " ) ";
        std::cerr << " not found in the DICOM header" << std::endl;
    }
    else
    {
        MetaDataStringType::ConstPointer entryvalue = dynamic_cast<const MetaDataStringType *>( tagItr->second.GetPointer() );
    
        if( entryvalue )
        {
            std::string tagvalue = entryvalue->GetMetaDataObjectValue();
            std::cout << name << " ( " << tag <<  ") ";
            std::cout << " is: " << tagvalue << std::endl;
        }
    }
}

void Input::adjustOrientation()
{
    itk::SpatialOrientation::ValidCoordinateOrientationFlags fileOrientation;
    std::cout << "file orientation default : " << fileOrientation << std::endl;

    itk::ExposeMetaData< itk::SpatialOrientation::ValidCoordinateOrientationFlags > ( /*m_volumeData->getItkData()->GetMetaDataDictionary()*/ m_gdcmIO->GetMetaDataDictionary(), itk::ITK_CoordinateOrientation , fileOrientation );
    
    itk::OrientImageFilter< ImageType , ImageType >::Pointer orienter = itk::OrientImageFilter< ImageType , ImageType >::New();

    orienter->SetDesiredCoordinateOrientation( itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RPI );
    orienter->SetGivenCoordinateOrientation( fileOrientation );
    std::cout << "Given coordinate :: " << fileOrientation << " and desired :: " << itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RPI << std::endl;

   

   orienter->SetInput( m_volumeData->getItkData() );

   orienter->Update();

   m_volumeData->setData( orienter->GetOutput() );
}

}; // end namespace udg

#endif
