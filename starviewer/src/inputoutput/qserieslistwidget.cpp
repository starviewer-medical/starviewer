#include "qserieslistwidget.h"

#include <QString>
#include <QIcon>

#include "series.h"

namespace udg {

QSeriesListWidget::QSeriesListWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    m_nonDicomImageSeriesList << "KO" << "PR" << "SR";
    m_lastInsertedImageRow = -1;

    createConnections();
}

void QSeriesListWidget::insertSeries(QString studyInstanceUID, Series *series)
{
    QString text;

    text = tr(" Series ") + series->getSeriesNumber();
    // Si hi ha descripció la inserim
    if (series->getProtocolName().length() > 0)
    {
        text += " " + series->getProtocolName();
    }
    text +="\n";

    if (series->getNumberOfImages() > 0)
    {
        text += QString::number(series->getNumberOfImages());
        QString modality = series->getModality();
        if (modality == "KO")
        {
            text += tr(" Key Object Note");
        }
        else if (modality == "PR")
        {
            text += tr(" Presentation State");
        }
        else if (modality == "SR")
        {
            text += tr(" Structured Report");
        }
        else
        {
            text += tr(" Images");
        }
    }

    // Guardem per la sèrie a quin estudi pertany
    m_HashSeriesStudy[series->getInstanceUID()] = studyInstanceUID;

    // TODO s'hauria de millorar el sistema d'ordenació de les sèries
    // Comprovem la posició que hem d'inserir la sèrie, si és un DICOM Non-Image (no és una imatge) val final, sinó va després de la última imatge inserida
    if (m_nonDicomImageSeriesList.contains(series->getModality()))
    {
        m_seriesThumbnailsPreviewWidget->append(series->getInstanceUID(), series->getThumbnail(), text);
    }
    else
    {
        // És una imatge
        m_lastInsertedImageRow++;
        m_seriesThumbnailsPreviewWidget->insert(m_lastInsertedImageRow, series->getInstanceUID(), series->getThumbnail(), text);
    }
}

void QSeriesListWidget::removeSeries(const QString &seriesInstanceUID)
{
    m_seriesThumbnailsPreviewWidget->remove(seriesInstanceUID);
}

QString QSeriesListWidget::getCurrentSeriesUID()
{
    return m_seriesThumbnailsPreviewWidget->getSelectedThumbnailID();
}

QString QSeriesListWidget::getCurrentStudyUID()
{
    return m_HashSeriesStudy[getCurrentSeriesUID()];
}

void QSeriesListWidget::setCurrentSeries(const QString &seriesUID)
{
    m_seriesThumbnailsPreviewWidget->setCurrentThumbnail(seriesUID);
}

void QSeriesListWidget::clear()
{
    m_seriesThumbnailsPreviewWidget->clear();
    m_HashSeriesStudy.clear();
    // Indiquem que la última imatge insertada està a la posició 0 perquè hem un clear
    m_lastInsertedImageRow = -1;
}

void QSeriesListWidget::createConnections()
{
    connect(m_seriesThumbnailsPreviewWidget, SIGNAL(thumbnailClicked(QString)), this, SLOT(seriesClicked(QString)));
    connect(m_seriesThumbnailsPreviewWidget, SIGNAL(thumbnailDoubleClicked(QString)), this, SLOT(seriesDoubleClicked(QString)));
}

void QSeriesListWidget::seriesClicked(QString IDThumbnail)
{
    emit(selectedSeriesIcon(getCurrentStudyUID(), IDThumbnail));
}

void QSeriesListWidget::seriesDoubleClicked(QString IDThumbnail)
{
    Q_UNUSED(IDThumbnail)
    emit(viewSeriesIcon());
}

}
