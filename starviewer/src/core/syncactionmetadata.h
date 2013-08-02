#ifndef UDGSYNCACTIONMETADATA_H
#define UDGSYNCACTIONMETADATA_H

#include <QString>

namespace udg {

/**
    Class to hold the meta-data for a SyncAction
    The data will be provided in the constructor
 */
class SyncActionMetaData {
public:
    SyncActionMetaData();
    /// Constructor by values
    /// @param name The name of the associated SyncAction
    /// @param interfaceName Translatable string associated with the SyncAction that will appear on the interface, i.e. menus
    /// @param settingsName String with the name that should be used at settings level to refer the associated SyncAction
    SyncActionMetaData(const QString &name, const QString &interfaceName, const QString &settingsName);
    ~SyncActionMetaData();

    /// Methods to return the name, interface name and settings name of the associated SyncAction
    QString getName() const;
    QString getUserInterfaceName() const;
    QString getSettingsName() const;

    /// Will return true if all properties are empty, false otherwise
    bool isEmpty() const;

    /// Operator less-than. Compares < the name attributes. Needed to use this class in QMap.
    bool operator <(const SyncActionMetaData &syncActionMetaData) const;

    /// Operator equality. Compares all members are the same
    bool operator ==(const SyncActionMetaData &syncActionMetaData) const;

private:
    /// Name of the associated SyncAction
    QString m_name;
    
    /// Translatable string associated with the SyncAction that will appear on the interface, i.e. menus
    QString m_userInterfaceName;

    /// String with the name that should be used at settings level to refer the associated SyncAction
    QString m_settingsName;
};

} // End namespace udg

#endif
