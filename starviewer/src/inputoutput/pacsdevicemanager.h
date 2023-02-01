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

#ifndef UDGPACSDEVICEMANAGER_H
#define UDGPACSDEVICEMANAGER_H

#include "settings.h"

#include <QList>

namespace udg {

class PacsDevice;

/**
 * @brief The PacsDeviceManager class allows to read and write PacsDevice objects from and to settings.
 */
class PacsDeviceManager {

public:
    /// Flags to filter PACS by different properties.
    enum PacsFilterFlag {
        CanRetrieve = 0x1,
        CanStore    = 0x2,
        All         = CanRetrieve | CanStore,
        OnlyDefault = 0x4
    };
    Q_DECLARE_FLAGS(PacsFilter, PacsFilterFlag)

    /// Saves the given PACS to settings as a new PACS and an ID is assigned to it.
    /// \param[in,out] pacs PacsDevice instance with the PACS data.
    /// \return \c false if an equivalent PACS is already saved, \c true otherwise.
    static bool addPacs(PacsDevice &pacs);

    /// Updates the given PACS in settings. If a PACS with the ID of the given one does not exist the method does nothing.
    /// \param[in,out] pacs PacsDevice instance with the updated PACS data.
    /// \warning The ID of the given PACS and other ones may change.
    static void updatePacs(PacsDevice &pacs);

    /// Deletes the PACS with the given ID from settings. If a PACS with the ID of the given one does not exist the method does nothing.
    /// \param[in] pacsID ID of the PACS that must be deleted.
    /// \warning The ID of other PACS will probably change.
    static void deletePacs(const QString &pacsID);

    /// Returns a list of PACS stored in settings, filtered by the given flags.
    /// \param[in] filter Combination of flags to filter the returned PACS.
    /// \return Filtered list of PACS.
    static QList<PacsDevice> getPacsList(PacsFilter filter = All);

    /// Retrieves and returns the PACS device stored with the given ID in settings.
    /// \param[in] pacsID ID of the PACS that must be returned.
    /// \return PacsDevice instance read from settings with the given ID. Empty PacsDevice if not found.
    static PacsDevice getPacsDeviceById(const QString &pacsID);

    /// Returns the given list without duplicate PACS.
    /// \param[in] pacsDeviceList List with possibly duplicate PACS.
    /// \return The same list with duplicate PACS removed.
    static QList<PacsDevice> removeDuplicatePacsFromList(const QList<PacsDevice> &pacsDeviceList);

    //TODO: Aquest codi està duplicat a DICOMSource, però com DICOMSource està al core no pot utilitzar aquest mètode, sinó tindríem dependència ciclica
    /// Returns true if an equivalent PACS to the given one is contained in the given list, and false otherwise.
    static bool isAddedSamePacsDeviceInList(const QList<PacsDevice> &pacsDeviceList, const PacsDevice &pacsDevice);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PacsDeviceManager::PacsFilter)

}

#endif
