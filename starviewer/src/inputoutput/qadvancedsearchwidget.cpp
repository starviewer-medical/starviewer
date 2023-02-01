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

#include "qadvancedsearchwidget.h"

#include <QCalendarWidget>

#include "dicommask.h"
#include "qbasicsearchwidget.h"

namespace udg {

static const QRegularExpression regExpGetMemberWidgets("^m_*");

QAdvancedSearchWidget::QAdvancedSearchWidget(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);

    createConnections();
    initialize();

    m_patientAge->setValidator(new QIntValidator(0, 199, m_patientAge));
}

void QAdvancedSearchWidget::initialize()
{
    m_fromPatientBirth->setDisplayFormat("dd/MM/yyyy");
    m_toPatientBirth->setDisplayFormat("dd/MM/yyyy");

    m_fromPatientBirth->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
    m_toPatientBirth->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
}

void QAdvancedSearchWidget::createConnections()
{
    connect(m_fromPatientBirthCheckBox, SIGNAL(toggled(bool)), m_fromPatientBirth, SLOT(setEnabled(bool)));
    connect(m_toPatientBirthCheckBox, SIGNAL(toggled(bool)), m_toPatientBirth, SLOT(setEnabled(bool)));
    connect(m_fromPatientBirth, SIGNAL(dateChanged(const QDate&)), SLOT(fromPatientBirthDateChanged()));
    connect(m_toPatientBirth, SIGNAL(dateChanged(const QDate&)), SLOT(toPatientBirthDateChanged()));

    //Creem connect per saber quan alguns dels controls per filtrar té valor, el findChildren fem que només ens retorni els widgets afegits
    //per nosaltres a la UI.
    foreach (QLineEdit *lineEdit, m_qwidgetAdvancedSearch->findChildren<QLineEdit*>(regExpGetMemberWidgets))
    {
        connect(lineEdit, SIGNAL(textChanged(const QString&)), SLOT(updateAdvancedSearchModifiedStatus()));
    }

    foreach (QCheckBox *checkBox, m_qwidgetAdvancedSearch->findChildren<QCheckBox*>())
    {
        connect(checkBox, SIGNAL(toggled(bool)), SLOT(updateAdvancedSearchModifiedStatus()));
    }
}

void QAdvancedSearchWidget::clear()
{
    //Camps pacient
    m_fromPatientBirthCheckBox->setChecked(false);
    m_toPatientBirthCheckBox->setChecked(false);
    m_patientAge->clear();
    
    // Camps estudi
    m_studyTimeLineEdit->clear();
    m_modalitiesInStudyLineEdit->clear();
    m_referringPhysiciansNameLineEdit->clear();
    m_studyDescriptionLineEdit->clear();
    m_accessionNumberLineEdit->clear();
    m_studyInstanceUidLineEdit->clear();
    m_studyIdLineEdit->clear();

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

    //El format de PatientAge de DICOM són 3 dígits + Y Ex: 054Y.  PS3.5 Taula 6.2-1 VR AS
    mask.setPatientAge(m_patientAge->text().isEmpty() ? "" : m_patientAge->text().rightJustified(3, '0') + "Y");
    mask.setPatientSex("");
    mask.setPatientBirth(m_fromPatientBirthCheckBox->isChecked() ? m_fromPatientBirth->date() : QDate(),
        m_toPatientBirthCheckBox->isChecked() ? m_toPatientBirth->date() : QDate());
    mask.setPatientID("");
    mask.setPatientName("");

    mask.setStudyID(m_studyIdLineEdit->text());
    QString studyDescription = m_studyDescriptionLineEdit->text();
    QBasicSearchWidget::addWildCards(studyDescription);
    mask.setStudyDescription(studyDescription);
    mask.setStudyInstanceUID(m_studyInstanceUidLineEdit->text());
    mask.setStudyModality(m_modalitiesInStudyLineEdit->text());
    mask.setAccessionNumber(m_accessionNumberLineEdit->text());
    mask.setReferringPhysiciansName(m_referringPhysiciansNameLineEdit->text());
    QPair<QTime, QTime> studyTimeRange = getTimeRangeToSearchFromString(m_studyTimeLineEdit->text());
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

        //Filtrem que els Widgets que ens retorna el mètode findChildren siguin Widgets afegits per nosaltres al UI, si no filtrèssim
        //el mètode findChildren ens retornaria tots els objectes de Qt que composen el tab, per exemple pel tab patient el QDateEdit està format 
        //per entre altres controls per QLineEdit, això ens pot generar problemes per saber si en aquell tab tenim algun valor per filtrar a les cerques
        //perquè trobaríem un QLineEdit amb valor
        foreach (QObject *child, tab->findChildren<QObject*>(regExpGetMemberWidgets))
        {
            if (isQLineEditEnabledAndIsNotEmpty(child) || isQCheckboxAndIsChecked(child))
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

void QAdvancedSearchWidget::fromPatientBirthDateChanged()
{
    m_toPatientBirth->setDate(m_fromPatientBirth->date());
}

void QAdvancedSearchWidget::toPatientBirthDateChanged()
{
    if (m_fromPatientBirth->date() > m_toPatientBirth->date())
    {
        m_fromPatientBirth->setDate(m_toPatientBirth->date());
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

bool QAdvancedSearchWidget::isQLineEditEnabledAndIsNotEmpty(QObject *qObject)
{
    if (qobject_cast<QLineEdit*>(qObject) != nullptr)
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(qObject);
        return !lineEdit->text().isEmpty() && lineEdit->isEnabled();
    }
    else
    {
        return false;
    }
}

bool QAdvancedSearchWidget::isQCheckboxAndIsChecked(QObject *qObject)
{
    if (qobject_cast<QCheckBox*>(qObject) != nullptr)
    {
        return (qobject_cast<QCheckBox*>(qObject))->isChecked();
    }
    else
    {
        return false;
    }
}

}
