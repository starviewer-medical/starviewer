/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGQCRASHREPORTER_H
#define UDGQCRASHREPORTER_H

#include <QWidget>
#include "ui_qcrashreporterbase.h"

namespace udg {

/** Aquesta classe és l'encarregada de fer tota la gestió del crash reporter. Bàsicament el que fa és informar a l'usuari que l'aplicació ha petat, 
  * permet veure la trama del problema i enviar la informació a l'equip de desenvolupament si ho creu oportú.
  */
class QCrashReporter : public QWidget, Ui::QCrashReporterBase {
Q_OBJECT

public:
    QCrashReporter( const QStringList& argv , QWidget *parent = 0);

public slots:
    void quitButtonClickedSlot();
    void restartButtonClickedSlot();

private:
    bool restart(const char * path);
    void sendReport();
    void maybeSendReport();

private:
    QString m_minidumpPath;
};

}

#endif
