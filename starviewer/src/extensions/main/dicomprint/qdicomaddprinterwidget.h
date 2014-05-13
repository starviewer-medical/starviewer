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

#ifndef UDGQDICOMADDPRINTERWIGET_H
#define UDGQDICOMADDPRINTERWIGET_H

#include "ui_qdicomaddprinterwidgetbase.h"

namespace udg {

class DicomPrinter;

/**
    Classe que ens permet afegir una nova impressora Dicom al Sistema
  */
class QDicomAddPrinterWidget : public QWidget, private::Ui::QDicomAddPrinterWidgetBase {
Q_OBJECT

public:
    QDicomAddPrinterWidget();

    /// Esborra les dades entrades per l'usuari
    void clearInputs();

signals:
    /// Signal que es llança quan una impressora s'ha afegit al sistema.
    void newPrinterAddedSignal(int);

public slots:
    /// Posa operatiu el boto d'afegir impressora quan tots els paràmetres estan entrats.
    void enableAddButton();

    /// Afegeix una impressora dicom a partir de les dades entrades per l'usuari.
    void addPrinter();

private:
    /// Es creen els connexions de la interfície
    void createConnections();

    /// Configurem InputValidator per alguns dels QLineEdit
    void configureInputValidator();

    /// Comprova la validesa dels paràmetres entrats. Retorna false si hi ha algun paràmetre erroni.
    bool validateInputSettings();

    /// Agafa les dades introduides per l'usuari i les afegeix a un DicomPrinter.
    void getPrinterSettingsFromControls(DicomPrinter &printer);
};
};
#endif
