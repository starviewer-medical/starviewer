#include "keyimagenotemanagerwidget.h"
#include "keyimagenote.h"
#include "keyimagenotemanager.h"
#include "keyimagenotedisplayer.h"

namespace udg {

KeyImageNoteManagerWidget::KeyImageNoteManagerWidget(KeyImageNoteManager *keyImageNoteManager, QWidget *parent)
: QWidget(parent)
{
    setupUi(this);
    m_keyImageNoteManager = keyImageNoteManager;
    initialize();
}

KeyImageNoteManagerWidget::~KeyImageNoteManagerWidget()
{

}

void KeyImageNoteManagerWidget::initialize()
{
    initializeKeyImageNoteDisplayers();
}

void KeyImageNoteManagerWidget::initializeKeyImageNoteDisplayers()
{
    foreach(KeyImageNote *keyImageNote, m_keyImageNoteManager->getKeyImageNotes())
    {
        createKeyImageNoteDisplayer(keyImageNote);
    }
}

void KeyImageNoteManagerWidget::createKeyImageNoteDisplayer(KeyImageNote *keyImageNote)
{
    KeyImageNoteDisplayer *keyImageNoteDisplayer = new KeyImageNoteDisplayer(keyImageNote);
    m_layout->addWidget(keyImageNoteDisplayer);
}

}

