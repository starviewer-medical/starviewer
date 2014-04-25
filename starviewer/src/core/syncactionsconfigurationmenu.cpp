#include "syncactionsconfigurationmenu.h"

#include "signaltosyncactionmapperfactory.h"
#include "syncactionsconfiguration.h"
#include "syncactionmetadata.h"

namespace udg {

SyncActionsConfigurationMenu::SyncActionsConfigurationMenu(SyncActionsConfiguration *config, QWidget *parent)
 : QMenu(parent)
{
    Q_ASSERT(config);
    
    m_syncActionsConfig = config;
    m_signalMapper = new QSignalMapper(this);
    
    foreach (const SyncActionMetaData &syncActionMetaData, SignalToSyncActionMapperFactory::instance()->getFactoryIdentifiersList())
    {
        // First we get the corresponding menu's QAction to the sync action
        QAction *enableSyncAction = getQAction(syncActionMetaData);

        // Adding the configured QAction to the menu
        addAction(enableSyncAction);
        
        // Then we configure the mapping of the sync actions and the corresponding QActions to enable/disable them when toggled
        m_syncActionsMap.insert(syncActionMetaData, enableSyncAction);
        connect(enableSyncAction, SIGNAL(toggled(bool)), m_signalMapper, SLOT(map()));
        m_signalMapper->setMapping(enableSyncAction, syncActionMetaData.getName());
    }

    // Finally, we configure the signal mapper
    connect(m_signalMapper, SIGNAL(mapped(const QString&)), SLOT(toggled(const QString&)));
}

SyncActionsConfigurationMenu::~SyncActionsConfigurationMenu()
{
}

QAction* SyncActionsConfigurationMenu::getQAction(const SyncActionMetaData &syncActionMetaData)
{
    QAction *qaction = new QAction(this);
    qaction->setCheckable(true);
    qaction->setText(syncActionMetaData.getUserInterfaceName());
    qaction->setChecked(m_syncActionsConfig->isSyncActionEnabled(syncActionMetaData));

    return qaction;
}

void SyncActionsConfigurationMenu::toggled(const QString &name)
{
    QMapIterator<SyncActionMetaData, QAction*> iterator(m_syncActionsMap);
    
    while (iterator.hasNext())
    {
        iterator.next();
        SyncActionMetaData currentSyncAction = iterator.key();
        if (currentSyncAction.getName() == name)
        {
            QAction *menuAction = iterator.value();
            m_syncActionsConfig->enableSyncAction(iterator.key(), menuAction->isChecked());
            // Stop searching
            iterator.toBack();
        }
    }
}

} // End namespace udg
