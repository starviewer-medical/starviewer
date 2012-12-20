#ifndef UDGSTUDYLAYOUTCONFIGSETTINGSMANAGER_H
#define UDGSTUDYLAYOUTCONFIGSETTINGSMANAGER_H

#include <QList>

namespace udg {

class StudyLayoutConfig;

/** 
    Classe que s'encarrega d'obtenir, afegir, actualitzar i esborrar els StudyLayoutConfig desats a settings.
  */
class StudyLayoutConfigSettingsManager {
public:
    StudyLayoutConfigSettingsManager();
    ~StudyLayoutConfigSettingsManager();

    /// Ens retorna la llista de configuracions desades a settings
    QList<StudyLayoutConfig> getConfigList() const;

    /// Mètodes per afegir una nova configuració, actualitzar-la o esborrar-la. Retorna cert si l'operació es duu a terme amb èxit, fals altrament.

    /// Afegeix una nova configuració si no n'existeix cap per aquella modalitat
    bool addItem(const StudyLayoutConfig &config);

    /// Actualitza la configuració en cas que n'existeixi una de configurada per aquella modalitat
    bool updateItem(const StudyLayoutConfig &config);

    /// Esborra la configuració en cas que n'existeixi una de configurada per aquella modalitat
    bool deleteItem(const StudyLayoutConfig &config);
};

}

#endif
