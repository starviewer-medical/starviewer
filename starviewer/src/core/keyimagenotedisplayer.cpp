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
    m_documentTitleText->setText(m_keyImageNote->getDocumentTitleAsString(m_keyImageNote->getDocumentTitle()));
    m_documentTitleQualityReasonText->setText(m_keyImageNote->getRejectedForQualityReasonsAsString(m_keyImageNote->getRejectedForQualityReasons()));
    m_observerContextTypeText->setText(m_keyImageNote->getObserverTypeAsString(m_keyImageNote->getObserverContextType()));
    m_observerContextNameText->setText(m_keyImageNote->getObserverContextName());
    m_keyObjectDescriptionText->setText(m_keyImageNote->getKeyObjectDescription());

    fillReferencedImagesWidget();
}

void KeyImageNoteDisplayer::fillReferencedImagesWidget()
{
    QSize size;
    size.setHeight(ScaledThumbnailsSizeY);
    size.setWidth(ScaledThumbnailsSizeX);
    m_referencedImagesWidget->setIconSize(size);
    
    QList<Image*> referencedImages = m_keyImageNote->getReferencedImages();

    foreach (Image *image, referencedImages)
    {
        insertImage(image);
    }
}

void KeyImageNoteDisplayer::insertImage(Image *image)
{
    QListWidgetItem *item = new QListWidgetItem();

    QIcon icon(image->getThumbnail());
    item->setIcon(icon);

    QString text;
    if (image->getInstanceNumber() != NULL)
    {
        text = tr( " Image " ) + image->getInstanceNumber();
        text += "\n";
        text += tr( " Series " ) + image->getParentSeries()->getSeriesNumber();
    }
    else
    {
        text = tr( " SOP Instance UID " ) + image->getSOPInstanceUID();
        text += "\n";
    }
    item->setText(text);

    m_referencedImagesWidget->addItem(item);
}

}
