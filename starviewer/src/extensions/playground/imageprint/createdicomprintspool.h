/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gr�ics de Girona						      *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGCREATEDICOMPRINTSPOOL_H
#define UDGCREATEDICOMPRINTSPOOL_H

#include "dicomprinter.h"
#include "dicomprintpage.h"

class DVPSStoredPrint;
class DVPresentationState;

namespace udg
{ 
class Image;

class CreateDicomPrintSpool
{
 public:

	//void setVolume(Volume *p_volume);
	QString createPrintSpool(DicomPrinter dicomPrinter, DicomPrintPage dicomPrintPage, const QString &spoolDirectoryPath);	

 private:
	
    void transformImageForPrinting(Image *image, const QString &spoolDirectoryPath);

    void setBasicFilmBoxAttributes();

    void createHardcopyGrayscaleImage(Image *imageToPrint, const void *pixelData, unsigned long bitmapWidth, unsigned long bitmapHeight, double pixelAspectRatio, const QString &spoolDirectoryPath);

    void setImageBoxAttributes();

    /**Crea el fitxer de Dcmtk on es guarden els paràmetres amb els quals s'ha de fer la impressió i la estructura de FilmBox, que conté un FilmSession
       que conté n imatges, i els seus paràmetres d'impressió.
       Retorna el pathfile del fitxer creat*/
    QString createStoredPrintDcmtkFile(const QString &spoolDirectoryPath);

    DicomPrintPage m_dicomPrintPage;
    DicomPrinter m_dicomPrinter;
	DVPSStoredPrint		* m_storedPrint;
	DVPresentationState * m_presentationState;
};
};

#endif