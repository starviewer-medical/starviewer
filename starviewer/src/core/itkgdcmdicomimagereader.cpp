/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "itkgdcmdicomimagereader.h"

#include "logging.h"
#include "image.h"

#include <itkImageFileReader.h>
#include <itkGDCMImageIO.h>

namespace udg {

itkGdcmDICOMImageReader::itkGdcmDICOMImageReader(QObject *parent)
 : DICOMImageReader(parent)
{
}

itkGdcmDICOMImageReader::~itkGdcmDICOMImageReader()
{
}

bool itkGdcmDICOMImageReader::load()
{
    bool ok = readyToLoad();

    if( ok )
    {
        // preparem els paràmetres de lectura
//         typedef itk::Image<signed short, 3> ImageType;
        typedef itk::Image<int, 3> ImageType;
        typedef itk::ImageFileReader< ImageType >  ReaderType;
        typedef itk::GDCMImageIO GDCMImageIOType;

        ReaderType::Pointer reader = ReaderType::New();
        GDCMImageIOType::Pointer gdcmIO = GDCMImageIOType::New();

        // assignem al lector quin és el tipus de lector que volem
        reader->SetImageIO( gdcmIO );

        // buffer on colocarem la llesca que hem llegit
        unsigned char *dicomBuffer = NULL;
        // imatges totals i comptador per calcular el progrés
        int total = m_inputImageList.size();
        int slice = 0;
        emit started();
        // Per cada imatge
        foreach( Image *image, m_inputImageList )
        {
            reader->SetFileName( qPrintable( image->getPath() ) );
            try
            {
                reader->Update();
            }
            catch ( itk::ExceptionObject & e )
            {
                ERROR_LOG( QString("Excepció llegint l'arxiu [%1]\nDescripció: [%2]")
                        .arg( image->getPath() )
                        .arg( e.GetDescription() )
                        );
            }
            dicomBuffer = (unsigned char *)reader->GetOutput()->GetPixelContainer()->GetBufferPointer();
            // copiem les dades del buffer d'imatge cap a vtk
            memcpy( m_imageBuffer, dicomBuffer, m_sliceByteIncrement );
            m_imageBuffer += m_sliceByteIncrement;
            slice++;
            emit progress( slice / total );
        }
        emit finished();
    }

    return ok;
}

}
