/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gr�ics de Girona						      *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGCREATEDICOMPRINTSPOOL_H
#define UDGCREATEDICOMPRINTSPOOL_H

#include "dicomprintjob.h"
#include "dicomprinter.h"

class DVPSStoredPrint;
class DVPresentationState;

namespace udg
{ 
class Image;

class CreateDicomPrintSpool
{
 public:

	//void setVolume(Volume *p_volume);
	QString createPrintSpool(DicomPrinter dicomPrinter, DicomPrintJob dicomPrintJob, const QString &spoolDirectoryPath);	

 private:
	
    void transformImageForPrinting(Image *image, const QString &spoolDirectoryPath);

    void setBasicFilmBoxAttributes();

    void createHardcopyGrayscaleImage(Image *imageToPrint, const void *pixelData, unsigned long bitmapWidth, unsigned long bitmapHeight, double pixelAspectRatio, const QString &spoolDirectoryPath);

    void setImageBoxAttributes();

    void createStoredPrintDcmtkFile(const QString &pathStoredPrintDcmtkFile);

    DicomPrintJob m_dicomPrintJob;
    DicomPrinter m_dicomPrinter;
	DVPSStoredPrint		* m_storedPrint;
	DVPresentationState * m_presentationState;
};
};

#endif