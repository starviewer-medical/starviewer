#include "qserieslistwidget.h"

#include <QString>

#include "series.h"

namespace udg {

QSeriesListWidget::QSeriesListWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    m_DICOMModalitiesNonImage << "KO" << "PR" << "SR";
    m_positionOfLastInsertedThumbnail = -1;

    createConnections();
}

void QSeriesListWidget::insertSeries(QString studyInstanceUID, Series *series)
{
    QString seriesThumbnailDescription = getSeriesThumbnailDescription(series);
    m_studyInstanceUIDBySeriesInstanceUID[series->getInstanceUID()] = studyInstanceUID;

    // Comprovem la posició que hem d'inserir la sèrie, si és un DICOM Non-Image (no és una imatge) val final, sinó va després de la última imatge inserida
    if (m_DICOMModalitiesNonImage.contains(series->getModality()))
    {
        m_seriesThumbnailsPreviewWidget->append(series->getInstanceUID(), series->getThumbnail(), seriesThumbnailDescription);
    }
    else
    {
        // És una imatge
        m_positionOfLastInsertedThumbnail++;
        m_seriesThumbnailsPreviewWidget->insert(m_positionOfLastInsertedThumbnail, series->getInstanceUID(), series->getThumbnail(), seriesThumbnailDescription);
    }
}

void QSeriesListWidget::removeSeries(const QString &seriesInstanceUID)
{
    m_seriesThumbnailsPreviewWidget->remove(seriesInstanceUID);
}

void QSeriesListWidget::setCurrentSeries(const QString &seriesUID)
{
    m_seriesThumbnailsPreviewWidget->setCurrentThumbnail(seriesUID);
}

void QSeriesListWidget::clear()
{
    m_seriesThumbnailsPreviewWidget->clear();
    m_studyInstanceUIDBySeriesInstanceUID.clear();
    // Indiquem que la última imatge insertada està a la posició 0 perquè hem un clear
    m_positionOfLastInsertedThumbnail = -1;
}

void QSeriesListWidget::createConnections()
{
    connect(m_seriesThumbnailsPreviewWidget, SIGNAL(thumbnailClicked(QString)), this, SLOT(seriesClicked(QString)));
    connect(m_seriesThumbnailsPreviewWidget, SIGNAL(thumbnailDoubleClicked(QString)), this, SLOT(seriesDoubleClicked(QString)));
}

QString QSeriesListWidget::getSeriesThumbnailDescription(Series *series)
{
    QString thumbnailDescription;
    thumbnailDescription = tr(" Series ") + series->getSeriesNumber();

    if (series->getProtocolName().length() > 0)
    {
        thumbnailDescription += " " + series->getProtocolName();
    }
    thumbnailDescription +="\n";

    if (series->getNumberOfImages() > 0)
    {
        thumbnailDescription += QString::number(series->getNumberOfImages());

        if (series->getModality() == "KO")
        {
            thumbnailDescription += tr(" Key Object Note");
        }
        else if (series->getModality() == "PR")
        {
            thumbnailDescription += tr(" Presentation State");
        }
        else if(series->getModality() == "SR")
        {
            thumbnailDescription += tr(" Structured Report");
        }
        else
        {
            thumbnailDescription += tr(" Images");
        }
    }

    return thumbnailDescription;
}

void QSeriesListWidget::seriesClicked(QString IDThumbnail)
{
    emit(seriesThumbnailClicked(m_studyInstanceUIDBySeriesInstanceUID[IDThumbnail], IDThumbnail));
}

void QSeriesListWidget::seriesDoubleClicked(QString IDThumbnail)
{
    emit(seriesThumbnailDoubleClicked(m_studyInstanceUIDBySeriesInstanceUID[IDThumbnail], IDThumbnail));
}

}
