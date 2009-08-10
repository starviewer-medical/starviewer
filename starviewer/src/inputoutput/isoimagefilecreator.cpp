/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *

 ***************************************************************************/

#include "isoimagefilecreator.h"

#include <QString>
#include <QApplication>
#include <QProgressDialog>
#include <QProcess>

namespace udg {

IsoImageFileCreator::IsoImageFileCreator()
{
}

IsoImageFileCreator::~IsoImageFileCreator()
{
}

bool IsoImageFileCreator::createIsoImageFile( QString input, QString output )
{
    QProcess process;
    QStringList processParameters;

    //com que de moment no hi ha comunicacio amb el mkisofs es crea aquest progress bar per donar algo de feeling a l'usuari, per a que no es pensi que s'ha penjat l'aplicació
    QProgressDialog *progressBar = new QProgressDialog( QObject::tr( "Creating DICOMDIR Image..." ) , "" , 0 , 10 );
    progressBar->setMinimumDuration( 0 );
    progressBar->setCancelButton( 0 );
    progressBar->setValue( 7 );

    processParameters << "-V"; 
    processParameters << "STARVIEWER DICOMDIR"; //indiquem que el label de la imatge és STARVIEWER DICOMDIR
    processParameters << "-o"; 
    processParameters << output; //nom i directori on guardarem la imatge
    processParameters << input; //path a convertir en iso

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

#if defined(Q_OS_WIN) || defined(Q_OS_MAC) //WINDOWS i MAC. Es presuposa que el mkisofs es troba a la mateixa carpeta que l'executable de l'starviewer
    process.start(QCoreApplication::applicationDirPath() + "/mkisofs", processParameters );
#else //UNIX
    process.start("mkisofs", processParameters );
#endif
    
    process.waitForFinished( -1 ); //esperem que s'hagi generat la imatge
    QApplication::restoreOverrideCursor();
    progressBar->close();

    return process.exitCode() == 0;
}

}
