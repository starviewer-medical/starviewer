/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQLISTENRISREQUESTSCONFIGURATIONSCREEN_H
#define UDGQLISTENRISREQUESTSCONFIGURATIONSCREEN_H

#include "ui_qlistenrisrequestsconfigurationscreenbase.h"

namespace udg {

/** Widget en el que es configuren els paràmetres de la Base de Dades local
*/
class QListenRisRequestsConfigurationScreen : public QWidget, private ::Ui::QListenRisRequestsConfigurationScreenBase
{
Q_OBJECT

public:
    /// Constructor de la classe
    QListenRisRequestsConfigurationScreen( QWidget *parent = 0 );

    ///Destructor de classe
    ~QListenRisRequestsConfigurationScreen();

public slots:
    /// Aplica els canvis de la configuració
    bool applyChanges();

signals:
    /// Signal que s'emet cada vegada que hi ha un canvi a la configuració que pot afectar al queryscreen
    void configurationChanged(const QString& configuration);

private slots:

    /// Slot que s'utilitza quant es fa algun canvi a la configuració, per activar els buttons apply
    void enableApplyButtons();

private:
    ///crea els connects dels signals i slots
    void createConnections();

    ///Carrega la configuració actual del RIS 
    void loadRisConfiguration();

    ///Configura la màscara del textbox
    void configureInputValidator();

    ///Crea els input validators necessaris pels diferents camps d'edició.
    bool validateChanges();

};

};// end namespace udg

#endif
