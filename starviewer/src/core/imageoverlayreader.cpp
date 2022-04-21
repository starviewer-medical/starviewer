/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "imageoverlayreader.h"

#include "dicomtagreader.h"
//#include "dicomvaluerepresentationconverter.h"
#include "imageoverlay.h"
#include "logging.h"

#include <dcdatset.h>   // DcmDataset
#include <dcmimage.h>   // DicomImage

namespace udg {

ImageOverlayReader::ImageOverlayReader()
    : m_numberOfOverlays(0), m_frameNumber(0)
{
}

void ImageOverlayReader::setFilename(const QString &filename)
{
    m_filename = filename;
}

void ImageOverlayReader::setNumberOfOverlays(int numberOfOverlays)
{
    m_numberOfOverlays = numberOfOverlays;
}

void ImageOverlayReader::setFrameNumber(int frameNumber)
{
    m_frameNumber = frameNumber;
}

bool ImageOverlayReader::read()
{
    m_overlaysList.clear();

    DICOMTagReader dicomReader(m_filename);
    DicomImage dicomImage(dicomReader.getDcmDataset(), dicomReader.getDcmDataset()->getOriginalXfer(), CIF_UsePartialAccessToPixelData, 0, 1);

    for (int plane = 0; plane < m_numberOfOverlays; plane++)
    {
        int frameToRead = 0;

        DICOMTag numberOfFramesInOverlayTag(DICOMNumberOfFramesInOverlay);
        numberOfFramesInOverlayTag.setGroup(dicomImage.getOverlayGroupNumber(plane));

        if (dicomReader.hasAttribute(numberOfFramesInOverlayTag))
        {
            // Multi-frame Overlay Module (PS3.3§C.9.3)
            int numberOfFramesInOverlay = dicomReader.getValueAttributeAsQString(numberOfFramesInOverlayTag).toInt();

            DICOMTag imageFrameOriginTag(DICOMImageFrameOrigin);
            imageFrameOriginTag.setGroup(dicomImage.getOverlayGroupNumber(plane));
            int imageFrameOrigin = 0;

            if (dicomReader.hasAttribute(imageFrameOriginTag))
            {
                imageFrameOrigin = dicomReader.getValueAttributeAsQString(imageFrameOriginTag).toInt() - 1; // in DICOM first frame is 1; for us is 0
            }

            frameToRead = m_frameNumber - imageFrameOrigin;

            if (frameToRead < 0 || frameToRead >= numberOfFramesInOverlay)
            {
                continue;   // no overlay for this frame and plane
            }
        }

        // TODO Use the below code after upgrading to DCMTK 3.6.6 or later
        //      The below code gets the full uncropped overlay but crashes causes a segmentation fault in DCMTK 3.6.5 if overlay origin is not (0,0).
//        DICOMTag overlayOriginTag(DICOMOverlayOrigin);
//        overlayOriginTag.setGroup(dicomImage.getOverlayGroupNumber(plane));
//        QString overlayOrigin = dicomReader.getValueAttributeAsQString(overlayOriginTag);
//        const QVector<int64_t> &origin = DICOMValueRepresentationConverter::signed64BitVeryLongToInt64Vector(overlayOrigin);
//        int x = origin[1];  // column
//        int y = origin[0];  // row

//        uint width, height;
//        const void *data = dicomImage.getFullOverlayData(plane, width, height, frameToRead);

        uint x, y, width, height;
        EM_Overlay mode;
        const void *data = dicomImage.getOverlayData(plane, x, y, width, height, mode, frameToRead);

        if (!data)
        {
            WARN_LOG(QString("Could not fetch overlay data for plane %1 and frame %2.").arg(plane).arg(m_frameNumber));
            continue;
        }

        ImageOverlay overlay;
        overlay.setRows(height);
        overlay.setColumns(width);
        overlay.setOrigin(x, y);
        unsigned char *dataCopy = new unsigned char[width * height];
        std::memcpy(dataCopy, data, width * height);
        overlay.setData(dataCopy);

        m_overlaysList.append(overlay);
    }

    return !m_overlaysList.isEmpty();
}

QList<ImageOverlay> ImageOverlayReader::getOverlays() const
{
    return m_overlaysList;
}

}
