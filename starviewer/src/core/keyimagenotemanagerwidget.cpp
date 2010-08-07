#include "keyimagenotemanagerwidget.h"
#include "keyimagenote.h"
#include "keyimagenotemanager.h"
#include "keyimagenotedisplayer.h"
#include "keyimagenotecreatorwidget.h"

#include <QShortcut>

namespace udg {

KeyImageNoteManagerWidget::KeyImageNoteManagerWidget(QWidget *parent): QWidget(parent), m_keyImageNoteManager(0), m_keyImageNoteCreator(0)
{
    setupUi(this);
    initializeThumbnailImageDisplayer();
    createConnections();
}

KeyImageNoteManagerWidget::~KeyImageNoteManagerWidget()
{

}

void KeyImageNoteManagerWidget::showKeyImageNoteManagerWidget()
{
    Q_ASSERT(m_keyImageNoteManager);

    if (!m_isKeyImageNoteManagerDataLoaded)
    {
        generateKeyImageNoteDisplayers();
    }
}

void KeyImageNoteManagerWidget::setKeyImageNoteManager(KeyImageNoteManager *keyImageNoteManager)
{
    m_keyImageNoteManager = keyImageNoteManager;
    m_isKeyImageNoteManagerDataLoaded = false;
    createConnectionsWithKeyImageNoteManager();
}

void KeyImageNoteManagerWidget::createConnectionsWithKeyImageNoteManager()
{
    connect(m_keyImageNoteManager, SIGNAL(currentSelectionCleared()), m_thumbnailImageDisplayer, SLOT(clearAllThumbnails()));
    connect(m_keyImageNoteManager, SIGNAL(imageAddedToTheCurrentSelectionOfImages(Image*)), m_thumbnailImageDisplayer, SLOT(addImage(Image*)));
    connect(m_keyImageNoteManager, SIGNAL(keyImageNoteOfPatientAdded(KeyImageNote*)), SLOT(createKeyImageNoteDisplayer(KeyImageNote*)));
    connect(m_thumbnailImageDisplayer, SIGNAL(show(const QString &)), m_keyImageNoteManager, SLOT(changeCurrentDisplayedImage(const QString &)));
}

void KeyImageNoteManagerWidget::createConnections()
{
    connect(m_createKeyImageNotePushButton, SIGNAL(clicked()), SLOT(showKeyImageNoteCreatorWidget()));
}

void KeyImageNoteManagerWidget::generateKeyImageNoteDisplayers()
{
    foreach (KeyImageNote *keyImageNote, m_keyImageNoteManager->getKeyImageNotesOfPatient())
    {
        createKeyImageNoteDisplayer(keyImageNote);
    }

    m_isKeyImageNoteManagerDataLoaded = true;
}

void KeyImageNoteManagerWidget::createKeyImageNoteDisplayer(KeyImageNote *keyImageNote)
{
    KeyImageNoteDisplayer *keyImageNoteDisplayer = new KeyImageNoteDisplayer();
    keyImageNoteDisplayer->setKeyImageNote(keyImageNote);
    keyImageNoteDisplayer->setKeyImageNoteManager(m_keyImageNoteManager);
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

void KeyImageNoteManagerWidget::initializeThumbnailImageDisplayer()
{
    m_thumbnailImageDisplayer->setThumbnailSize(ThumbnailImageDisplayer::Medium);

    QMenu *contextMenuThumbnailImageDisplayer = new QMenu();
    contextMenuThumbnailImageDisplayer->addAction(QIcon(":/images/databaseRemove.png"), tr("&Delete"), this, SLOT(deleteSelectedItemsFromCurrentSelection()), Qt::Key_Delete);
    m_thumbnailImageDisplayer->setContextMenu(contextMenuThumbnailImageDisplayer);
}

void KeyImageNoteManagerWidget::deleteSelectedItemsFromCurrentSelection()
{
    QString removedItem = m_thumbnailImageDisplayer->removeSelectedItems();
    m_keyImageNoteManager->removeItemOfCurrentSelection(removedItem);
}
}
