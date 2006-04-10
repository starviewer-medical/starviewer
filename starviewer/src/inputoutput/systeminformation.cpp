/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "systeminformation.h"

#ifdef _WIN32
	#include <windows.h>
#else // linux stuff
	#include <sys/statvfs.h>
	#include <sys/stat.h>
#endif // _WIN32

namespace udg {

SystemInformation::SystemInformation(QObject *parent)
 : QObject(parent)
{
}

unsigned long long SystemInformation::getDiskFreeSpace(QString dir)
{
    unsigned long long fFree, blocksFree, blockSize;
#ifdef _WIN32
//codi per Windows
	
	ULARGE_INTEGER free,total;
	bool bRes = ::GetDiskFreeSpaceExA( 0 , &free , &total , NULL );
	if ( !bRes ) return false;

	QDir::setCurrent( dir );

	fFree = static_cast<double>( static_cast<__int64>(free.QuadPart)  );// / fKB;
// 	fTotal = static_cast<double>( static_cast<__int64>(total.QuadPart));// / fKB;

#else 
//codi per linux 
	
        struct stat stst;
	struct statvfs stvfs;
        double Mb = 1024*1024;

	if ( ::stat(dir.toLocal8Bit(),&stst) == -1 ) return false; //retrona el numero de blocs lliures
	if ( ::statvfs(dir.toLocal8Bit(),&stvfs) == -1 ) return false; //retorna la mida dels blocs

	blocksFree = stvfs.f_bavail;
	blockSize = stst.st_blksize;
        fFree = ( blocksFree * blockSize ) / Mb;

#endif // _WIN32
	
        return fFree;
}
SystemInformation::~SystemInformation()
{
}


}
