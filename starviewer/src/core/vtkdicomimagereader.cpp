/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "vtkdicomimagereader.h"

#include "image.h"
#include <QApplication> // pel "processEvents()"

#include <vtkDICOMImageReader.h>
#include <vtkImageData.h>

namespace udg {

vtkDICOMImageReader::vtkDICOMImageReader(QObject *parent)
 : DICOMImageReader(parent)
{
}

vtkDICOMImageReader::~vtkDICOMImageReader()
{
}

bool vtkDICOMImageReader::load()
{
    bool ok = readyToLoad();

    if( ok )
    {
        // buffer on colocarem la llesca que hem llegit
        unsigned char *dicomBuffer = NULL;
        // imatges totals i comptador per calcular el progrés
        int total = m_inputImageList.size();
        int slice = 0;
        emit started();
        // Per cada imatge
        foreach( Image *image, m_inputImageList )
        {
            ::vtkDICOMImageReader *reader = ::vtkDICOMImageReader::New();
            reader->SetFileName( qPrintable( image->getPath() ) );
            reader->Update();
            dicomBuffer = (unsigned char *)reader->GetOutput()->GetScalarPointer();
            // copiem les dades del buffer d'imatge cap a vtk
            memcpy( m_imageBuffer, dicomBuffer, m_sliceByteIncrement );
            m_imageBuffer += m_sliceByteIncrement;
            slice++;
            qApp->processEvents();
            emit progress( (int)floor( (double)slice/(double)total * 100 ) );
        }
        emit finished();
    }

    return ok;
}

}
