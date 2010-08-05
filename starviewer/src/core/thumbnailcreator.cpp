/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "thumbnailcreator.h"

#include <QObject>
#include <QImage>
#include <QString>
#include <QPainter>

#include "series.h"
#include "image.h"
#include "logging.h"
#include "dicomtagreader.h"
// fem servir dcmtk per l'escalat de les imatges dicom
#include <dcmimage.h>
#include <ofbmanip.h>
#include <dcdatset.h>
// Necessari per suportar imatges de color
#include <diregist.h>

namespace udg {

const QString PreviewNotAvailableText( QObject::tr("Preview image not available") );

QImage ThumbnailCreator::getThumbnail(const Series *series, int resolution)
{
    QImage thumbnail;

    if(series->getModality() == "KO")
    {
        thumbnail.load(":/images/kinThumbnail.png");
    }
    else if(series->getModality() == "PR")
    {
        thumbnail.load(":/images/presentationStateThumbnail.png");
    }
    else if(series->getModality() == "SR")
    {
        thumbnail.load(":/images/structuredReportThumbnail.png");
    }
    else
    {
        int numberOfImages = series->getImages().size();

        if (numberOfImages > 0)
        {
            thumbnail = createImageThumbnail(series->getImages()[numberOfImages / 2 ]->getPath(), resolution);
        }
        else
        {
            // si la sèrie no conté imatges en el thumbnail ho indicarem
            thumbnail = makeEmptyThumbnailWithCustomText(QObject::tr("No Images Available"));
        }
    }

    return thumbnail;
}

QImage ThumbnailCreator::getThumbnail(const Image *image, int resolution)
{
    return createImageThumbnail(image->getPath(), resolution);
}

QImage ThumbnailCreator::getThumbnail(DICOMTagReader *reader, int resolution)
{
    return createThumbnail(reader,resolution);
}

QImage ThumbnailCreator::makeEmptyThumbnailWithCustomText(const QString &text, int resolution)
{
	QImage thumbnail;
	
	thumbnail = QImage(resolution, resolution, QImage::Format_RGB32);
    thumbnail.fill(Qt::black);

    QPainter painter(&thumbnail);
    painter.setPen(Qt::white);
    painter.drawText(0, 0, resolution, resolution, Qt::AlignCenter | Qt::TextWordWrap, text);

    return thumbnail;
}

QImage ThumbnailCreator::createImageThumbnail(QString imageFileName, int resolution)
{
    DICOMTagReader reader( imageFileName );
    return createThumbnail(&reader,resolution);
}

QImage ThumbnailCreator::createThumbnail(DICOMTagReader *reader, int resolution)
{
    QImage thumbnail;
    
    if( isSuitableForThumbnailCreation(reader) )
    {
        // Carreguem el fitxer dicom a escalar
        DicomImage *dicomImage = new DicomImage(reader->getDcmDataset(), reader->getDcmDataset()->getOriginalXfer() );
        thumbnail = createThumbnail(dicomImage,resolution);
        
        // Cal esborrar la DicomImage per no tenir fugues de memòria
        if ( dicomImage )
            delete dicomImage;
    }
    else
    {
        // Creem thumbnail alternatiu indicant que no es pot mostrar una imatge de preview
        thumbnail = makeEmptyThumbnailWithCustomText( PreviewNotAvailableText );
    }

    return thumbnail;
}

QImage ThumbnailCreator::createThumbnail(DicomImage *dicomImage, int resolution)
{
    QImage thumbnail;
    bool ok = false;

    if(dicomImage == NULL)
    {
        ok = false;
        DEBUG_LOG("Memòria insuficient per carregar l'imatge DICOM al fer el thumbnail o punter nul");
    }
    else if(dicomImage->getStatus() == EIS_Normal)
    {
        dicomImage->hideAllOverlays();
        dicomImage->setMinMaxWindow(1);
        // Escalem la imatge
        DicomImage *scaledImage;
        // Escalem pel cantó més gran
        unsigned long width, height;
        if(dicomImage->getWidth() < dicomImage->getHeight())
        {
            width = 0;
            height = resolution;
        }
        else
        {
            width = resolution;
            height = 0;
        }
        scaledImage = dicomImage->createScaledImage(width,height, 1, 1);
        if( scaledImage == NULL)
        {
            ok = false;
            DEBUG_LOG("La imatge escalada s'ha retornat com a nul");
        }
        else if(scaledImage->getStatus() == EIS_Normal)
        {
            if (scaledImage->isMonochrome())
            {
                thumbnail = createPGMImage(scaledImage);
                if (thumbnail == QImage())
                {
                    ok = false;
                }
                else
                {
                    ok = true;
                }
            }
            else
            {
                thumbnail = createPPMImage(scaledImage);
                if (thumbnail == QImage())
                {
                    ok = false;
                }
                else
                {
                    ok = true;
                }
            }
            // Cal esborrar la DicomImage per no tenir fugues de memòria
            delete scaledImage;
        }
        else
        {
            ok = false;
            DEBUG_LOG(QString( "La imatge escalada té errors. Error: %1 ").arg( DicomImage::getString( scaledImage->getStatus())));
        }
    }
    else
    {
        ok = false;
        DEBUG_LOG(QString("Error en carregar la DicomImage. Error: %1 ").arg(DicomImage::getString( dicomImage->getStatus())));
    }

    // Si no hem pogut generar el thumbnail, creem un de buit
    if(!ok)
    {
        thumbnail = makeEmptyThumbnailWithCustomText( PreviewNotAvailableText );
    }

    return thumbnail;
}

bool ThumbnailCreator::isSuitableForThumbnailCreation(DICOMTagReader *reader) const
{
    if( !reader )
    {
        DEBUG_LOG("El DICOMTagReader donat és NUL!");
        return false;
    }

    if( !reader->getDcmDataset() )
    {
        DEBUG_LOG("El DICOMTagReader no té cap DcmDataset assignat, no podem generar el thumbnail.");
        return false;
    }
    
    bool suitable = true;

    // Ens hem trobat que per algunes imatges que contenen Overlays, la DICOMImage no es pot crear. 
    // Els casos que hem trobat estan descrits al ticket #1121
    // La solució adoptada ara mateix és que si trobem que la imatge conté algun dels tags següents, 
    // descartem la creació del thumbnail i en creem un de "neutre" indicant que no s'ha pogut crear aquest
    // En quant siguem capaços de tornar a llegir aquestes imatges sense problema, aquesta comprovació desapareixerà
    QList<DICOMTag> tags;
    tags << DICOMOverlayRows << DICOMOverlayColumns << DICOMOverlayType << DICOMOverlayOrigin << DICOMOverlayBitsAllocated << DICOMOverlayBitPosition <<  DICOMOverlayData;
    foreach( DICOMTag tag, tags )
    {
        if( reader->tagExists(tag) )
        {
            suitable = false;
            DEBUG_LOG( QString("Found Tag: %1,%2. Overlay restriction applied. Preview image won't be available.").arg(tag.getGroup(),0,16).arg(tag.getElement(),0,16) );
        }
    }

    // TODO Comprovar la modalitat també?
    return suitable;
}

QImage ThumbnailCreator::createPGMImage(DicomImage *dicomImage)
{
    Q_ASSERT(dicomImage);
    Q_ASSERT(dicomImage->isMonochrome());
    
    // El següent codi crea una imatge pgm a memòria i carreguem aquest buffer directament al pixmap
    // Obtingut de http://forum.dcmtk.org/viewtopic.php?t=120&highlight=qpixmap
    const int width = (int)(dicomImage->getWidth());
    const int height = (int)(dicomImage->getHeight());
    char header[32];
    // Create PGM header
    sprintf(header, "P5\n%i %i\n255\n", width, height);
    const int offset = strlen(header);
    
    // QImage en la que carregarem el buffer de dades
    QImage thumbnail;
    // Create output buffer for DicomImage class
    const unsigned int length = width * height + offset;
    Uint8 *buffer = new Uint8[length];
    if (buffer != NULL)
    {
        // Copy PGM header to buffer
        OFBitmanipTemplate<Uint8>::copyMem((const Uint8 *)header, buffer, offset);
        if (dicomImage->getOutputData((void *)(buffer + offset), length, 8))
        {
            if (!thumbnail.loadFromData((const unsigned char *)buffer, length, "PGM"))
            {
                DEBUG_LOG("La càrrega del buffer al thumbnail ha fallat :(");
            }
        }
        // Delete temporary pixel buffer
        delete[] buffer;
    }
    else
    {
        DEBUG_LOG("Memòria insuficient per crear el buffer del thumbnail!");
    }

    return thumbnail;
}

QImage ThumbnailCreator::createPPMImage(DicomImage *dicomImage)
{
    Q_ASSERT(dicomImage);
    Q_ASSERT(!dicomImage->isMonochrome());
    
    // El següent codi crea una imatge PPM a memòria i carreguem aquest buffer directament al pixmap
    // Obtingut de http://forum.dcmtk.org/viewtopic.php?t=120&highlight=qpixmap
    const int width = (int)(dicomImage->getWidth());
    const int height = (int)(dicomImage->getHeight());
    char header[32];
    // Create PPM header
    sprintf(header, "P6\n%i %i\n255\n", width, height);
    const int offset = strlen(header);
    
    // QImage en la que carregarem el buffer de dades
    QImage thumbnail;
    // Create output buffer for DicomImage class
    const unsigned int length = (width * height) * 3 + offset;
    Uint8 *buffer = new Uint8[length];
    if (buffer != NULL)
    {
        // Copy PPM header to buffer
        OFBitmanipTemplate<Uint8>::copyMem((const Uint8 *)header, buffer, offset);
        if (dicomImage->getOutputData((void *)(buffer + offset), length, 8))
        {
            if (!thumbnail.loadFromData((const unsigned char *)buffer, length, "PPM"))
            {
                DEBUG_LOG("La càrrega del buffer al thumbnail ha fallat :(");
            }
        }
        // Delete temporary pixel buffer
        delete[] buffer;
    }
    else
    {
        DEBUG_LOG("Memòria insuficient per crear el buffer del thumbnail!");
    }

    return thumbnail;
}

};
