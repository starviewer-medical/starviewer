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

#ifndef DICOMENTITYFLAGS_H
#define DICOMENTITYFLAGS_H

#include <QFlags>

namespace udg {

/**
 * @brief The DicomEntityFlag struct acts as an envelop an enum to represent flags corresponding to different types of DICOM entities.
 *
 * This flags are used by other classes to declare the supported DICOM entities.
 */
struct DicomEntity
{
    enum Flag { Image = 0x1, EncapsulatedDocument = 0x2 };
};

Q_DECLARE_FLAGS(DicomEntityFlags, DicomEntity::Flag)
Q_DECLARE_OPERATORS_FOR_FLAGS(DicomEntityFlags)

}

#endif // DICOMENTITYFLAGS_H
