/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "harddiskinformation.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/statvfs.h> // per statvfs
#endif // _WIN32

namespace udg{

HardDiskInformation::HardDiskInformation()
{
}


HardDiskInformation::~HardDiskInformation()
{
}

unsigned long HardDiskInformation::getTotalNumberOfBytes(QString path)
{
    return static_cast<unsigned long>( getTotalBytesPlataformEspecific(path) );
}

unsigned long HardDiskInformation::getNumberOfFreeBytes(QString path)
{
    return static_cast<unsigned long>( getFreeBytesPlataformEspecific(path) );
}

unsigned long HardDiskInformation::getTotalNumberOfMBytes(QString path)
{
    return static_cast<unsigned long>( getTotalBytesPlataformEspecific(path) / 1024 );
}

unsigned long HardDiskInformation::getNumberOfFreeMBytes(QString path)
{
    return static_cast<unsigned long>( getFreeBytesPlataformEspecific(path) / 1024);
}

quint64 HardDiskInformation::getTotalBytesPlataformEspecific(QString path)
{
    quint64 total;
    bool existsError = true;

#ifdef _WIN32
#error Mètode no comprovat en win32. Cal descomentar i comprovar-ho.
    /*
    ULARGE_INTEGER totalBytes;
    
    if ( GetDiskFreeSpaceExA(path.toAscii(), null, &totalBytes, null) )
    {
        total = static_cast<quint64>(totalBytes.QuadPart);
        existsError = false;
    }
    */
#else
    struct statvfs fsd;

    if ( statvfs(path.toAscii(), &fsd) == 0 )
    {
        total = static_cast<quint64>( fsd.f_blocks * fsd.f_frsize );
        existsError = false;
    }

#endif // _WIN32

    if (existsError)
    {
        total = 0;
    }

    return total;
}

quint64 HardDiskInformation::getFreeBytesPlataformEspecific(QString path)
{
    quint64 total;
    bool existsError = true;

#ifdef _WIN32
#error Mètode no comprovat en win32. Cal descomentar i comprovar-ho.
    /*
    ULARGE_INTEGER freeBytes;
    
    if ( GetDiskFreeSpaceExA(path.toAscii(), &freeBytes, null, null) )
    {
        total = static_cast<quint64>(freeBytes.QuadPart);
        existsError = false;
    }
    */
#else
    struct statvfs fsd;

    if ( statvfs(path.toAscii(), &fsd) == 0 )
    {
        total = static_cast<quint64>( fsd.f_bavail * fsd.f_bsize );
        existsError = false;
    }

#endif // _WIN32

    if (existsError)
    {
        total = 0;
    }

    return total;
}

}; //end udg namespace
