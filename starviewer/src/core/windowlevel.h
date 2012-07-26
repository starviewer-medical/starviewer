#ifndef UDGWINDOWLEVEL_H
#define UDGWINDOWLEVEL_H

#include <QString>

namespace udg {

/**
    Classe que encapsula el concepte de Window Level.
  */
class WindowLevel {

public:
    WindowLevel();
    ~WindowLevel();

    /// Defineix/obté la descripció window level
    void setName(const QString &name);
    QString getName() const;
    
    /// Defineix/obté el width del window level
    void setWidth(double width);
    double getWidth() const;
    
    /// Defineix/obté el level del window level
    void setLevel(double level);
    double getLevel() const;

private:
    QString m_name;
    double m_width;
    double m_level;
};

}

#endif
