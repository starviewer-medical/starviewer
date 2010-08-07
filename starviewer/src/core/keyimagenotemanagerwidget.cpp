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
    m_splitter->setSizes(QList<int>() << 0 << 1);
    m_createKeyImageNotePushButton->setEnabled(false);
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
    connect(m_keyImageNoteManager, SIGNAL(keyImageNoteOfPatientAdded(KeyImageNote*)), SLOT(updateKeyImageNoteDisplayers(KeyImageNote*)));
    connect(m_thumbnailImageDisplayer, SIGNAL(show(const QString &)), m_keyImageNoteManager, SLOT(changeCurrentDisplayedImage(const QString &)));
    connect(m_keyImageNoteManager, SIGNAL(imageAddedToTheCurrentSelectionOfImages(Image*)), SLOT(updateCreateKeyImageNotePushButtonStatus()));
    connect(m_keyImageNoteManager, SIGNAL(keyImageNoteOfPatientAdded(KeyImageNote*)), SLOT(toogleMySelectionWidget()));
}

void KeyImageNoteManagerWidget::createConnections()
{
    connect(m_createKeyImageNotePushButton, SIGNAL(clicked()), SLOT(showKeyImageNoteCreatorWidget()));
    connect(m_newKeyImageNoteButton, SIGNAL(clicked()), SLOT(toogleMySelectionWidget()));
}

void KeyImageNoteManagerWidget::generateKeyImageNoteDisplayers()
{
    foreach (KeyImageNote *keyImageNote, m_keyImageNoteManager->getKeyImageNotesOfPatient())
    {
        KeyImageNoteDisplayer *keyImageNoteDisplayer = createKeyImageNoteDisplayer(keyImageNote);
        m_keyImageNoteDisplayers.append(keyImageNoteDisplayer);
        m_layout->addWidget(keyImageNoteDisplayer);
    }

    m_isKeyImageNoteManagerDataLoaded = true;
}

KeyImageNoteDisplayer* KeyImageNoteManagerWidget::createKeyImageNoteDisplayer(KeyImageNote *keyImageNote)
{
    KeyImageNoteDisplayer *keyImageNoteDisplayer = new KeyImageNoteDisplayer();
    keyImageNoteDisplayer->setKeyImageNote(keyImageNote);
    keyImageNoteDisplayer->setKeyImageNoteManager(m_keyImageNoteManager);

    return keyImageNoteDisplayer;
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
    updateCreateKeyImageNotePushButtonStatus();
}

void KeyImageNoteManagerWidget::updateKeyImageNoteDisplayers(KeyImageNote *keyImageNote)
{
    KeyImageNoteDisplayer *keyImageNoteDisplayer = createKeyImageNoteDisplayer(keyImageNote);
    m_layout->insertWidget(0, keyImageNoteDisplayer);
    m_keyImageNoteDisplayers.append(keyImageNoteDisplayer);
}

void KeyImageNoteManagerWidget::toogleMySelectionWidget()
{
    if (m_splitter->sizes()[0] == 0)
    {
        m_splitter->setSizes(QList<int>() << 1 << 1 );
    }
    else
    {
        m_splitter->setSizes(QList<int>() << 0 << 1);
    }
}

void KeyImageNoteManagerWidget::updateCreateKeyImageNotePushButtonStatus()
{
    m_createKeyImageNotePushButton->setEnabled(m_keyImageNoteManager->getNumberOfImagesInCurrentSelection() > 0);
}
}
