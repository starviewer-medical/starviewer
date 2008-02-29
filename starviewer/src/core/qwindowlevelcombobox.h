/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQWINDOWLEVELCOMBOBOX_H
#define UDGQWINDOWLEVELCOMBOBOX_H

#include <QComboBox>

namespace udg {

// FWD declarations
class QCustomWindowLevelDialog;
class WindowLevelPresetsToolData;

/**
Combo box personalitzat per a escollir valors de window level predeterminats i personalitzats

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QWindowLevelComboBox : public QComboBox
{
Q_OBJECT
public:
    QWindowLevelComboBox(QWidget *parent = 0);

    ~QWindowLevelComboBox();

    /**
     * Li assignem la font de dades a partir de la qual obté els valors de window level
     * @param windowLevelData Les dades en sí
     */
    void setPresetsData( WindowLevelPresetsToolData *windowLevelData );

public slots:
    /**
     * Selecciona el preset indicat en el combo, però no l'activa
     * @param preset Descripció del preset
     */
    void selectPreset( const QString &preset );

private slots:
    /**
     * Afegeix o elimina un preset de la llista
     * @param preset Preset a afegir o eliminar
     */
    void addPreset(QString preset);
    void removePreset(QString preset);

    /// Seleccionem el window level que volem aplicar com a actiu
    void setActiveWindowLevel( int value );

private:
    /// Diàleg per escollir un window level personalitzat
    QCustomWindowLevelDialog *m_customWindowLevelDialog;

    /**
     * Omple el combo a partir de les dades de presets. Neteja les dades que hi pugui haver anteriorment
     */
    void populateFromPresetsData();

    /// Les dades dels valors pre-establerts
    WindowLevelPresetsToolData *m_presetsData;
};

}

#endif
