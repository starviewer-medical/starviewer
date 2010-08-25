/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQCONFIGURATIONSCREEN_H
#define UDGQCONFIGURATIONSCREEN_H

#include "ui_qconfigurationscreenbase.h"

namespace udg {

class PacsDevice;

/** Interfície que permet configurar els paràmetres del PACS
@author marc
*/
class QConfigurationScreen : public QWidget, private Ui::QConfigurationScreenBase
{
Q_OBJECT
public:
    /// Constructor de la classe
    QConfigurationScreen(QWidget *parent = 0);

    ///Destructor de classe
    ~QConfigurationScreen();

public slots:
    /// Aplica els canvis de la configuració
    bool applyChanges();

signals:
    /// Signal que s'emet cada vegada que hi ha un canvi a la configuració que pot afectar al queryscreen
    void configurationChanged(const QString& configuration);

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

    /// Slot que s'utilitza quant es fa algun canvi a la configuració, per activar els buttons apply
    void enableApplyButtons();

    /** Si el port que s'indica per les connexions entrants del Pacs canvia, comprovem si està en ús per un altra aplicació que no sigui 
     *  l'Starviewer, si està en ús es mostrar un warning al cantó del TextBox 
     */
    void checkIncomingConnectionsPortNotInUse();

    ///Slot que s'activa quan es prem els radioButton per indica si està disponible en el PACS el servei de Query/Retrieve
    void queryRetrieveServiceEnabledChanged();

    ///Slot que s'activa quan es prem els radioButton per indica si està disponible en el PACS el servei de Store
    void storeServiceEnabledChanged();

    /**Si s'ha indicat que el servei d'store està permés i aquest no té el port configurat al editar el valor del port de Q/R 
       *li donem per defecte el valor d'aquest*/
    void m_textQueryRetrieveServicePortChanged();

private:
    ///crea els connects dels signals i slots
    void createConnections();

    /// Crea els input validators necessaris pels diferents camps d'edició.
    void configureInputValidator();

    ///Comprova que ens hagin entrat les dades bàsiques per poguer fer un echo a un PACS.
    ///Aquestes dades són: que tingui AETitle, que tingui adreça, que tingui un servei activat i un port entre 0 i 65535
    bool validatePacsDeviceToEcho();

    ///Valida que tinguis les dades per fer un echo invocant el mètode validateBasicPacsDeviceToEcho() i a més comprova que la institució no estigui buida
    bool validatePacsDeviceToSave();

    /** Valida que els canvis de la configuració siguin correctes
     *  Port local entre 0 i 65535
     *  Numero màxim de connexions 15
     *  Path de la base de dades i directori dicom's existeix
     *  @return indica si els canvis son correctes
     */
    bool validateChanges();

    /// Emplena el ListView amb les dades dels PACS que tenim guardades a la bd
    void fillPacsListView();

    /// Emplena els textboxs amb les dades del PACS
    void loadPacsDefaults();

    ///Emplena els textbox amb la informació de la Institució
    void loadInstitutionInformation();

    /// Guarda els canvis a la configuració dels paràmetres del PACS
    void applyChangesPacs();

    ///Guarda els canvis fets a la informació de la institució
    void applyChangesInstitutionInformation();

    ///Indica si el port per Connexions Entrans del PACS és utilitzat per una altra aplicació
    bool isIncomingConnectionsPortInUseByAnotherApplication();

    ///A partir dels controls de la interfície retorna emplenat un objecte PacsDevice.
    PacsDevice getPacsDeviceFromControls();

private:
    QString m_selectedPacsID; /// Conté el ID del pacs seleccionat en aquell moment
    bool m_configurationChanged; ///Indica si la configuració ha canviat
};

};// end namespace udg

#endif
