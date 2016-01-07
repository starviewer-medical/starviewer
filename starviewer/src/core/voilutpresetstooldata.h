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

#ifndef UDGVOILUTPRESETSTOOLDATA_H
#define UDGVOILUTPRESETSTOOLDATA_H

#include "tooldata.h"

#include "voilut.h"

#include <QMap>

namespace udg {

/**
    Conté les dades de diversos valors predefinits de window level.
    Dins d'aquests valors, en podem tenir diverses procedències, com per exemple
    - Els que defineix la informació DICOM de la imatge que s'està visualitzant en aquell moment
    - Un conjunt estàndar predefinit, com per exemple valors pre-establerts de diferents teixits CT
    - Valors definits per l'usuari
    - Altres...

    Aquests valors predefinits es poden afegir o treure dinàmicament.
    Aquests valors poden ser afegits a través de la lectura d'algun tipus de format (Settings, XML, parelles atribut-valor, com en fitxers ".ini", etc)

    Cada valor vindrà definit per una descripció textual més el parell de valors WL/WW, com per exemple "Bone CT", WL: 500, WW: 2000.
    La descripció serà tractada com a camp clau, per tant no hi poden haver dos presets amb el mateix nom.

    Per poder fer una diferenciació de la procedència de cada valor, es poden crear certs grups com ja s'ha anomenat abans.
  */
class VoiLutPresetsToolData : public ToolData {
Q_OBJECT
public:
    VoiLutPresetsToolData(QObject *parent = 0);
    ~VoiLutPresetsToolData();

    /// Etiquetes per cada agrupació de presets
    enum GroupsLabel { AutomaticPreset, FileDefined, StandardPresets, UserDefined, CustomPreset, Other };

    /// Afegeix un nou preset. Si la operació es fa amb èxit s'emet un senyal donant la informació del preset.
    /// @param preset WindowLevel object. Its description has to be unique.
    /// @param group Grup al que volem que pertanyi, que serà "Other" si no s'especifica
    void addPreset(const VoiLut &preset, GroupsLabel group = Other);

    /// Eliminem el preset que tingui la descripció donada
    /// @param description Descripció del preset que volem eliminar
    void removePreset(const VoiLut &preset);

    /// Elimina els presets d'un grup
    /// @param group Grup que volem buidar
    void removePresetsFromGroup(GroupsLabel group);

    /// Returns true if this VOI LUT data contains a preset with the given description.
    bool containsPreset(const QString &description) const;

    /// Returns the VOI LUT preset with the given description.
    /// If there isn't any VOI LUT preset matching the given description, a default-constructed VoiLut is returned.
    VoiLut getFromDescription(const QString &description) const;

    /// Returns the group to which the preset with the given description belongs.
    /// If there isn't any VOI LUT preset matching the given description, a default-constructed GroupsLabel is returned.
    GroupsLabel getGroup(const QString &description) const;

    /// Ens retorna una llista de presets que conté un grup
    /// @param group grup de presets
    /// @return Retorna llista de descripcions de presets del grup donat.
    /// Si no hi ha cap preset dins del grup demanat la llista serà buida.
    QStringList getDescriptionsFromGroup(GroupsLabel group) const;

    QList<VoiLut> getPresetsFromGroup(GroupsLabel group) const;
    
    /// Returns the current activated preset
    const VoiLut& getCurrentPreset() const;

    /// Shortcut for getCurrentPreset().getName()
    const QString& getCurrentPresetName() const;

    /// Updates the given preset with the new values of VOI LUT. A preset with the same name has to be present in order to update it.
    void updatePreset(const VoiLut &preset);

    /// Returns the name for the custom preset.
    static QString getCustomPresetName();

public slots:
    /// Sets the given VOI LUT as the custom and current VOI LUT. Its name is ignored and the one given by getCustomPresetName() is used instead.
    void setCustomVoiLut(VoiLut voiLut);

    /// Selects the current present by name from the available presets. If there's no preset with such name, it does nothing
    void selectPreset(const QString &presetName);

    /// Sets the given preset as the current one.
    /// If it already exists with the same name and values, it behaves exactly as selectPreset().
    /// If it already exists but has different values, it updates its values and then selects it.
    /// If it does not exist, it adds the preset, then selects it
    void setCurrentPreset(const VoiLut &preset);

signals:
    void presetAdded(VoiLut preset);
    void presetRemoved(VoiLut preset);

    /// Emitted when the current preset values have changed
    void currentPresetChanged(VoiLut preset);

    /// Emitted when a preset is selected
    void presetSelected(VoiLut preset);

private:
    /// Afegeix els CustomWindowLevels que hi ha al repository
    void loadCustomWindowLevelPresets();

private slots:
    /// Actualitza els CustomWindowLevels del repository
    void updateCustomWindowLevels();

private:
    /// Últim preset activat
    VoiLut m_currentPreset;

    /// Map from each description to its corresponding preset.
    QMap<QString, VoiLut> m_presetsByDescription;

    /// Map from each description to the groups it belongs to.
    QMap<QString, GroupsLabel> m_groupsByDescription;

};

}

#endif
