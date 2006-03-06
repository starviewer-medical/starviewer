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
        emit progress(100);
    }
    return ok;
}

bool Input::readSeries( const char *dirPath )
{
/*    SeriesProgressCommand::Pointer observer = SeriesProgressCommand::New();
    m_seriesReader->AddObserver( itk::ProgressEvent(), observer );
*/
    
    bool ok = true;
    
    m_namesGenerator->SetInputDirectory( dirPath );
    
    const SeriesReaderType::FileNamesContainer &filenames =                            m_namesGenerator->GetInputFileNames();
    
//     m_seriesReader->SetImageIO( m_gdcmIO ); // això fa que exclusivament faci servir el lector de DICOMs, si no ho fèssim ell mateix s'espabilaria per trobar el lector adequat
    
    m_seriesReader->SetFileNames( filenames );
    
    //emit progress(0);
    
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
        //emit progress(100);
    }
    
    return ok;
}

}; // end namespace udg

#endif
