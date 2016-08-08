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

#ifndef NO_CRASH_REPORTER

#include "crashhandler.h"
#include "executablesnames.h"
#include "../core/starviewerapplication.h"

#include <QString>
#include <QDir>
#include <QCoreApplication>

#ifdef Q_OS_LINUX

#include <client/linux/handler/exception_handler.h>

static bool launchCrashReporter(const google_breakpad::MinidumpDescriptor &minidumpDescriptor, void *crashHandler, bool succeeded)
{
    // DON'T USE THE HEAP!!!
    // So crashHandler indeed means, no QStrings, no qDebug(), no QAnything, seriously!

    if (!succeeded)
    {
        return false;
    }

    pid_t pid = fork();

    if (pid == -1)
    {
        // Fork failed
        return false;
    }
    if (pid == 0)
    {
        std::string path(minidumpDescriptor.path());
        // substring starting after "directory/" of length = total - dir.length - 5 ("/" + ".dmp")
        std::string minidumpId = path.substr(minidumpDescriptor.directory().length() + 1, path.length() - minidumpDescriptor.directory().length() - 5);

        // We are the fork
        execl(static_cast<CrashHandler*>(crashHandler)->getCrashReporterPath(),
              static_cast<CrashHandler*>(crashHandler)->getCrashReporterPath(),
              minidumpDescriptor.directory().c_str(),
              minidumpId.c_str(),
              (char *) 0);
        // execl replaces this process, so no more code will be executed
        // unless it failed. If it failed, then we should return false.
        return false;
    }

    // We called fork()
    return true;
}

#elif defined Q_OS_MAC

#include "client/mac/handler/exception_handler.h"

#include <unistd.h>
#include <string>
#include <iostream>

static bool launchCrashReporter(const char *dumpDirPath, const char *minidumpId, void *crashHandler, bool succeeded)
{
    // DON'T USE THE HEAP!!!
    // So crashHandler indeed means, no QStrings, no qDebug(), no QAnything, seriously!

    if (!succeeded)
    {
        return false;
    }

    pid_t pid = fork();

    if (pid == -1)
    {
        // Fork failed
        return false;
    }
    if (pid == 0)
    {
        // We are the fork
        execl(static_cast<CrashHandler*>(crashHandler)->getCrashReporterPath(),
              static_cast<CrashHandler*>(crashHandler)->getCrashReporterPath(),
              dumpDirPath,
              minidumpId,
              (char *) 0);
        // execl replaces this process, so no more code will be executed
        // unless it failed. If it failed, then we should return false.
        return false;
    }

    // We called fork()
    return true;
}

#elif defined Q_OS_WIN32

#include "client/windows/handler/exception_handler.h"

static bool launchCrashReporter(const wchar_t *dumpDirPath, const wchar_t *minidumpId, void *crashHandler, EXCEPTION_POINTERS *exinfo,
                                MDRawAssertionInfo *assertion, bool succeeded)
{
    if (!succeeded)
    {
        return false;
    }

    // DON'T USE THE HEAP!!!
    // So crashHandler indeed means, no QStrings, no qDebug(), no QAnything, seriously!

    const char *crashReporterPath = static_cast<CrashHandler*>(crashHandler)->getCrashReporterPath();

    // Convert crashReporterPath to widechars, which sadly means the product name must be Latin1
    wchar_t crashReporterPathWchar[256];
    char *out = (char *)crashReporterPathWchar;
    const char *in = crashReporterPath - 1;
    do
    {
        // Latin1 chars fit in first byte of each wchar
        *out++ = *++in;
        // Every second byte is NULL
        *out++ = '\0';
    }
    while (*in);

    wchar_t command[MAX_PATH * 3 + 6];
    wcscpy_s(command, crashReporterPathWchar);
    wcscat_s(command, L" \"");
    wcscat_s(command, dumpDirPath);
    wcscat_s(command, L"\" \"");
    wcscat_s(command, minidumpId);
    wcscat_s(command, L"\"");

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;
    ZeroMemory(&pi, sizeof(pi));

    if (CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        TerminateProcess(GetCurrentProcess(), 1);
    }

    return false;
}

#endif // Q_OS_*

CrashHandler::CrashHandler()
{
#ifndef NO_CRASH_REPORTER
    // Primer comprovem que existeixi el directori ~/.starviewer/dumps/ on guradarem els dumps
    QDir dumpsDir = udg::UserDataRootPath + "dumps/";
    if (!dumpsDir.exists())
    {
        // Creem el directori
        dumpsDir.mkpath(dumpsDir.absolutePath());
    }

    QString crashReporterPath = QCoreApplication::applicationDirPath() + "/" + STARVIEWER_CRASH_REPORTER_EXE;

    crashReporterPathByteArray = crashReporterPath.toLocal8Bit();
    const char *crashReporterPathCString = crashReporterPathByteArray.constData();

    this->setCrashReporterPath(crashReporterPathCString);

#ifdef Q_OS_LINUX
    exceptionHandler = new google_breakpad::ExceptionHandler(google_breakpad::MinidumpDescriptor(dumpsDir.absolutePath().toStdString()), nullptr,
                                                             launchCrashReporter, this, true, -1);
#elif defined Q_OS_MAC
    exceptionHandler = new google_breakpad::ExceptionHandler(dumpsDir.absolutePath().toStdString(), 0, launchCrashReporter, this, true, nullptr);
#elif defined Q_OS_WIN32
    exceptionHandler = new google_breakpad::ExceptionHandler(dumpsDir.absolutePath().toStdWString(), 0, launchCrashReporter, this,
                                                             google_breakpad::ExceptionHandler::HANDLER_ALL);
#endif // Q_OS_*

#endif // NO_CRASH_REPORTER
}

void CrashHandler::setCrashReporterPath(const char *path)
{
    m_crashReporterPath = path;
}

const char* CrashHandler::getCrashReporterPath() const
{
    return m_crashReporterPath;
}

#endif // NO_CRASH_REPORTER
