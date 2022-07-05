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
#include <QUrl>

namespace udg {

/**
 * @brief The PacsDevice class stores data about a PACS server.
 *
 * This data includes connexion data and other descriptive data (institution, location and description).
 *
 * There are three types of PACS according to the protocols they use to search, download and upload studies:
 * - DIMSE: C-FIND + C-MOVE + C-STORE
 * - WADO: QIDO-RS + WADO-RS + STOW-RS
 * - Hybrid WADO-URI + DIMSE: C-FIND + WADO-URI + C-STORE
 */
class PacsDevice {
public:
    /// Type of PACS.
    enum class Type { Dimse, Wado, WadoUriDimse };

    /// Creates an invalid or empty PACS.
    PacsDevice();

    /// Returns the internal id of this PACS, assigned by the application.
    const QString& getID() const;
    /// Sets the internal id of this PACS, assigned by the application.
    void setID(QString id);

    /// Returns the type of this PACS.
    Type getType() const;
    /// Sets the type of this PACS.
    void setType(Type type);

    /// Returns the AE title. Not applicable to WADO PACS.
    const QString& getAETitle() const;
    /// Sets the AE title. Not applicable to WADO PACS.
    void setAETitle(QString aeTitle);

    /// Returns the address (IP or hostname). Not applicable to WADO PACS.
    const QString& getAddress() const;
    /// Sets the address (IP or hostname). Not applicable to WADO PACS.
    void setAddress(QString address);

    /// Returns true if this PACS has the query/retrieve service enabled and false otherwise. Not applicable to WADO PACS.
    bool isQueryRetrieveServiceEnabled() const;
    /// Sets whether this PACS has the query/retrieve service enabled or not. Not applicable to WADO PACS.
    void setQueryRetrieveServiceEnabled(bool enabled);

    /// Returns the port for the query/retrieve service. Not applicable to WADO PACS.
    int getQueryRetrieveServicePort() const;
    /// Sets the port for the query/retrieve service. Not applicable to WADO PACS.
    void setQueryRetrieveServicePort(int port);

    /// Returns true if this PACS has the store service enabled and false otherwise. Not applicable to WADO PACS.
    bool isStoreServiceEnabled() const;
    /// Sets whether this PACS has the store service enabled or not. Not applicable to WADO PACS.
    void setStoreServiceEnabled(bool enabled);

    /// Returns the port for the store service. Not applicable to WADO PACS.
    int getStoreServicePort() const;
    /// Sets the port for the store service. Not applicable to WADO PACS.
    void setStoreServicePort(int port);

    /// Returns the base URI. Not applicable to DIMSE PACS.
    const QUrl& getBaseUri() const;
    /// Sets the base URI. Not applicable to DIMSE PACS.
    void setBaseUri(QUrl baseUri);

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

    /// Returns true if this PACS represents the same as the given one and false otherwise, comparing only type and query and download connection settings.
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
    /// Type of this PACS.
    Type m_type;
    /// AE Title. Only applicable to DIMSE and hybrid PACS.
    QString m_AETitle;
    /// Address (IP or hostname). Only applicable to DIMSE and hybrid PACS.
    QString m_address;
    /// Whether the query/retrieve service is enabled or not. Only applicable to DIMSE and hybrid PACS.
    bool m_isQueryRetrieveServiceEnabled;
    /// Port for the query/retrieve service. Only applicable to DIMSE and hybrid PACS.
    int m_queryRetrieveServicePort;
    /// Whether the store service is enabled or not. Only applicable to DIMSE and hybrid PACS.
    bool m_isStoreServiceEnabled;
    /// Port for the store service. Only applicable to DIMSE and hybrid PACS.
    int m_storeServicePort;
    /// Base URI. Only applicable to WADO and hybrid PACS.
    QUrl m_baseUri;
    /// Institution that owns or manages this PACS.
    QString m_institution;
    /// Location of this PACS.
    QString m_location;
    /// A description for this PACS.
    QString m_description;
};

}
#endif
