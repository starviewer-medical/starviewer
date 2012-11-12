#ifndef UDGSCREEN_H
#define UDGSCREEN_H

#include <QRect>

namespace udg {

/**
    Classe per definir una pantalla. Ens permet definir la seva geometria, el seu ID i si és principal o no.
    També incorpora mètodes per determinar les posicions relatives entre la seva geometria
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
    
    /// Ens diu si aquesta pantalla està més amunt/avall que l'altra. Només comprova que el seu top estigui per sobre/sota del top de la pantalla a comparar,
    /// per tant retornarà cert tant si està completa com parcialment per sobre/sota.
    bool isHigher(const Screen &screen);
    bool isLower(const Screen &screen);
    
    /// Ens diu si aquesta pantalla està més a la esquerra/dreta que l'altra. Només comprova que que l'esquerra/dreta de cadascú ho estigui més o no,
    /// per tant retornarà cert tant si està completa com parcialment més a l'esquerra/dreta.
    bool isMoreToTheLeft(const Screen &screen);
    bool isMoreToTheRight(const Screen &screen);

    /// Indica si aquesta pantalla està completament per sobre de l'altra. Comprova que el bottom d'aquesta estigui per sobre del top de l'altra.
    bool isOver(const Screen &screen) const;
    
    /// Indica si aquesta pantalla està completament per sota de l'altra. Comprova que el top d'aquesta estigui per sota del bottom de l'altra.
    bool isUnder(const Screen &screen) const;
    
    /// Indica si aquesta pantalla està completament a l'esquerra de l'altra. Comprova que el right d'aquesta sigui menor que el left de l'altra.
    bool isOnLeft(const Screen &screen) const;
    
    /// Indica si aquesta pantalla està completament a la dreta de l'altra. Comprova que el left d'aquesta sigui major que el right de l'altra.
    bool isOnRight(const Screen &screen) const;
    
    /// Indica si aquesta pantalla està sobre de l'altra. Per que sigui cert han de ser de la mateixa amplada
    /// i el top d'aquesta ha de ser igual al bottom de l'altra
    bool isTop(const Screen &screen) const;
    
    /// Indica si aquesta pantalla està sota de l'altra. Per que sigui cert han de ser de la mateixa amplada
    /// i el bottom d'aquesta ha de ser igual al top de l'altra
    bool isBottom(const Screen &screen) const;
    
    /// Indica si aquesta pantalla està a l'esquerra de l'altra. Per que sigui cert han de ser de la mateixa alçada
    /// i el right d'aquesta ha de ser igual al left de l'altra
    bool isLeft(const Screen &screen) const;
    
    /// Indica si aquesta pantalla està a la dreta de l'altra. Per que sigui cert han de ser de la mateixa alçada
    /// i el left d'aquesta ha de ser igual al right de l'altra
    bool isRight(const Screen &screen) const;
    
    /// Indica si aquesta pantalla està en diagonal a sobre a l'esquerra de l'altra. Per que sigui cert
    /// el punt bottomRight d'aquesta ha de ser igual al punt topLeft de l'altra
    bool isTopLeft(const Screen &screen) const;
    
    /// Indica si aquesta pantalla està en diagonal a sobre a la dreta de l'altra. Per que sigui cert
    /// el punt bottomLeft d'aquesta ha de ser igual al punt topRight de l'altra
    bool isTopRight(const Screen &screen) const;
    
    /// Indica si aquesta pantalla està en diagonal a sota a l'esquerra de l'altra. Per que sigui cert
    /// el punt topRight d'aquesta ha de ser igual al punt bottomLeft de l'altra
    bool isBottomLeft(const Screen &screen) const;
    
    /// Indica si aquesta pantalla està en diagonal a sota a la dreta de l'altra. Per que sigui cert
    /// el punt topLeft d'aquesta ha de ser igual al punt bottomRight de l'altra
    bool isBottomRight(const Screen &screen) const;
    
    /// Operador igualtat
    bool operator==(const Screen &screen) const;
    
    /// Constant per definir l'ID de pantalla inexistent
    static const int NullScreenID;

private:
    /// Threshold amb el que definim la màxima distància entre finestres per determinar que estan de cantó
    static const int MaximumDistanceInBetween;
    
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
