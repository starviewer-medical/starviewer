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

#ifndef UDGINPUTOUTPUTSETTINGS_H
#define UDGINPUTOUTPUTSETTINGS_H

#include "defaultsettings.h"

namespace udg {

class InputOutputSettings : public DefaultSettings {
public:
    InputOutputSettings();
    ~InputOutputSettings();

    void init();

    /// Declaració de claus
    /// Path absolut de l'arxiu de base dades
    static const QString DatabaseAbsoluteFilePath;
    /// Path del directori de la cache
    static const QString CachePath;
    /// Polítiques d'autogestió de cache
    static const QString DeleteLeastRecentlyUsedStudiesInDaysCriteria;
    static const QString DeleteLeastRecentlyUsedStudiesNoFreeSpaceCriteria;
    static const QString MinimumGigaBytesToFreeIfCacheIsFull;
    static const QString MinimumFreeGigaBytesForCache;
    static const QString MinimumDaysUnusedToDeleteStudy;
    /// Controlar quin estudi està baixant-se
    static const QString RetrievingStudy;   // previous key, used until Starviewer 1.1 to hold only one Study UID; should be deleted some day in the future
    static const QString StudiesBeingRetrieved; // current key, used to hold several Study UIDs

    /// Paràmetres del RIS
    static const QString RISRequestsPort;
    static const QString ListenToRISRequests;
    static const QString RISRequestViewOnceRetrieved;

    /// Últim path des del que hem obert un dicomdir
    static const QString LastOpenedDICOMDIRPath;
    /// Geometria de les columnes del llistat d'estudis d'un DICOMDIR/llista d'operacions/PACS
    static const QString DICOMDIRStudyListColumnsWidth;
    static const QString CreateDICOMDIRStudyListColumnsWidth;
    static const QString CreateDICOMDIRGeometry;
    static const QString OperationStateColumnsWidth;
    static const QString PACSStudyListColumnsWidth;
    static const QString PacsListColumnWidths;
    static const QString ConfigurationScreenPACSList;
    static const QString LocalDatabaseStudyList;
    static const QString LocalDatabaseSplitterState;
    static const QString QueryScreenGeometry;
    static const QString QueryScreenPACSListIsVisible;
    static const QString PACSStudyListSortByColumn;
    static const QString PACSStudyListSortOrder;
    static const QString LocalDatabaseStudyListSortByColumn;
    static const QString LocalDatabaseStudyListSortOrder;
    static const QString DICOMDIRStudyListSortByColumn;
    static const QString DICOMDIRStudyListSortOrder;

    /// Paràmetres de connexió amb un PACS
    static const QString LocalAETitle;
    static const QString IncomingDICOMConnectionsPort;
    static const QString PACSConnectionTimeout;
    static const QString MaximumPACSConnections;

    /// Llista de PACS
    static const QString PacsListConfigurationSectionName;

    /// Paràmetres de la configuració DICOMDIR
    /// Path del programa de gravació
    static const QString DICOMDIRBurningApplicationPathKey;
    /// Parameters del programa de gravació
    static const QString DICOMDIRBurningApplicationParametersKey;
    /// Paràmetres de gravació d'un CD quan són diferents amb els de DVD
    static const QString DICOMDIRBurningApplicationCDParametersKey;
    /// Paràmetres de gravació d'un DVD quan són diferents amb els de CD
    static const QString DICOMDIRBurningApplicationDVDParametersKey;
    /// CheckBox per saber si cal tenir o no en compte els paràmetres de gravació d'un CD o DVD
    static const QString DICOMDIRBurningApplicationHasDifferentCDDVDParametersKey;
    /// CheckBox per saber si cal convertir les imatges del DICOMDIR a little endian
    static const QString ConvertDICOMDIRImagesToLittleEndianKey;
    /// Path del directori del que s'ha de copiar el visor DICOMDIR
    static const QString DICOMDIRFolderPathToCopy;
    /// Checkbox per saber si cal copiar el contingut del directori que apunta el setting DICOMDIRFolderPathToCopy als CD/DVD DICOMDIR
    static const QString CopyFolderContentToDICOMDIROnCDOrDVD;
    /// Checkbox per saber si cal copiar el contingut del directori que apunta el setting DICOMDIRFolderPathToCopy als Usb/HardDisk DICOMDIR
    static const QString CopyFolderContentToDICOMDIROnUSBOrHardDisk;

    // Informació Institució
    static const QString InstitutionName;
    static const QString InstitutionTown;
    static const QString InstitutionAddress;
    static const QString InstitutionZipCode;
    static const QString InstitutionCountry;
    static const QString InstitutionPhoneNumber;
    static const QString InstitutionEmail;

    // Columna i ordre d'ordenació de la QOperationStateScreen
    static const QString OperationStateListSortByColumn;
    static const QString OperationStateListSortOrder;

    // Boolea per saber si s'ha de cercar previes a partir del nom del pacient.
    static const QString SearchRelatedStudiesByName;
};

} // end namespace udg

#endif
