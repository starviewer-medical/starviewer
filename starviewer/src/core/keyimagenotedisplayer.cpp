#include "keyimagenotedisplayer.h"
#include "image.h"
#include "series.h"
#include "keyimagenote.h"

namespace udg {

KeyImageNoteDisplayer::KeyImageNoteDisplayer(KeyImageNote *keyImageNote, QWidget *parent): QWidget(parent)
{
    setupUi(this);
    m_keyImageNote = keyImageNote;
    initialize();
}

KeyImageNoteDisplayer::~KeyImageNoteDisplayer()
{

}

void KeyImageNoteDisplayer::initialize()
{
    QString documentTitle = m_keyImageNote->getDocumentTitleAsString(m_keyImageNote->getDocumentTitle());
    m_documentTitleText->setText(documentTitle);

    QString rejectedForQualityReasons = m_keyImageNote->getRejectedForQualityReasonsAsString(m_keyImageNote->getRejectedForQualityReasons());
    m_documentTitleQualityReasonText->setText(rejectedForQualityReasons);

    QString observerContextType = m_keyImageNote->getObserverTypeAsString(m_keyImageNote->getObserverContextType());
    m_observerContextTypeText->setText(observerContextType);

    QString observerContextName = m_keyImageNote->getObserverContextName();
    m_observerContextNameText->setText(observerContextName);

    QString keyObjectDescription = m_keyImageNote->getKeyObjectDescription();
    m_keyObjectDescriptionText->setText(keyObjectDescription);

    initializeReferencedImagesWidget();
}

void KeyImageNoteDisplayer::initializeReferencedImagesWidget()
{
    QSize size;
    size.setHeight(scaledImagesSizeY);
    size.setWidth(scaledImagesSizeX);
    m_referencedImagesWidget->setIconSize(size);
    
    QList<Image*> referencedImages = m_keyImageNote->getReferencedImages();

    foreach(Image *image, referencedImages)
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
    if(image->getInstanceNumber() != NULL)
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
