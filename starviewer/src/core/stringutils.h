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

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <QString>

namespace udg {

namespace StringUtils {

/// Finds the common parts between \a string1 and \a string2 at the beginning and the end of both and returns the found pattern where the variating part in the
/// middle is replaced by the \a patternString. Example: given "I'll be back" and "I'll go back" it returns "I'll * back".
QString findCommonPattern(const QString &string1, const QString &string2, const QString &patternString = "*");

}

}

#endif // STRINGUTILS_H
