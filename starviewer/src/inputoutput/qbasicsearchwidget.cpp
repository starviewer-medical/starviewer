#include "qbasicsearchwidget.h"

#include <QString>
#include <QCalendarWidget>

#include "dicommask.h"

namespace udg {

QBasicSearchWidget::QBasicSearchWidget(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);

    createConnections();

    initialize();
}

void QBasicSearchWidget::createConnections()
{
    connect(m_fromStudyDate, SIGNAL(dateChanged(QDate)), SLOT(checkFromDate(QDate)));
    connect(m_toStudyDate, SIGNAL(dateChanged(QDate)), SLOT(checkToDate(QDate)));
}

void QBasicSearchWidget::initialize()
{
    m_fromStudyDate->setDate(QDate::currentDate());
    m_toStudyDate->setDate(QDate::currentDate());
    // Indiquem que les setmanes del calendari que apareixen per escollir la data comencin el dia segons la locale configurada o la del sistema en el seu defecte
    QLocale locale;
    m_fromStudyDate->calendarWidget()->setFirstDayOfWeek(locale.firstDayOfWeek());
    m_toStudyDate->calendarWidget()->setFirstDayOfWeek(locale.firstDayOfWeek());

    m_fromStudyDate->setDisplayFormat("dd/MM/yyyy");
    m_toStudyDate->setDisplayFormat("dd/MM/yyyy");

    m_buttonGroupModality->enableAllModalitiesCheckBox(true);
    m_buttonGroupModality->enableOtherModalitiesCheckBox(true);
    m_buttonGroupModality->setExclusive(true);
    
    m_widgetHasBeenShowed = false;
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
    // Per fer cerques entre valors consultat el capítol 4 de DICOM punt C.2.2.2.5
    // Per defecte si passem un valor buit a la màscara,farà una cerca per tots els els valor d'aquella camp
    // En aquí hem de fer un set a tots els camps que volem cercar
    DicomMask mask;
    QString modalityMask;

    mask.setStudyID("");
    mask.setStudyDescription("");
    mask.setStudyTime(QTime(), QTime());
    mask.setStudyInstanceUID("");
    mask.setStudyModality("");
    mask.setPatientAge("");
    mask.setAccessionNumber("");
    mask.setReferringPhysiciansName("");
    mask.setPatientSex("");
    mask.setPatientBirth(QDate(), QDate());

    // Per PatientId i PatientName si el lineEdit és buit es fa un Universal Matching. Universal Matching és quan indiquem que cerquem per un tag de dicom però
    // no li donem valor, en aquest cas la normativa DICOM indica que el SCP ha de fer match per tots els objectes DICOM, el universal matching és un mecanisme
    // per indicar al fer c-find, d'aquell tag ens n'ha de retornar el valor que té tots els objecte DICOM que compleixen la cerca. La normativa
    // DICOM indica que fer una wildcard amb '*' és el mateix que fer Universal Matching. Nosaltres hem optat per fer Universal matching perquè hi ha algun scp
    // que si li passem un asterisc sol '*' al fer la cerca no es comportant correctament, per exemple retorna cap resultat.

    // Si pel contrari algun dels lineEdit tenen valor, llavors fem wild card matching "*" + valor "*", posant "*" a davant i a darrera del valor indiquem
    // que el SCP que ens ha de retornar tots els objectes dicom que per aquell tag alguna part de la seu text coincideix amb el valor que ens han indicat.

    // Per més informació consultar el PS 3.4 C.2.2.2

    // S'afegeix '*' al patientId i patientName automàticament
    QString patientID = m_patientIDText->text();
    if (!patientID.isEmpty())
    {
        if (!patientID.startsWith("*"))
        {
            patientID = "*" + patientID;
        }
        if (!patientID.endsWith("*"))
        {
            patientID = patientID + "*";
        }
    }
    mask.setPatientID(patientID);

    QString patientName = m_patientNameText->text();
    if (!patientName.isEmpty())
    {
        if (!patientName.startsWith("*"))
        {
            patientName = "*" + patientName;
        }
        if (!patientName.endsWith("*"))
        {
            patientName = patientName + "*";
        }
    }
    mask.setPatientName(patientName);

    setStudyDateToDICOMMask(&mask);

    // Si hem de filtrar per un camp a nivell d'imatge o serie activem els filtres de serie
    if (!m_buttonGroupModality->isAllModalitiesCheckBoxChecked())
    {
        mask.setSeriesDate(QDate(), QDate());
        mask.setSeriesTime(QTime(), QTime());
        mask.setSeriesModality("");
        mask.setSeriesNumber("");
        mask.setSeriesInstanceUID("");
        mask.setRequestAttributeSequence("", "");
        mask.setPPSStartDate(QDate(), QDate());
        mask.setPPStartTime(QTime(), QTime());

        if (m_buttonGroupModality->isEnabled())
        {
            // Es crea una sentencia per poder fer un in
            QStringList checkedModalities = m_buttonGroupModality->getCheckedModalities();
            // Com que el grup de modalitats és exclusiu, només s'hauria de tenir una marcada com a màxim
            if (!checkedModalities.isEmpty())
            {
                mask.setSeriesModality(checkedModalities.first());
            }
        }
    }

    return mask;
}

void QBasicSearchWidget::setDefaultDate(DefaultDateFlags flag)
{
    switch (flag)
    {
        case AnyDate:
            m_anyDateRadioButton->setChecked(true);
            break;

        case Today:
            m_todayRadioButton->setChecked(true);
            break;

        case Yesterday:
            m_yesterdayRadioButton->setChecked(true);
            break;

        case LastWeek:
            m_lastWeekRadioButton->setChecked(true);
            break;
    }
}

void QBasicSearchWidget::setStudyDateToDICOMMask(DicomMask *mask)
{
    if (m_anyDateRadioButton->isChecked())
    {
        mask->setStudyDate(QDate(), QDate());
    }
    else if (m_todayRadioButton->isChecked())
    {
        mask->setStudyDate(QDate::currentDate(), QDate::currentDate());
    }
    else if (m_yesterdayRadioButton->isChecked())
    {
        return mask->setStudyDate(QDate::currentDate().addDays(-1), QDate::currentDate().addDays(-1));
    }
    else if (m_lastWeekRadioButton->isChecked())
    {
        return mask->setStudyDate(QDate::currentDate().addDays(-7), QDate::currentDate());
    }
    else if (m_customDateRadioButton->isChecked())
    {
        if (m_fromDateCheck->isChecked() && m_toDateCheck->isChecked())
        {
            mask->setStudyDate(m_fromStudyDate->date(), m_toStudyDate->date());
        }
        else
        {
            if (m_fromDateCheck->isChecked())
            {
                // Indiquem que volem buscar tots els estudis d'aquella data en endavant
                mask->setStudyDate(m_fromStudyDate->date(), QDate());
            }
            else if (m_toDateCheck->isChecked())
            {
                // Indiquem que volem buscar tots els estudis que no superin aquesta data
                mask->setStudyDate(QDate(), m_toStudyDate->date());
            }
        }
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
    m_buttonGroupModality->clear();
    m_buttonGroupModality->setAllModalitiesCheckBoxChecked(true);
}

void QBasicSearchWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);

    if (!m_widgetHasBeenShowed)
    {
        // La primera vegada que mostrem el widget donem focus al patientName. Tot i que en teoria el TabOrder està ben definit pels controls del widget això
        // es fa per si mai ens equivoquem i s'altera el TabOrder de manera incorrecte. Com és un Widget molt utilitzat ens interessa assegurar-nos que sempre
        // en primer lloc està enfocant al PatientName, ja que és pel camp que més cerquen.
        m_patientNameText->setFocus();
        m_widgetHasBeenShowed = true;
    }
}
QBasicSearchWidget::~QBasicSearchWidget()
{
}

}
