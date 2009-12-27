
#include "fakeobjects.h"

#include <QList>

#include "../inputoutput/localdatabasemanager.h"
#include "../inputoutput/dicommask.h"
#include "../core/image.h"

#include "logging.h"

namespace udg{

DicomPrinter FakeObjects::getFakeDicomPrinter()
{
    DicomPrinter dicomPrinter;

    dicomPrinter.setHostname("localhost");
    dicomPrinter.setPort(104);//10006
    dicomPrinter.setAETitle("DVTK_TEST");//PRINTSCP

    return dicomPrinter;
}

DicomPrinter FakeObjects::getFakeDicomScopeDicomPrinter()
{
    DicomPrinter dicomPrinter;

    dicomPrinter.setHostname("localhost");
    dicomPrinter.setPort(10006);
    dicomPrinter.setAETitle("PRINTSCP");

    return dicomPrinter;
}

DicomPrintJob FakeObjects::getFakeDicomPrintJob(QString studyUID, QString seriesUID, int numberOfImages)
{
    DicomPrintJob dicomPrintJob;

    dicomPrintJob.setFilmDestination("BIN_1");
    dicomPrintJob.setLabel("");
    dicomPrintJob.setMediumType("BLUE FILM");
    dicomPrintJob.setNumberOfCopies(1);
    dicomPrintJob.setPrintPriority("MED");
    dicomPrintJob.setDicomPrintPages(getFakeDicomPrintPage(studyUID, seriesUID, numberOfImages));

    return dicomPrintJob;
}

DicomPrintJob FakeObjects::getFakeDicomScopeDicomPrintJob(QString studyUID, QString seriesUID, int numberOfImages)
{
    DicomPrintJob dicomPrintJob;

    dicomPrintJob.setFilmDestination("STOREDPRINT");
    dicomPrintJob.setLabel("");
    dicomPrintJob.setMediumType("STOREDPRINT");
    dicomPrintJob.setNumberOfCopies(1);
    dicomPrintJob.setPrintPriority("MED");
    dicomPrintJob.setDicomPrintPages(getFakeDicomPrintPage(studyUID, seriesUID, numberOfImages));

    return dicomPrintJob;
}

QList<DicomPrintPage> FakeObjects::getFakeDicomPrintPage(QString studyUID, QString seriesUID, int numberOfImages)
{
    QList<DicomPrintPage> dicomPrintPageList;
    DicomMask maskImagesToPrint;
    QList<Image*> databaseImages, imagesToPrint;
    int index = 0, rows = 2, columns = 2;


    maskImagesToPrint.setStudyUID(studyUID);
    maskImagesToPrint.setSeriesUID(seriesUID);

    databaseImages = LocalDatabaseManager().queryImage(maskImagesToPrint);


    while (index < numberOfImages &&  index <databaseImages.count())
    {
        DicomPrintPage dicomPrintPage;
        int indexNumberOfImagesPage = 0;

        while (indexNumberOfImagesPage < rows * columns &&  index <databaseImages.count())
        {
            imagesToPrint.append(databaseImages.at(index));
            index++;
            indexNumberOfImagesPage++;
        }

        dicomPrintPage.setImagesToPrint(imagesToPrint);

        dicomPrintPage.setBorderDensity("BLACK");
        dicomPrintPage.setEmptyImageDensity("WHITE");
        dicomPrintPage.setFilmLayout("STANDARD\\2,2");
        dicomPrintPage.setFilmOrientation("PORTRAIT");
        dicomPrintPage.setFilmSize("8INX10IN");
        dicomPrintPage.setMagnificationType("CUBIC");
        dicomPrintPage.setMaxDensity(20);
        dicomPrintPage.setMinDensity(1);
        dicomPrintPage.setPageNumber(1);
        dicomPrintPage.setPolarity("NORMAL");
        //dicomPrintPage.setSmoothingType(""); depen de MagnificationType TODO:Testejar
        dicomPrintPage.setTrim(true);

        dicomPrintPageList.append(dicomPrintPage);
    }
    return dicomPrintPageList;
}

}