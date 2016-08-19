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

#include "starviewerapplication.h"

#include <QApplication>

namespace udg {

QString installationPath()
{
    return qApp->applicationDirPath();
}

// Explanation: This has to point to the starviewer directory knowing the directory containing the executable (qApp->applicationDirPath()).
//              The shadow build directory is always at the same level than the starviewer directory.
// Windows and Linux  no shadow build app dir path = starviewer/bin
//                       shadow build                build-starviewer.../bin
// Mac OS X           no shadow build                starviewer/bin/starviewer.app/Contents/MacOS
//                       shadow build                build-starviewer.../bin/starviewer.app/Contents/MacOS
// Mac crash reporter no shadow build                starviewer/bin/starviewer.app/Contents/MacOS/starviewer_crashreporter.app/Contents/MacOS
//                       shadow build                build-starviewer.../bin/starviewer.app/Contents/MacOS/starviewer_crashreporter.app/Contents/MacOS
// All autotests      no shadow build app dir path = starviewer/tests/auto
//                       shadow build                build-starviewer.../tests/auto
QString sourcePath()
{
    QString sourceDirPath;

    if (qApp->applicationFilePath().contains("autotests"))
    {
        sourceDirPath = qApp->applicationDirPath() + "/../..";
    }
    else
    {
#ifdef Q_OS_OSX
        sourceDirPath = qApp->applicationFilePath().endsWith("starviewer") ?
                    qApp->applicationDirPath() + "/../../../.." :           // starviewer
                    qApp->applicationDirPath() + "/../../../../../../..";   // starviewer_crashreporter
#else
        sourceDirPath = qApp->applicationDirPath() + "/..";
#endif
    }

    if (QDir(sourceDirPath).dirName() != "starviewer")
    {
        // Assume shadow build
        sourceDirPath += "/../starviewer";
    }

    return sourceDirPath;
}

}
