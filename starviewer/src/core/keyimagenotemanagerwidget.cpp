#include "keyimagenotemanagerwidget.h"
#include "keyimagenote.h"
#include "keyimagenotemanager.h"
#include "keyimagenotedisplayer.h"
#include "keyimagenotecreatorwidget.h"

namespace udg {

KeyImageNoteManagerWidget::KeyImageNoteManagerWidget(QWidget *parent): QWidget(parent), m_keyImageNoteManager(0), m_keyImageNoteCreator(0)
{
    setupUi(this);
    m_thumbnailImageDisplayer->setThumbnailSize(ThumbnailImageDisplayer::Medium);
}

KeyImageNoteManagerWidget::~KeyImageNoteManagerWidget()
{

}

void KeyImageNoteManagerWidget::showKeyImageNoteManagerWidget()
{
    if (!m_isKeyImageNoteManagerDataLoaded)
    {
        generateKeyImageNoteDisplayers();
        m_isKeyImageNoteManagerDataLoaded = true;
    }
}

void KeyImageNoteManagerWidget::setKeyImageNoteManager(KeyImageNoteManager *keyImageNoteManager)
{
    m_keyImageNoteManager = keyImageNoteManager;
    m_isKeyImageNoteManagerDataLoaded = false;
    createConnections();
}

void KeyImageNoteManagerWidget::createConnections()
{
    connect(m_createKeyImageNotePushButton, SIGNAL(clicked()), SLOT(showKeyImageNoteCreatorWidget()));
    connect(m_keyImageNoteManager, SIGNAL(imageAddedToTheCurrentSelectionOfImages(Image*)), m_thumbnailImageDisplayer,SLOT(addImage(Image*)));
    connect(m_keyImageNoteManager, SIGNAL(currentSelectionCleared()), m_thumbnailImageDisplayer, SLOT(clearAllThumbnails()));
}

void KeyImageNoteManagerWidget::generateKeyImageNoteDisplayers()
{
    if (m_keyImageNoteManager)
    {
        foreach (KeyImageNote *keyImageNote, m_keyImageNoteManager->getKeyImageNotesOfPatient())
        {
            createKeyImageNoteDisplayer(keyImageNote);
        }
    }
}

void KeyImageNoteManagerWidget::createKeyImageNoteDisplayer(KeyImageNote *keyImageNote)
{
    KeyImageNoteDisplayer *keyImageNoteDisplayer = new KeyImageNoteDisplayer(keyImageNote);
    m_layout->addWidget(keyImageNoteDisplayer);
}

void KeyImageNoteManagerWidget::showKeyImageNoteCreatorWidget()
{
    if (!m_keyImageNoteCreator)
    {
        m_keyImageNoteCreator = new KeyImageNoteCreatorWidget(this, m_keyImageNoteManager);
    }
    
    m_keyImageNoteCreator->exec();
}

}
