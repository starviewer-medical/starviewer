#ifndef UDGWINDOWLEVELPRESETSTOOLDATA_H
#define UDGWINDOWLEVELPRESETSTOOLDATA_H

#include "tooldata.h"

#include <QMultiMap>
#include <QStringList>

#include "windowlevel.h"

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
class WindowLevelPresetsToolData : public ToolData {
Q_OBJECT
public:
    WindowLevelPresetsToolData(QObject *parent = 0);
    ~WindowLevelPresetsToolData();

    /// Etiquetes per cada agrupació de presets
    enum GroupsLabel { AutomaticPreset, FileDefined, StandardPresets, UserDefined, CustomPreset, Other };

    /// Afegeix un nou preset. Si la operació es fa amb èxit s'emet un senyal donant la informació del preset.
    /// @param preset WindowLevel object. Its description has to be unique.
    /// @param group Grup al que volem que pertanyi, que serà "Other" si no s'especifica
    void addPreset(const WindowLevel &preset, int group = Other);

    /// Eliminem el preset que tingui la descripció donada
    /// @param description Descripció del preset que volem eliminar
    void removePreset(const WindowLevel &preset);

    /// Elimina els presets d'un grup
    /// @param group Grup que volem buidar
    void removePresetsFromGroup(int group);

    /// Ens retorna els valors de window level del preset amb la descripció donada
    /// Si el preset no existeix els valor retornat serà 0 per tots dos
    /// @param description Descripció del preset
    /// @param window variable on es retornarà el valor de window
    /// @param level variable on es retornarà el valor de level
    /// @return Cert si existeix aquest preset, fals altrament
    bool getFromDescription(const QString &description, WindowLevel &preset);

    /// Ens diu a quin grup pertany el preset indicat. Si no existeix la descripció,
    /// el valor retornat en group és indeterminat
    /// @param description Descripció del preset que busquem
    /// @param group variable on se'ns tornarà el grup al que pertany la descripció donada
    /// @return Cert si la descripció donada existeix, fals altrement
    bool getGroup(const WindowLevel &preset, int &group);

    /// Ens retorna una llista de presets que conté un grup
    /// @param group grup de presets
    /// @return Retorna llista de descripcions de presets del grup donat.
    /// Si no hi ha cap preset dins del grup demanat la llista serà buida.
    QStringList getDescriptionsFromGroup(int group);

    QList<WindowLevel> getPresetsFromGroup(int group);
    
    /// Returns the current activated preset
    WindowLevel getCurrentPreset() const;

    /// Shortcut for getCurrentPreset().getName()
    QString getCurrentPresetName() const;

    /// Updates the given preset with the new values of window/level. A preset with the same name has to be present in order to update it.
    /// Returns true in case the preset could be updated, false otherwise (i.e. no preset with such name exists)
    bool updatePreset(const WindowLevel &preset);

public slots:
    /// Donem el valor del preset "A mida"
    /// @param window valor de window
    /// @param level valor de level
    void setCustomWindowLevel(double window, double level);

    /// Indiquem que volem activar el preset indicat
    /// @param preset Nom del preset a activar
    void activatePreset(const QString &presetName);

signals:
    void presetAdded(WindowLevel preset);
    void presetRemoved(WindowLevel preset);

    /// Aquest senyal s'envia pre notificar quin preset s'ha activat
    void presetChanged(WindowLevel preset);

private:
    /// Afegeix els CustomWindowLevels que hi ha al repository
    void loadCustomWindowLevelPresets();

private slots:
    /// Actualitza els CustomWindowLevels del repository
    void updateCustomWindowLevels();

private:
    /// Últim preset activat
    WindowLevel m_currentPreset;

    /// Map grouping presets by its group
    QMultiMap<int, WindowLevel> m_presetsByGroup;
};

}

#endif
