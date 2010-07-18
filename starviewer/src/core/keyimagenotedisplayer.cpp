#include "keyimagenotedisplayer.h"
#include "image.h"
#include "series.h"
#include "keyimagenote.h"

namespace udg {

KeyImageNoteDisplayer::KeyImageNoteDisplayer(KeyImageNote *keyImageNote, QWidget *parent): QWidget(parent)
{
    setupUi(this);
    m_keyImageNote = keyImageNote;
    fillKeyImageNoteDisplayer();
}

KeyImageNoteDisplayer::~KeyImageNoteDisplayer()
{

}

void KeyImageNoteDisplayer::fillKeyImageNoteDisplayer()
{
    m_documentTitleText->setText(m_keyImageNote->getDocumentTitleAsString());
    m_documentTitleQualityReasonText->setText(m_keyImageNote->getRejectedForQualityReasonsAsString());
    m_observerContextTypeText->setText(m_keyImageNote->getObserverTypeAsString());
    m_observerContextNameText->setText(m_keyImageNote->getObserverContextName());
    m_keyObjectDescriptionText->setText(m_keyImageNote->getKeyObjectDescription());
    m_contentDate->setText(m_keyImageNote->getContentDate().toString("dd/MM/yyyy"));
    m_contentTime->setText(m_keyImageNote->getContentTime().toString("hh:mm"));

    fillReferencedImagesWidget();
}

void KeyImageNoteDisplayer::fillReferencedImagesWidget()
{
    m_thumbnailImageDisplayer->setThumbnailSize(ThumbnailImageDisplayer::Medium);

    foreach (Image *image, m_keyImageNote->getReferencedImages())
    {
        m_thumbnailImageDisplayer->addImage(image);
    }
}

}
