#include "keyimagenotecreatorwidget.h"
#include "keyimagenotemanager.h"
#include "keyimagenote.h"

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
}

void KeyImageNoteCreatorWidget::setVisibilityOfRejectedForQualityReasons(const QString &documentTitle)
{
    if (KeyImageNote::documentTitleNeedRejectedForQualityReasons(KeyImageNote::getDocumentTitleInstanceFromString(documentTitle)))
    {
        m_documentTitleQualityReasonsComboBox->setVisible(true);
        m_rejectedForQualityReasonsLabel->setVisible(true);
    }
    else
    {
        m_documentTitleQualityReasonsComboBox->setVisible(false);
        m_rejectedForQualityReasonsLabel->setVisible(false);
    }

    adjustSize();
}

void KeyImageNoteCreatorWidget::createKeyImageNote()
{
    m_keyImageNoteManager->createKeyImageNote(m_documentTitleComboBox->currentText(), m_documentTitleQualityReasonsComboBox->currentText(), m_observerName->text(), m_keyObjectDescription->toPlainText());
    setDefaultValuesOfKeyImageNoteCreatorWidget();
    close();
}

void KeyImageNoteCreatorWidget::setDefaultValuesOfKeyImageNoteCreatorWidget()
{
    m_observerName->clear();
    m_keyObjectDescription->clear();
    m_documentTitleComboBox->setCurrentIndex(0);
    m_documentTitleQualityReasonsComboBox->setCurrentIndex(0);
}

}
