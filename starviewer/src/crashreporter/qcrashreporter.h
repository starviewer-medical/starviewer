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

#ifndef UDGQCRASHREPORTER_H
#define UDGQCRASHREPORTER_H

#include <QWidget>
#include "ui_qcrashreporterbase.h"

namespace udg {

/**
    Aquesta classe és l'encarregada de fer tota la gestió del crash reporter. Bàsicament el que fa és informar a l'usuari que l'aplicació ha petat,
    permet veure la trama del problema i enviar la informació a l'equip de desenvolupament si ho creu oportú.
  */
class QCrashReporter : public QWidget, Ui::QCrashReporterBase {
Q_OBJECT

public:
    QCrashReporter(const QStringList &argv, QWidget *parent = 0);

public Q_SLOTS:
    void quitButtonClickedSlot();
    void restartButtonClickedSlot();

private:
    bool restart(const QString &path);
    void sendReport();
    void maybeSendReport();

private:
    QString m_minidumpPath;
};

}

#endif
