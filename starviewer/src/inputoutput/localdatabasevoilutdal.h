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

#ifndef UDG_LOCALDATABASEVOILUTDAL_H
#define UDG_LOCALDATABASEVOILUTDAL_H

#include "localdatabasebasedal.h"

#include <QList>

namespace udg {

class DicomMask;
class Image;
class VoiLut;

/**
 * @brief The LocalDatabaseVoiLutDAL class is a database access layer for VoiLut.
 */
class LocalDatabaseVoiLutDAL : public LocalDatabaseBaseDAL
{
public:
    LocalDatabaseVoiLutDAL(DatabaseConnection *dbConnection);

    /// Inserts a new VoiLut from the given Image in the database.
    bool insert(const VoiLut &voiLut, Image *image);

    /// Deletes all VoiLuts in the database that match the given mask.
    bool del(const DicomMask &mask);

    /// Returns all VoiLuts in the database that match the given mask.
    QList<VoiLut> query(const DicomMask &mask);

};

} // namespace udg

#endif // UDG_LOCALDATABASEVOILUTDAL_H
