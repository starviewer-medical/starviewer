/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQCUSTOMWINDOWLEVELDIALOG_H
#define UDGQCUSTOMWINDOWLEVELDIALOG_H

#include "ui_qcustomwindowleveldialogbase.h"

namespace udg {

// FWD declarations
class WindowLevelPresetsToolData;

/**
Diàleg per escollir manualment els ajustaments pel window level

	@author Grup de Gràfics de Girona  (GGG) <vismed@ima.udg.es>
*/
class QCustomWindowLevelDialog : public QDialog, private Ui::QCustomWindowLevelDialogBase
{
Q_OBJECT
public:
    QCustomWindowLevelDialog(QDialog *parent = 0);

    ~QCustomWindowLevelDialog();

    void setPresetsData(WindowLevelPresetsToolData *windowLevelData);

public slots:
    /// dóna el window level que mostrarà per defecte quan s'obri
    void setDefaultWindowLevel(double window, double level);
    
signals:
    /// envia la senyal del window level escollit
    void windowLevel(double window, double level);
    
private:
    /// Crea les connexions 
    void createConnections();
    /// Carrega des del fitxer .ini
    void loadFromFile();
    /// Actualitzem els presets
    void updatePresets();
    ///Guarda al fitxer .ini
    void saveToFile();
    ///Actualitzem el fitxer un cop borrem un preset.
    void refreshFile();

private slots:
    /// Comprova el window level quan diem ok, fa les validacions i envia els senyas pertinents
    void confirmWindowLevel();
    void windowLevelHandler();
    void editPresets();

private:
    /// Les dades dels valors pre-establerts
    WindowLevelPresetsToolData *m_presetsData;
    /// Estructura per guardar els presets composada per un QString que fara de clau i coma valor
    /// guardarem dos doubles agrupats en un QVector. Ala posició 0 el Window i a la 1 es Level.
    QMap <QString, QVector<double> > m_presets;
    ///Nom del preset
    QString m_description;
    ///Variable necessària per saber si s'ha d'actualitzar el m_presetsData al canviar devisor
    /// després de borrar un preset.
    bool needToReload;

};

}

#endif
