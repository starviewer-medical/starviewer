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

public slots:
    /// Aplica els canvis de la configuració
    bool applyChanges();

private slots:
    /// Slot que s'utilitza quant es fa algun canvi a la configuració, per activar els buttons apply
    void enableApplyButtons();

private:
    /// Crea els connects dels signals i slots
    void createConnections();

    /// Carrega la configuració actual del RIS
    void loadRisConfiguration();

    /// Configura la màscara del textbox
    void configureInputValidator();
};

};// end namespace udg

#endif
