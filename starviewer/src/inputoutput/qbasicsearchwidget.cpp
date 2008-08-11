/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qbasicsearchwidget.h"

#include <QString>

#include "dicommask.h"
#include "status.h"

namespace udg
{

QBasicSearchWidget::QBasicSearchWidget(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);

    createConnections();

    initialize();
}

void QBasicSearchWidget::createConnections()
{
    connect(m_textOtherModality, SIGNAL(editingFinished()), SLOT(textOtherModalityEdited()));

    connect(m_fromStudyDate, SIGNAL(dateChanged(QDate)), SLOT(checkFromDate(QDate)));
    connect(m_toStudyDate, SIGNAL(dateChanged(QDate)), SLOT(checkToDate(QDate)));
}

void QBasicSearchWidget::initialize()
{
    m_fromStudyDate->setDate(QDate::currentDate());
    m_toStudyDate->setDate(QDate::currentDate());

    m_patientNameText->setFocus();
}

void QBasicSearchWidget::clear()
{
    m_patientIDText->clear();
    m_patientNameText->clear();

    clearSeriesModality();

    m_anyDateRadioButton->setChecked(true);
}

void QBasicSearchWidget::setEnabledSeriesModality(bool enabled)
{
    m_buttonGroupModality->setEnabled(enabled);
    clearSeriesModality();
}

DicomMask QBasicSearchWidget::buildDicomMask()
{
    /*Per fer cerques entre valors consultat el capítol 4 de DICOM punt C.2.2.2.5*/
    /*Per defecte si passem un valor buit a la màscara,farà una cerca per tots els els valor d'aquella camp*/
    /*En aquí hem de fer un set a tots els camps que volem cercar */
    DicomMask mask;
    QString modalityMask;

    mask.setStudyId("");
    mask.setStudyDescription("");
    mask.setStudyTime("");
    mask.setStudyUID("");
    mask.setStudyModality("");
    mask.setPatientAge("");
    mask.setAccessionNumber("");
    mask.setReferringPhysiciansName("");
    mask.setPatientSex("");
    mask.setPatientBirth("");

    //S'afegeix '*' al patientId i patientName automàticament
    QString patientID = m_patientIDText->text();
    if (!patientID.startsWith("*")) patientID = "*" + patientID;
    if (!patientID.endsWith("*")) patientID = patientID + "*";
    mask.setPatientId(patientID);

    QString patientName = m_patientNameText->text();
    if (!patientName.startsWith("*")) patientName = "*" + patientName;
    if (!patientName.endsWith("*")) patientName = patientName + "*";
    mask.setPatientName(patientName);

    mask.setStudyDate(getStudyDatesStringMask());

    //si hem de filtrar per un camp a nivell d'imatge o serie activem els filtres de serie
    if (!m_checkAll->isChecked())
    {
        mask.setSeriesDate("");
        mask.setSeriesTime("");
        mask.setSeriesModality("");
        mask.setSeriesNumber("");
        mask.setSeriesUID("");
        mask.setRequestAttributeSequence("" , "");
        mask.setPPSStartDate("");
        mask.setPPStartTime("");

        if (m_buttonGroupModality->isEnabled())
        { //es crea una sentencia per poder fer un in
            if (m_checkCT->isChecked())
            {
                mask.setSeriesModality("CT");
            }
            else if (m_checkCR->isChecked())
            {
                mask.setSeriesModality("CR");
            }
            else if (m_checkDX->isChecked())
            {
                mask.setSeriesModality("DX");
            }
            else if (m_checkES->isChecked())
            {
                mask.setSeriesModality("ES");
            }
            else if (m_checkMG->isChecked())
            {
                mask.setSeriesModality("MG");
            }
            else if (m_checkMR->isChecked())
            {
                mask.setSeriesModality("MR");
            }
            else if (m_checkNM->isChecked())
            {
                mask.setSeriesModality("NM");
            }
            else if (m_checkDT->isChecked())
            {
                mask.setSeriesModality("DT");
            }
            else if (m_checkPT->isChecked())
            {
                mask.setSeriesModality("PT");
            }
            else if (m_checkRF->isChecked())
            {
                mask.setSeriesModality("RF");
            }
            else if (m_checkSC->isChecked())
            {
                mask.setSeriesModality("SC");
            }
            else if (m_checkUS->isChecked())
            {
                mask.setSeriesModality("US");
            }
            else if (m_checkXA->isChecked())
            {
                mask.setSeriesModality("XA");
            }
            else if (m_checkOtherModality->isChecked())
            {
                mask.setSeriesModality(m_textOtherModality->text());
            }
        }
    }

    return mask;
}

QString QBasicSearchWidget::getStudyDatesStringMask()
{
    if (m_anyDateRadioButton->isChecked())
    {
        return "";
    }
    else if (m_todayRadioButton->isChecked())
    {
        return QDate::currentDate().toString("yyyyMMdd");
    }
    else if (m_yesterdayRadioButton->isChecked())
    {
        return QDate::currentDate().addDays(-1).toString("yyyyMMdd");
    }
    else if (m_lastWeekRadioButton->isChecked())
    {
        return QDate::currentDate().addDays(-7).toString("yyyyMMdd") + "-" + QDate::currentDate().toString("yyyyMMdd");
    }
    else if (m_customDateRadioButton->isChecked())
    {
        QString date;

        if (m_fromDateCheck->isChecked() && m_toDateCheck->isChecked())
        {
            if (m_fromStudyDate->date() == m_toStudyDate->date())
            {
                date = m_fromStudyDate->date().toString("yyyyMMdd");
            }
            else
            {
                date = m_fromStudyDate->date().toString("yyyyMMdd") + "-" + m_toStudyDate->date().toString("yyyyMMdd");
            }
        }
        else
        {
            if (m_fromDateCheck->isChecked())
            {
                // indiquem que volem buscar tots els estudis d'aquella data en endavant
                date = m_fromStudyDate->date().toString("yyyyMMdd") + "-";
            }
            else if (m_toDateCheck->isChecked())
            {
                //indiquem que volem buscar tots els estudis que no superin aquesta data
                date = "-"+ m_toStudyDate->date().toString("yyyyMMdd");
            }
        }

        return date;
    }
    return "";
}

void QBasicSearchWidget::textOtherModalityEdited()
{
    if (m_textOtherModality->text().isEmpty())
    {
        m_checkAll->setChecked(true);
    }
}

void QBasicSearchWidget::checkFromDate(QDate date)
{
    if (date > m_toStudyDate->date())
    {
        m_toStudyDate->setDate(date);
    }
}

void QBasicSearchWidget::checkToDate(QDate date)
{
    if (date < m_fromStudyDate->date())
    {
        m_fromStudyDate->setDate(date);;
    }
}

void QBasicSearchWidget::clearSeriesModality()
{
    m_checkAll->setChecked(true);
    m_textOtherModality->clear();
}

QBasicSearchWidget::~QBasicSearchWidget()
{
}


}
