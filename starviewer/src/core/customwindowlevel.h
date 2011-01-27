#ifndef UDGCUSTOMWINDOWLEVEL_H
#define UDGCUSTOMWINDOWLEVEL_H

#include <QString>

namespace udg {

/**
   Classe que ens representa el Mappeig d'un fitxer que conté la llista de custom window levels
*/

class CustomWindowLevel {

public:    
    CustomWindowLevel();

    ~CustomWindowLevel();

    /// Mètode per definir el nom del custom window level
    void setName(const QString &name);
    /// Mètode per definir el window del custom window level
    void setWindow(double window);
    /// Mètode per definir el level del custom window level
    void setLevel(double level);

    /// Mètode que retorna el nom del custom window level
    QString getName() const;
    /// Mètode que retorna el window del custom window level
    double getWindow() const;
    /// Mètode que retorna el level del custom window level
    double getLevel() const;

private:    
    QString m_name;
    double m_window;
    double m_level;
};

}

#endif