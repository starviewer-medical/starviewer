#include "keyimagenotedisplayer.h"
#include "image.h"
#include "series.h"
#include "keyimagenote.h"
#include "keyimagenotemanager.h"

namespace udg {

KeyImageNoteDisplayer::KeyImageNoteDisplayer(QWidget *parent): QWidget(parent)
{
    setupUi(this);
}

KeyImageNoteDisplayer::~KeyImageNoteDisplayer()
{

}

void KeyImageNoteDisplayer::setKeyImageNote(KeyImageNote *keyImageNote)
{
    m_keyImageNote = keyImageNote;
    fillKeyImageNoteDisplayer();
}

void KeyImageNoteDisplayer::setKeyImageNoteManager(KeyImageNoteManager *keyImageNoteManager)
{
    m_keyImageNoteManager = keyImageNoteManager;
    connect(m_thumbnailImageDisplayer, SIGNAL(show(const QString &, const QString &)), m_keyImageNoteManager, SLOT(changeCurrentDisplayedImage(const QString &, const QString &)));
}

void KeyImageNoteDisplayer::fillKeyImageNoteDisplayer()
{
    QString titleText;
    titleText = KeyImageNote::getDocumentTitleAsString(m_keyImageNote->getDocumentTitle());

    if (m_keyImageNote->getRejectedForQualityReasons() != KeyImageNote::NoneRejectedForQualityReasons)
    {
        titleText += " " + tr("Quality Reasons:") + " " + KeyImageNote::getRejectedForQualityReasonsAsString(m_keyImageNote->getRejectedForQualityReasons());
    }

    m_documentTitleText->setText(titleText);

    QString observerText;

    if (m_keyImageNote->getObserverContextType() == KeyImageNote::Device)
    {
        observerText = tr("Device:") + " ";
    }

    observerText += m_keyImageNote->getObserverContextName();
    m_observerContextNameText->setText(observerText);

    m_keyObjectDescriptionText->setText(m_keyImageNote->getKeyObjectDescription());
    m_contentDate->setText(m_keyImageNote->getContentDate().toString("dd/MM/yyyy") + " " + m_keyImageNote->getContentTime().toString("hh:mm"));

    fillReferencedImagesWidget();
}

void KeyImageNoteDisplayer::fillReferencedImagesWidget()
{
    m_thumbnailImageDisplayer->clearAllThumbnails();
    m_thumbnailImageDisplayer->setThumbnailSize(ThumbnailImageDisplayer::Medium);

    foreach (Image *image, m_keyImageNote->getReferencedImages())
    {
        m_thumbnailImageDisplayer->addImage(image);
    }
}

}
