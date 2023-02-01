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

#ifndef UDGPACSQCONFIGURATIONSCREEN_H
#define UDGPACSQCONFIGURATIONSCREEN_H

#include "ui_qpacsconfigurationscreenbase.h"

namespace udg {

class PacsDeviceModel;

/**
    Interfície que permet configurar els paràmetres del PACS
  */
class QPacsConfigurationScreen : public QWidget, private ::Ui::QPacsConfigurationScreenBase {
Q_OBJECT
public:
    /// Constructor de la classe
    QPacsConfigurationScreen(QWidget *parent = 0);

    /// Destructor de classe
    ~QPacsConfigurationScreen();

private slots:
    /// Shows a dialog to add a new DIMSE PACS.
    void addDimsePacs();

    /// Shows a dialog to add a new WADO PACS.
    void addWadoPacs();

    /// Shows a dialog to add a new WADO-URI + DIMSE PACS.
    void addWadoUriDimsePacs();

    /// Shows a dialog to edit the selected PACS.
    void editPacs();

    /// Slot que esborra el pacs seleccionat
    void deletePacs();

    /// Enables or disables Edit and Delete buttons depending on whether a PACS is selected or not.
    void updateButtonsState();

    /// Si el port que s'indica per les connexions entrants del Pacs canvia, comprovem si està en ús per un altra aplicació que no sigui
    ///  l'Starviewer, si està en ús es mostrar un warning al cantó del TextBox
    void checkIncomingConnectionsPortNotInUse();

    /// Updates for local DICOM configuration settings
    void updateAETitleSetting();
    void updateTimeoutSetting();
    void updateLocalPortSetting();
    void updateMaxConnectionsSetting();
    
    /// Updates for Institution contact information settings
    void updateInstitutionNameSetting();
    void updateInstitutionAddressSetting();
    void updateInstitutionTownSetting();
    void updateInstitutionZipCodeSetting();
    void updateInstitutionCountrySetting();
    void updateInstitutionEmailSetting();
    void updateInstitutionPhoneNumberSetting();

private:
    /// Crea els connects dels signals i slots
    void createConnections();
    void createLocalConfigurationTabConnections();

    /// Crea els input validators necessaris pels diferents camps d'edició.
    void configureInputValidator();

    /// Emplena els textboxs amb les dades del PACS
    void loadPacsDefaults();

    /// Emplena els textbox amb la informació de la Institució
    void loadInstitutionInformation();

    /// Indica si el port per Connexions Entrans del PACS és utilitzat per una altra aplicació
    bool isIncomingConnectionsPortInUseByAnotherApplication();

    /// Refreshes the PACS list in this configuration screen and in the query screen.
    void refreshPacsList();

private:
    /// The model for the table view.
    PacsDeviceModel *m_pacsDeviceModel;
};

};// end namespace udg

#endif
