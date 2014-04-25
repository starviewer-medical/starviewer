#ifndef UDGSYNCACTIONSCONFIGURATIONHANDLER_H
#define UDGSYNCACTIONSCONFIGURATIONHANDLER_H

#include <QList>

class QString;

namespace udg {

class SyncActionsConfiguration;
class SyncActionMetaData;

/**
    Class to handle sync actions configurations. It lets save to and retrieve from settings, giving a specific context,
    then several configurations can be handled for diferent contexts if needed.
 */
class SyncActionsConfigurationHandler {
public:
    SyncActionsConfigurationHandler();
    ~SyncActionsConfigurationHandler();

    /// Gets the configuration corresponding to the given settings context. If context does not exists,
    /// the resulting configuration will be the same as getDefaultSyncActionsConfiguration()
    SyncActionsConfiguration* getConfiguration(const QString &context);

    /// Saves the given configuration in settings under the given named context
    void saveConfiguration(SyncActionsConfiguration *configuration, const QString &context);
    
    /// Gets the default sync actions configuration, which corresponds to all the registered sync actions
    static SyncActionsConfiguration* getDefaultSyncActionsConfiguration();

private:
    /// Gets the right formatted setting name
    QString getFormattedSettingName(const QString &context);

    /// Gets the list of registered sync actions
    static QList<SyncActionMetaData> getRegisteredSyncActions();
};

} // End namespace udg

#endif
