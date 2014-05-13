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

#ifndef UDGQCONFIGURATIONSCREEN_H
#define UDGQCONFIGURATIONSCREEN_H

#include "ui_qconfigurationscreenbase.h"

namespace udg {

class PacsDevice;

/**
    Interfície que permet configurar els paràmetres del PACS
  */
class QConfigurationScreen : public QWidget, private Ui::QConfigurationScreenBase {
Q_OBJECT
public:
    /// Constructor de la classe
    QConfigurationScreen(QWidget *parent = 0);

    /// Destructor de classe
    ~QConfigurationScreen();

private slots:
    /// Neteja els line edit de la pantalla
    void clear();

    /// Slot que dona d'alta el PACS a la la base de dades
    void addPacs();

    /// Slot que s'activa quant seleccionem un Pacs del PacsListView, omplint les caixes de texts amb les dades del Pacs seleccionat
    void updateSelectedPACSInformation();

    /// Slot que updata les dades d'un pacs
    void updatePacs();

    /// Slot que esborra el pacs seleccionat
    void deletePacs();

    /// Fa un echo a les dades del PACS que estan als textbox
    void test();

    /// Si el port que s'indica per les connexions entrants del Pacs canvia, comprovem si està en ús per un altra aplicació que no sigui
    ///  l'Starviewer, si està en ús es mostrar un warning al cantó del TextBox
    void checkIncomingConnectionsPortNotInUse();

    /// Slot que s'activa quan es prem els radioButton per indica si està disponible en el PACS el servei de Query/Retrieve
    void queryRetrieveServiceEnabledChanged();

    /// Slot que s'activa quan es prem els radioButton per indica si està disponible en el PACS el servei de Store
    void storeServiceEnabledChanged();

    /// Si s'ha indicat que el servei d'store està permés i aquest no té el port configurat al editar el valor del port de Q/R
    /// li donem per defecte el valor d'aquest
    void onQueryRetrieveServicePortChanged();

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

    /// Comprova que ens hagin entrat les dades bàsiques per poguer fer un echo a un PACS.
    /// Aquestes dades són: que tingui AETitle, que tingui adreça, que tingui un servei activat i un port entre 0 i 65535
    bool validatePacsDeviceToEcho();

    /// Valida que tinguis les dades per fer un echo invocant el mètode validateBasicPacsDeviceToEcho() i a més comprova que la institució no estigui buida
    bool validatePacsDeviceToSave();

    /// Emplena el ListView amb les dades dels PACS que tenim guardades a la bd
    void fillPacsListView();

    /// Emplena els textboxs amb les dades del PACS
    void loadPacsDefaults();

    /// Emplena els textbox amb la informació de la Institució
    void loadInstitutionInformation();

    /// Indica si el port per Connexions Entrans del PACS és utilitzat per una altra aplicació
    bool isIncomingConnectionsPortInUseByAnotherApplication();

    /// A partir dels controls de la interfície retorna emplenat un objecte PacsDevice.
    PacsDevice getPacsDeviceFromControls();

private:
    /// Conté el ID del pacs seleccionat en aquell moment
    QString m_selectedPacsID;
};

};// end namespace udg

#endif
