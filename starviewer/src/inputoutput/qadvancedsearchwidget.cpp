#include "qadvancedsearchwidget.h"

#include "dicommask.h"
#include "status.h"

namespace udg {

QAdvancedSearchWidget::QAdvancedSearchWidget(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);

    createConnections();
}

void QAdvancedSearchWidget::createConnections()
{
    foreach (QLineEdit *lineEdit, m_qwidgetAdvancedSearch->findChildren<QLineEdit*>())
    {
        connect(lineEdit, SIGNAL(textChanged(const QString &)), SLOT(updateAdvancedSearchModifiedStatus()));
    }
}

void QAdvancedSearchWidget::clear()
{
    // Camps estudi
    m_studyIDText->clear();
    m_studyUIDText->clear();
    m_accessionNumberText->clear();
    m_referringPhysiciansNameText->clear();
    m_studyModalityText->clear();
    m_studyTimeText->clear();

    // Camps sèries
    m_seriesUIDText->clear();
    m_requestedProcedureIDText->clear();
    m_scheduledProcedureStepIDText->clear();
    m_PPStartDateText->clear();
    m_PPStartTimeText->clear();
    m_seriesNumberText->clear();

    // Camps imatge
    m_SOPInstanceUIDText->clear();
    m_instanceNumberText->clear();
}

DicomMask QAdvancedSearchWidget::buildDicomMask()
{
    // Per fer cerques entre valors consultat el capítol 4 de DICOM punt C.2.2.2.5
    // Per defecte si passem un valor buit a la màscara,farà una cerca per tots els els valor d'aquella camp
    // En aquí hem de fer un set a tots els camps que volem cercar
    DicomMask mask;

    mask.setPatientAge("");
    mask.setPatientSex("");
    mask.setPatientBirth(QDate(), QDate());
    mask.setPatientID("");
    mask.setPatientName("");

    mask.setStudyID(m_studyIDText->text());
    mask.setStudyDescription("");
    mask.setStudyInstanceUID(m_studyUIDText->text());
    mask.setStudyModality(m_studyModalityText->text());
    mask.setAccessionNumber(m_accessionNumberText->text());
    mask.setReferringPhysiciansName(m_referringPhysiciansNameText->text());
    QPair<QTime, QTime> studyTimeRange = getTimeRangeToSearchFromString(m_studyTimeText->text());
    mask.setStudyTime(studyTimeRange.first, studyTimeRange.second);


    // Si hem de filtrar per un camp a nivell d'imatge o serie activem els filtres de serie
    if (!m_seriesUIDText->text().isEmpty() || !m_scheduledProcedureStepIDText->text().isEmpty() ||
        !m_requestedProcedureIDText->text().isEmpty() ||
        !m_SOPInstanceUIDText->text().isEmpty() || !m_instanceNumberText->text().isEmpty() ||
        !m_PPStartDateText->text().isEmpty() || !m_PPStartTimeText->text().isEmpty() ||
        !m_seriesNumberText->text().isEmpty())
    {
        mask.setSeriesDate(QDate(), QDate());
        mask.setSeriesTime(QTime(), QTime());
        mask.setSeriesModality("");
        mask.setSeriesNumber(m_seriesNumberText->text());
        mask.setSeriesInstanceUID(m_seriesUIDText->text());
        mask.setRequestAttributeSequence(m_requestedProcedureIDText->text(), m_scheduledProcedureStepIDText->text());

        QPair<QDate, QDate> rangePPSStartDate = getDateRangeToSearchFromString(m_PPStartDateText->text());
        mask.setPPSStartDate(rangePPSStartDate.first, rangePPSStartDate.second);
        QPair<QTime, QTime> rangePPSStartTime = getTimeRangeToSearchFromString(m_PPStartTimeText->text());
        mask.setPPStartTime(rangePPSStartTime.first, rangePPSStartTime.second);

        if (!m_SOPInstanceUIDText->text().isEmpty() || !m_instanceNumberText->text().isEmpty())
        {
            mask.setImageNumber(m_instanceNumberText->text());
            mask.setSOPInstanceUID(m_SOPInstanceUIDText->text());
        }

    }

    return mask;
}

void QAdvancedSearchWidget::updateAdvancedSearchModifiedStatus()
{
    for (int i = 0; i < m_qwidgetAdvancedSearch->count(); i++)
    {
        bool hasModifiedLineEdit = false;
        QWidget *tab = m_qwidgetAdvancedSearch->widget(i);

        foreach (QLineEdit *lineEdit, tab->findChildren<QLineEdit*>())
        {
            if (lineEdit->text() != "")
            {
                hasModifiedLineEdit = true;
                break;
            }
        }
        QString tabText = m_qwidgetAdvancedSearch->tabText(i).remove(QRegExp("\\*$"));
        if (hasModifiedLineEdit)
        {
            tabText += "*";
        }
        m_qwidgetAdvancedSearch->setTabText(i, tabText);
    }
}

QPair<QDate, QDate> QAdvancedSearchWidget::getDateRangeToSearchFromString(QString dateRangeAsDICOMFormat)
{
    QPair<QDate, QDate> rangeDate;

    if (!dateRangeAsDICOMFormat.contains("-"))
    {
        //No és un rang
        rangeDate.first = QDate().fromString(dateRangeAsDICOMFormat, "yyyyMMdd");
        rangeDate.second = QDate().fromString(dateRangeAsDICOMFormat, "yyyyMMdd");
    }
    else
    {
        QStringList rangeDateSplitted = dateRangeAsDICOMFormat.split("-");

        if (rangeDateSplitted.count() > 1)
        {
            rangeDate.first = QDate().fromString(rangeDateSplitted.at(0), "yyyyMMdd");
            rangeDate.second = QDate().fromString(rangeDateSplitted.at(1), "yyyyMMdd");
        }
    }

    return rangeDate;
}

QPair<QTime, QTime> QAdvancedSearchWidget::getTimeRangeToSearchFromString(QString timeRangeAsDICOMFormat)
{
    QPair<QTime, QTime> rangeTime;

    if (!timeRangeAsDICOMFormat.contains("-"))
    {
        //No és un rang
        rangeTime.first = QTime().fromString(timeRangeAsDICOMFormat, "HHmmss");
        rangeTime.second = QTime().fromString(timeRangeAsDICOMFormat, "HHmmss");
    }
    else
    {
        QStringList rangeTimeSplitted = timeRangeAsDICOMFormat.split("-");

        if (rangeTimeSplitted.count() > 1)
        {
            rangeTime.first = QTime().fromString(rangeTimeSplitted .at(0), "HHmmss");
            rangeTime.second = QTime().fromString(rangeTimeSplitted .at(1), "HHmmss");
        }
    }

    return rangeTime;
}


}
