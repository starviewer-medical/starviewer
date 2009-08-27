
#include "fakeobjects.h"

#include <QList>

#include "../inputoutput/localdatabasemanager.h"
#include "../inputoutput/dicommask.h"
#include "../core/image.h"

#include "logging.h"

namespace udg{

DicomPrinter FakeObjects::getDicomPrinterFake()
{
    DicomPrinter dicomPrinter;

    dicomPrinter.setHostname("localhost");
    dicomPrinter.setPort(104);
    dicomPrinter.setAETitle("STARVIEWER");

    return dicomPrinter;
}

DicomPrintJob FakeObjects::getDicomPrintJobFake(QString studyUID, QString seriesUID, int numberOfImages)
{
    DicomPrintJob dicomPrintJob;

    dicomPrintJob.setFilmDestination("BIN_1");
    dicomPrintJob.setLabel("");
    dicomPrintJob.setMediumType("BLUE FILM");
    dicomPrintJob.setNumberOfCopies(1);
    dicomPrintJob.setPrintPriority("MED");
    dicomPrintJob.setPrintPage(getDicomPrintPageFake(studyUID, seriesUID, numberOfImages));

    return dicomPrintJob;
}

DicomPrintPage FakeObjects::getDicomPrintPageFake(QString studyUID, QString seriesUID, int numberOfImages)
{
    DicomPrintPage dicomPrintPage;
    DicomMask maskImagesToPrint;
    QList<Image*> databaseImages, imagesToPrint;
    int index = 0;

    maskImagesToPrint.setStudyUID(studyUID);
    maskImagesToPrint.setSeriesUID(seriesUID);

    databaseImages = LocalDatabaseManager().queryImage(maskImagesToPrint);

    while (index < numberOfImages &&  index <databaseImages.count())
    {
        imagesToPrint.append(databaseImages.at(index));
        index++;
    }

    dicomPrintPage.setImagesToPrint(imagesToPrint);

    dicomPrintPage.setBorderDensity("BLACK");
    dicomPrintPage.setEmptyImageDensity("WHITE");
    dicomPrintPage.setFilmLayout("STANDARD\2,2");
    dicomPrintPage.setFilmOrientation("PORTRAIT");
    dicomPrintPage.setFilmSize("8INX10IN");
    dicomPrintPage.setMagnificationType("CUBIC");
    dicomPrintPage.setMaxDensity(20);
    dicomPrintPage.setMinDensity(1);
    dicomPrintPage.setPageNumber(1);
    dicomPrintPage.setPolarity("NORMAL");
    //dicomPrintPage.setSmoothingType(""); depen de MagnificationType TODO:Testejar
    dicomPrintPage.setTrim(true);

    return dicomPrintPage;
}

}