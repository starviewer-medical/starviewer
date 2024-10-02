/*************************************************************************************
  Copyright (C) 2024 Laboratori de Gràfics i Imatge, Universitat de Girona &
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

#ifndef UTILS_H
#define UTILS_H

#include <QString>

template <typename T> class QVector;

namespace udg {

/// Collection of random utility functions that may be used from anywhere.
namespace Utils {

/// Finds the common parts between \a string1 and \a string2 at the beginning and the end of both and returns the found pattern where the variating part in the
/// middle is replaced by the \a patternString. Example: given "I'll be back" and "I'll go back" it returns "I'll * back".
QString findCommonPattern(const QString &string1, const QString &string2, const QString &patternString = "*");

/// Compares \a vector1 and \a vector2 numerically element by element. The first different element decides the result. If one of the vectors is a "prefix" of
/// the other, the shorter one is considered smaller, e.g. [1, 2, 3] is considered smaller than [1, 2, 3, 4].
/// \return negative (<0) if \a vector1 is smaller than \a vector2, 0 if they are equal, and positive (>0) if \a vector1 is bigger than \a vector2.
int compareUintVectors(const QVector<uint> &vector1, const QVector<uint> &vector2);

/// Compares \a uid1 and \a uid2 numerically (decomposing the UID in segments and comparing each segment numerically). If one of the UIDs is a "prefix" of the
/// other, the shorter one is considered smaller, e.g. "1.2.3" is considered smaller than "1.2.3.4".
/// \return negative (<0) if \a uid1 is smaller than \a uid2, 0 if they are equal, and positive (>0) if \a uid1 is bigger than \a uid2.
int compareUids(const QString &uid1, const QString &uid2);

/// Generates and returns a new UID with the help of DCMTK.
QString generateUid(const QString &prefix = QString());

}

}

#endif // UTILS_H
