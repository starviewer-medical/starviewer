#ifndef UDGSCREEN_H
#define UDGSCREEN_H

#include <QRect>

namespace udg {

/**
    Classe per definir una pantalla. Ens permet definir la seva geometria, el seu ID i si és principal o no.
 */
class Screen {
public:
    Screen();
    Screen(const QRect &geometry, const QRect &availableGeometry);
    ~Screen();

    /// Assigna/Obté la geometria de la pantalla
    void setGeometry(const QRect &geometry);
    QRect getGeometry() const;

    /// Assigna/Obté la geometria disponible de la pantalla per les aplicacions, tenint en compte, per exemple,
    /// les barres de menú de sistema i altres elements de l'entorn d'escriptori
    void setAvailableGeometry(const QRect &geometry);
    QRect getAvailableGeometry() const;

    /// Indica/Obté si la pantalla és principal
    void setAsPrimary(bool isPrimary);
    bool isPrimary() const;

    /// Assigna/obté l'ID de la pantalla
    void setID(int ID);
    int getID() const;

    /// Ens retorna les propietats de l'objecte en un string
    QString toString() const;
    
    /// Operador igualtat
    bool operator==(const Screen &screen) const;
    
    /// Constant per definir l'ID de pantalla inexistent
    static const int NullScreenID;

private:
    /// Inicialitza valors per defecte
    void initializeValues();

private:
    /// Indica si és una pantalla principal
    bool m_isPrimary;

    /// Geometria total i disponible de la pantalla
    QRect m_geometry;
    QRect m_availableGeometry;

    /// ID de la pantalla
    int m_ID;
};

} // End namespace udg

#endif
