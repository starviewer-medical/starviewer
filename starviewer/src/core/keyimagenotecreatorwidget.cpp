#include "keyimagenotecreatorwidget.h"
#include "keyimagenotemanager.h"
#include "keyimagenote.h"
#include "pacsdevicemanager.h"

namespace udg {

KeyImageNoteCreatorWidget::KeyImageNoteCreatorWidget(QWidget *parent, KeyImageNoteManager *keyImageNoteManager): QDialog(parent)
{
    Q_ASSERT(keyImageNoteManager);
    m_keyImageNoteManager = keyImageNoteManager;
    setupUi(this);

    createConnections();
    m_documentTitleComboBox->addItems(KeyImageNote::getAllDocumentTitles());
    m_documentTitleQualityReasonsComboBox->addItems(KeyImageNote::getAllRejectedForQualityReasons());

    setDefaultValuesOfKeyImageNoteCreatorWidget();
}

KeyImageNoteCreatorWidget::~KeyImageNoteCreatorWidget()
{

}

void KeyImageNoteCreatorWidget::createConnections()
{
    connect(m_saveButton, SIGNAL(clicked()), SLOT(createKeyImageNote()));
    connect(m_cancelButton, SIGNAL(clicked()), SLOT(close()));
    connect(m_documentTitleComboBox, SIGNAL(currentIndexChanged(const QString &)), SLOT(setVisibilityOfRejectedForQualityReasons(const QString &)));
    connect(m_sendToPacsCheckBox, SIGNAL(toggled(bool)), m_pacsNodeComboBox , SLOT(setEnabled(bool)));
    connect(m_storeToLocalCheckBox, SIGNAL(clicked(bool)), this, SLOT(destinationsChanged(bool)));
    connect(m_sendToPacsCheckBox, SIGNAL(clicked(bool)), this, SLOT(destinationsChanged(bool)));
}

void KeyImageNoteCreatorWidget::setVisibilityOfRejectedForQualityReasons(const QString &documentTitle)
{
    if (KeyImageNote::isDocumentTitleModifiedForQualityReasonsOrIssues(KeyImageNote::getDocumentTitleInstanceFromString(documentTitle)))
    {
        m_documentTitleQualityReasonsComboBox->setVisible(true);
        m_rejectedForQualityReasonsLabel->setVisible(true);
    }
    else
    {
        m_documentTitleQualityReasonsComboBox->setVisible(false);
        m_rejectedForQualityReasonsLabel->setVisible(false);
    }
}

void KeyImageNoteCreatorWidget::createKeyImageNote()
{
    m_keyImageNoteManager->generateAndStoreNewKeyImageNote(m_documentTitleComboBox->currentText(), m_documentTitleQualityReasonsComboBox->currentText(), m_observerName->text(), m_keyObjectDescription->toPlainText(), m_storeToLocalCheckBox->isChecked(), m_sendToPacsCheckBox->isChecked(), m_pacsNodeComboBox->currentText());
    setDefaultValuesOfKeyImageNoteCreatorWidget();
    close();
}

void KeyImageNoteCreatorWidget::setDefaultValuesOfKeyImageNoteCreatorWidget()
{
    m_observerName->clear();
    m_keyObjectDescription->clear();
    m_documentTitleComboBox->setCurrentIndex(0);
    m_documentTitleQualityReasonsComboBox->setCurrentIndex(0);
    
    // Omplim la llista de pacs. En cas que sigui buida la opció d'enviar a PACS quedarà deshabilitada.

    PacsDeviceManager deviceManager;

    if (deviceManager.getPACSList(PacsDeviceManager::PacsWithStoreServiceEnabled).size() == 0)
    {
        m_sendToPacsCheckBox->setEnabled(false);
    }
    else
    {
        m_pacsNodeComboBox->clear();
        foreach (PacsDevice device, deviceManager.getPACSList(PacsDeviceManager::PacsWithStoreServiceEnabled))
        {
            m_pacsNodeComboBox->addItem(QString("%1 - %2").arg(device.getAETitle(), device.getDescription()), device.getID());
        }
    }
}

void KeyImageNoteCreatorWidget::destinationsChanged(bool checked)
{
    if (checked)
    {
        if (m_sendToPacsCheckBox->isChecked())
        {
            m_storeToLocalCheckBox->setChecked(true);
        }

        m_saveButton->setEnabled(true);
    }
    else
    {
        if (!m_storeToLocalCheckBox->isChecked())
        {
            m_sendToPacsCheckBox->setChecked(false);
        }

        if (!m_storeToLocalCheckBox->isChecked() && !m_sendToPacsCheckBox->isChecked())
        {
            m_saveButton->setEnabled(false);
        }
    }
}
}

