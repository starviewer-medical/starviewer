/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gr�ics de Girona						      *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGSTOREDDCMTK_H
#define UDGSTOREDDCMTK_H

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

    CreateDicomPrintSpool();

	//void setVolume(Volume *p_volume);
	void createPrintSpool(DicomPrinter dicomPrinter, DicomPrintJob dicomPrintJo);	

 private:
	

    void transformImageForPrinting(Image *image);

    void configureDcmtkDVPSStoredPrint();

    void createHardcopyGrayscaleImage(Image *imageToPrint, const void *pixelData, unsigned long bitmapWidth, unsigned long bitmapHeight, double pixelAspectRatio);
	

    void setImageBoxAttributes();

    void createStoredPrintDcmtkFile();

   // Versió xapusera. Demanar com millorar-ho.
	char	m_tranformedImageToPrintUID[70];

    DicomPrintJob m_dicomPrintJob;
    DicomPrinter m_dicomPrinter;
	DVPSStoredPrint		* m_StoredPrint;
	DVPresentationState * m_PresentationState;

    QString m_spoolDirectoryPath;
};
};

#endif