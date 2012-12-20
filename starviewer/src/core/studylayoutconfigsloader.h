#ifndef UDGSTUDYLAYOUTCONFIGSLOADER_H
#define UDGSTUDYLAYOUTCONFIGSLOADER_H

class QString;

namespace udg {

class StudyLayoutConfig;

class StudyLayoutConfigsLoader {
public:
    StudyLayoutConfigsLoader();
    ~StudyLayoutConfigsLoader();

    /// Carrega els valors que corresponen per defecte o que ja estiguin configurats per l'usuari
    void load();

    /// Retorna la configuració de fàbrica d'un layout corresponent a una modalitat
    static StudyLayoutConfig getDefaultConfigForModality(const QString &modality);
};

} // End namespace udg
#endif
