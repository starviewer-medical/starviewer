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

// fem servir dcmtk per l'escalat de les imatges dicom
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/ofstd/ofbmanip.h"

namespace udg
{

    ///Crea un thumbnail a partir de les imatges de la sèrie
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
            thumbnail = createImageThumbnail(series->getImages()[numberOfImages / 2 ]->getPath());
        }
        else
        {
            // si la sèrie no conté imatges en el thumbnail ho indicarem
            thumbnail = QImage(resolution, resolution, QImage::Format_RGB32);
            thumbnail.fill(Qt::black);

            QPainter painter(&thumbnail);
            painter.setPen(Qt::white);
            painter.drawText(0, 0, 100, 100, Qt::AlignCenter | Qt::TextWordWrap, QObject::tr("No Images Available"));
        }
    }

    return thumbnail;
}

QImage ThumbnailCreator::getThumbnail(const Image *image, int resolution)
{
    return createImageThumbnail(image->getPath(), resolution);
}

QImage ThumbnailCreator::createImageThumbnail(QString imageFileName, int resolution)
{
    QImage thumbnail;
    bool ok = false;

    //carreguem el fitxer dicom a escalar
    DicomImage *dicomImage = new DicomImage(qPrintable(imageFileName));

    if(dicomImage == NULL)
    {
        ok = false;
        DEBUG_LOG("Memòria insuficient per carregar l'imatge DICOM al fer el thumbnail");
    }
    else if(dicomImage->getStatus() == EIS_Normal)
    {
        dicomImage->hideAllOverlays();
        dicomImage->setMinMaxWindow(1);
        //escalem l'imatge
        DicomImage *scaledImage;
        //Escalem pel cantó més gran
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
            // el següent codi crea una imatge pgm a memòria i carreguem aquest buffer directament al pixmap
            // obtingut de http://forum.dcmtk.org/viewtopic.php?t=120&highlight=qpixmap
            // get image extension
            const int width = (int)(scaledImage->getWidth());
            const int height = (int)(scaledImage->getHeight());
            char header[32];
            // create PGM header
            sprintf(header, "P5\n%i %i\n255\n", width, height);
            const int offset = strlen(header);
            const unsigned int length = width * height + offset;
            // create output buffer for DicomImage class
            Uint8 *buffer = new Uint8[length];
            if (buffer != NULL)
            {
                // copy PGM header to buffer
                OFBitmanipTemplate<Uint8>::copyMem((const Uint8 *)header, buffer, offset);
                if (scaledImage->getOutputData((void *)(buffer + offset), length, 8))
                {
                    if(thumbnail.loadFromData((const unsigned char *)buffer, length, "PGM"))
                    {
                        ok = true;
                    }
                    else
                        DEBUG_LOG(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Ha fallat :(");

                }
                // delete temporary pixel buffer
                delete[] buffer;
            }
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

    if(!ok) // no hem pogut generar el thumbnail, creem un de buit
    {
        thumbnail = QImage(resolution, resolution, QImage::Format_RGB32);
        thumbnail.fill(Qt::black);

        QPainter painter( &thumbnail);
        painter.setPen(Qt::white);
        painter.drawText(0, 0, resolution, resolution, Qt::AlignCenter | Qt::TextWordWrap, QObject::tr("Preview image not available"));
    }

    return thumbnail;
}

};
