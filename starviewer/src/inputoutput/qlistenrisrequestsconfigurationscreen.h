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

#ifndef UDGQLISTENRISREQUESTSCONFIGURATIONSCREEN_H
#define UDGQLISTENRISREQUESTSCONFIGURATIONSCREEN_H

#include "ui_qlistenrisrequestsconfigurationscreenbase.h"

namespace udg {

/**
    Widget to edit RIS settings
  */
class QListenRisRequestsConfigurationScreen : public QWidget, private Ui::QListenRisRequestsConfigurationScreenBase {
Q_OBJECT

public:
    /// Constructor de la classe
    QListenRisRequestsConfigurationScreen(QWidget *parent = 0);

    /// Destructor de classe
    ~QListenRisRequestsConfigurationScreen();

private:
    /// Crea els connects dels signals i slots
    void createConnections();

    /// Carrega la configuració actual del RIS
    void loadRisConfiguration();

    /// Configura la màscara del textbox
    void configureInputValidator();

private slots:
    /// Updates the label with warning message when appropiate
    void updateRISRequestsPortWarning();
    
    /// Slots that update the corresponding settings when appropiate
    void updateListenRISRequestsSetting(bool enable);
    void updateRISRequestsPortSetting();
    void updateAutomaticallyViewStudiesSetting(bool enable);
};

};// end namespace udg

#endif
