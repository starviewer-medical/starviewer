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

namespace udg {

class DicomMask;
class Image;
class VoiLut;

/**
 * @brief The LocalDatabaseVoiLutDAL class is the Data Access Layer class for VOI LUTs.
 */
class LocalDatabaseVoiLutDAL : public LocalDatabaseBaseDAL
{
public:
    LocalDatabaseVoiLutDAL(DatabaseConnection &databaseConnection);

    /// Inserts to the database the given VOI LUT from the given image. Returns true if successful and false otherwise.
    bool insert(const VoiLut &voiLut, const Image *image);

    /// Deletes from the database the VOI LUTs that match the given mask. Returns true if successful and false otherwise.
    bool del(const DicomMask &mask);

    /// Retrieves from the database the VOI LUTs that match the given mask and returns them in a list.
    QList<VoiLut> query(const DicomMask &mask);

};

} // namespace udg

#endif // UDG_LOCALDATABASEVOILUTDAL_H
