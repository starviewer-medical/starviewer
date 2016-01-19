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

#ifndef UDGLOCALDATABASEDISPLAYSHUTTERDAL_H
#define UDGLOCALDATABASEDISPLAYSHUTTERDAL_H

#include "localdatabasebasedal.h"

namespace udg {

class DicomMask;
class DisplayShutter;
class Image;

/**
 * @brief The LocalDatabaseDisplayShutterDAL class is the Data Access Layer class for display shutters.
 */
class LocalDatabaseDisplayShutterDAL : public LocalDatabaseBaseDAL {

public:
    LocalDatabaseDisplayShutterDAL(DatabaseConnection &databaseConnection);

    /// Inserts to the database the given display shutter that corresponds to the given image. Returns true if successful and false otherwise.
    bool insert(const DisplayShutter &shutter, const Image *shuttersImage);

    /// Updates in the database the display shutters that belong to the given image, which are the ones given in the list.
    /// The existing shutters are deleted and the given ones are inserted. Returns true if successful and false otherwise.
    bool update(const QList<DisplayShutter> &shuttersList, const Image *shuttersImage);

    /// Deletes from the database the display shutters that match the given mask. Returns true if successful and false otherwise.
    bool del(const DicomMask &mask);

    /// Retrieves from the database the display shutters that match the given mask and returns them in a list.
    QList<DisplayShutter> query(const DicomMask &mask);

};

} // End namespace udg

#endif
