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

#ifndef UDGQDICOMPRINTERBASICSETTINGSWIDGETBASE_H
#define UDGQDICOMPRINTERBASICSETTINGSWIDGETBASE_H

#include "ui_qdicomprinterbasicsettingswidgetbase.h"

#include "dicomprinter.h"

namespace udg {

/// Classe que mostra i permet especificar els paràmetres bàsics de configuració d'una impressora dicom;
class QDicomPrinterBasicSettingsWidget : public QWidget, private::Ui::QDicomPrinterBasicSettingsWidgetBase {
Q_OBJECT

public:
    QDicomPrinterBasicSettingsWidget(QWidget *parent = 0);

    /// Retorna/Mostra els basic printer settings
    void getDicomPrinterBasicSettings(DicomPrinter &dicomPrinter);
    void setDicomPrinterBasicSettings(DicomPrinter &dicomPrinter);

    /// Neteja els controls
    void clear();

signals:
    /// Signal que s'emet quan es canvia algun dels settings d'una impressora
    void basicDicomPrinterSettingChanged();

private slots:
    /// Slot que emet signal indicant que algunes de les propietats de la impressora DICOM ha canviat
    void settingChanged();

private:
    void createConnections();

    /// Emplena DicomPrinter amb les dades del GroupBox Film Settings
    void getFilmSettings(DicomPrinter &dicomPrinter);

    /// Emplena DicomPrinter amb les dades del GroupBox Print Settings
    void getPrintSettings(DicomPrinter &dicomPrinter);

    /// Mostra els paràmetres de Film Settings
    void setFilmSettings(DicomPrinter &dicomPrinter);

    /// Mostra els paràmetres del Print Settings
    void setPrintSettings(DicomPrinter &dicomPrinter);
};
} // end namespace udg.

#endif
