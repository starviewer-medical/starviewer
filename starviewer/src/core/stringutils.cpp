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

#include "stringutils.h"

namespace udg {

namespace StringUtils {

QString findCommonPattern(const QString& string1, const QString& string2, const QString &patternString)
{
    int length1 = string1.length();
    int length2 = string2.length();
    int minLength = std::min(length1, length2);

    // Find common part at the beginning
    int start = 0;

    while (start < minLength && string1[start] == string2[start])
    {
        ++start;
    }

    // Find common part at the end
    int end1 = length1 - 1;
    int end2 = length2 - 1;

    while (end1 >= start && end2 >= start && string1[end1] == string2[end2])
    {
        --end1;
        --end2;
    }

    // Create new string with the common pattern
    QString commonPattern = string1.left(start);

    if (start <= end1 || end1 != end2)
    {
        commonPattern += patternString;
    }

    commonPattern += string1.right(length1 - end1 - 1);

    return commonPattern;
}

}

}
