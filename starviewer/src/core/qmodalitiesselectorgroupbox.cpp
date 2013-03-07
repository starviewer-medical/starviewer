#include "qmodalitiesselectorgroupbox.h"

#include <QButtonGroup>

namespace udg {

QModalitiesSelectorGroupBox::QModalitiesSelectorGroupBox(QWidget *parent)
 : QGroupBox(parent)
{
    setupUi(this);
    initialize();
}

QModalitiesSelectorGroupBox::~QModalitiesSelectorGroupBox()
{
}

void QModalitiesSelectorGroupBox::enableAllModalitiesCheckBox(bool enable)
{
    m_allCheckBox->setVisible(enable);
}

void QModalitiesSelectorGroupBox::enableOtherModalitiesCheckBox(bool enable)
{
    m_otherCheckBox->setVisible(enable);
    m_otherLineEdit->setVisible(enable);
}

void QModalitiesSelectorGroupBox::setExclusive(bool exclusive)
{
    m_buttonGroup->setExclusive(exclusive);
}

void QModalitiesSelectorGroupBox::clear()
{
    foreach (QAbstractButton *button, m_buttonGroup->buttons())
    {
        button->setChecked(false);
    }
    m_otherLineEdit->clear();
}

QStringList QModalitiesSelectorGroupBox::getCheckedModalities()
{
    QStringList checkedModalities;

    foreach (QAbstractButton *button, m_buttonGroup->buttons())
    {
        // TODO Fer que 'All' i 'Other' siguin constants definides i que s'assignin als check box
        if (button->text() != tr("All") && button->text() != tr("Other") && button->isChecked())
        {
            checkedModalities << button->text();
        }
    }

    if (m_otherCheckBox->isChecked())
    {
        checkedModalities << m_otherLineEdit->text();
    }

    return checkedModalities;
}

void QModalitiesSelectorGroupBox::checkModalities(const QStringList &modalities)
{
    if (modalities.contains("CR"))
    {
        m_CRCheckBox->setChecked(true);
    }
    if (modalities.contains("CT"))
    {
        m_CTCheckBox->setChecked(true);
    }
    if (modalities.contains("DX"))
    {
        m_DXCheckBox->setChecked(true);
    }
    if (modalities.contains("ES"))
    {
        m_ESCheckBox->setChecked(true);
    }
    if (modalities.contains("MG"))
    {
        m_MGCheckBox->setChecked(true);
    }
    if (modalities.contains("MR"))
    {
        m_MRCheckBox->setChecked(true);
    }
    if (modalities.contains("NM"))
    {
        m_NMCheckBox->setChecked(true);
    }
    if (modalities.contains("OP"))
    {
        m_OPCheckBox->setChecked(true);
    }
    if (modalities.contains("PT"))
    {
        m_PTCheckBox->setChecked(true);
    }
    if (modalities.contains("RF"))
    {
        m_RFCheckBox->setChecked(true);
    }
    if (modalities.contains("SC"))
    {
        m_SCCheckBox->setChecked(true);
    }
    if (modalities.contains("US"))
    {
        m_USCheckBox->setChecked(true);
    }
    if (modalities.contains("XA"))
    {
        m_XACheckBox->setChecked(true);
    }
    if (modalities.contains("XC"))
    {
        m_XCCheckBox->setChecked(true);
    }
}

void QModalitiesSelectorGroupBox::initialize()
{
    // Inicialitzem el grup de check box
    m_buttonGroup = new QButtonGroup(this);
    m_buttonGroup->setExclusive(false);
    
    m_buttonGroup->addButton(m_CRCheckBox);
    m_buttonGroup->addButton(m_CTCheckBox);
    m_buttonGroup->addButton(m_DXCheckBox);
    m_buttonGroup->addButton(m_ESCheckBox);
    m_buttonGroup->addButton(m_MGCheckBox);
    m_buttonGroup->addButton(m_MRCheckBox);
    m_buttonGroup->addButton(m_NMCheckBox);
    m_buttonGroup->addButton(m_OPCheckBox);
    m_buttonGroup->addButton(m_PTCheckBox);
    m_buttonGroup->addButton(m_RFCheckBox);
    m_buttonGroup->addButton(m_SCCheckBox);
    m_buttonGroup->addButton(m_USCheckBox);
    m_buttonGroup->addButton(m_XACheckBox);
    m_buttonGroup->addButton(m_XCCheckBox);
    // Botons "especials"
    m_buttonGroup->addButton(m_allCheckBox);
    m_buttonGroup->addButton(m_otherCheckBox);
    
    // El quadre de text d'altres modalitats només estarà habilitat quan el corresponent check box estigui marcat
    m_otherLineEdit->setEnabled(false);
    connect(m_otherCheckBox, SIGNAL(toggled(bool)), m_otherLineEdit, SLOT(setEnabled(bool)));
}

void QModalitiesSelectorGroupBox::setAllModalitiesCheckBoxChecked(bool checked)
{
    m_allCheckBox->setChecked(checked);
}

bool QModalitiesSelectorGroupBox::isAllModalitiesCheckBoxChecked() const
{
    return m_allCheckBox->isChecked();
}

}
