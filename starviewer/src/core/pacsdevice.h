/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef PACSDEVICE
#define PACSDEVICE

#include <QString>

namespace udg {

/**
 * @brief The PacsDevice class stores data about a PACS server.
 *
 * This data includes connexion data (AE Title, address, query/retrieve port and store port) and other descriptive data (institution, location and description).
 */
class PacsDevice {
public:
    /// Creates an invalid or empty PACS.
    PacsDevice();

    /// Returns the internal id of this PACS, assigned by the application.
    const QString& getID() const;
    /// Sets the internal id of this PACS, assigned by the application.
    void setID(QString id);

    /// Returns the AE title.
    const QString& getAETitle() const;
    /// Sets the AE title.
    void setAETitle(QString aeTitle);

    /// Returns the address (IP or hostname).
    const QString& getAddress() const;
    /// Sets the address (IP or hostname).
    void setAddress(QString address);

    /// Returns true if this PACS has the query/retrieve service enabled and false otherwise.
    bool isQueryRetrieveServiceEnabled() const;
    /// Sets whether this PACS has the query/retrieve service enabled or not.
    void setQueryRetrieveServiceEnabled(bool enabled);

    /// Returns the port for the query/retrieve service.
    int getQueryRetrieveServicePort() const;
    /// Sets the port for the query/retrieve service.
    void setQueryRetrieveServicePort(int port);

    /// Returns true if this PACS has the store service enabled and false otherwise.
    bool isStoreServiceEnabled() const;
    /// Sets whether this PACS has the store service enabled or not.
    void setStoreServiceEnabled(bool enabled);

    /// Returns the port for the store service.
    int getStoreServicePort() const;
    /// Sets the port for the store service.
    void setStoreServicePort(int port);

    /// Returns the institution that owns or manages this PACS.
    const QString& getInstitution() const;
    /// Sets the institution that owns or manages this PACS.
    void setInstitution(QString institution);

    /// Returns the location of this PACS.
    const QString& getLocation() const;
    /// Sets the location of this PACS.
    void setLocation(QString location);

    /// Returns a description for this PACS.
    const QString& getDescription() const;
    /// Sets a description for this PACS.
    void setDescription(QString description);

    /// Returns true if this PACS is marked as default for queries and false otherwise.
    bool isDefault() const;
    /// Sets whether this PACS is marked as default for queries.
    void setDefault(bool isDefault);

    /// Returns true if this PACS is empty (default constructed) and false otherwise.
    bool isEmpty() const;

    /// Returns true if this PACS represents the same as the given one, i.e. same AE Title, address and Q/R port, and false otherwise.
    bool isSamePacsDevice(const PacsDevice &pacsDevice) const;

    /// Returns true if this PacsDevice instance is exactly equal to the given one.
    bool operator==(const PacsDevice &pacsDevice) const;

private:
    /// Returns the key name to be used in the default PACS list.
    QString getKeyName() const;

    /// Returns a list of key names for all the default PACS.
    QStringList getDefaultPACSKeyNamesList() const;

private:
    /// Internal id of this PACS, assigned by the application.
    QString m_id;
    /// AE Title.
    QString m_AETitle;
    /// Address (IP or hostname).
    QString m_address;
    /// Whether the query/retrieve service is enabled or not.
    bool m_isQueryRetrieveServiceEnabled;
    /// Port for the query/retrieve service.
    int m_queryRetrieveServicePort;
    /// Whether the store service is enabled or not.
    bool m_isStoreServiceEnabled;
    /// Port for the store service.
    int m_storeServicePort;
    /// Institution that owns or manages this PACS.
    QString m_institution;
    /// Location of this PACS.
    QString m_location;
    /// A description for this PACS.
    QString m_description;
};

}
#endif
