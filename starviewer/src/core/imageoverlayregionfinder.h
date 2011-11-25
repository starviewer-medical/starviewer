#ifndef UDGIMAGEOVERLAYREGIONFINDER_H
#define UDGIMAGEOVERLAYREGIONFINDER_H

#include <QList>

class QBitArray;
class QRect;

namespace udg {

class ImageOverlay;

/**
    Aquesta classe permet trobar regions en un ImageOverlay intentant minimitzar l'àrea buida total però sense fer moltes regions molt petites.
 */
class ImageOverlayRegionFinder {

public:

    ImageOverlayRegionFinder(const ImageOverlay &overlay);

    /// Troba les regions de l'overlay que contenen objectes i les guarda a la llista de regions.
    /// Si optimizeForPowersOf2 és cert, ajunta les regions que juntes ocupen menys memòria de textures que per separat, tenint en compte que les textures tenen
    /// mides que són potències de 2.
    void findRegions(bool optimizeForPowersOf2);
    /// Retorna la llista de regions de l'overlay.
    const QList<QRect>& regions() const;

protected:

    /// Retorna la distància entre les regions donades, calculada com una distància de Chebyshev (http://en.wikipedia.org/wiki/Chebyshev_distance).
    static int distanceBetweenRegions(const QRect &region1, const QRect &region2);

private:

    /// Retorna l'índex per accedir a les dades a partir de la fila i la columna.
    int getDataIndex(int row, int column) const;
    /// Retorna l'índex de la fila a partir de l'índex de les dades.
    int getRowIndex(int i) const;
    /// Retorna l'índex de la columna a partir de l'índex de les dades.
    int getColumnIndex(int i) const;

    /// Fa créixer una regió a partir del píxel indicat. Emplena la màscara i retorna la regió trobada.
    QRect growRegion(int row, int column, QBitArray &mask);
    /// Posa a 1 tots els píxels de la màscara que pertanyen a la regió.
    void fillMaskForRegion(QBitArray &mask, const QRect &region);
    /// Afegeix un padding d'un píxel al voltant de la regió.
    void addPadding(QRect &region);
    /// Treu el padding d'un píxel al voltant de la regió.
    void removePadding(QRect &region);
    /// Afegeix la regió a la llista, fusionant-la amb altres si són molt properes.
    /// Si optimizeForPowersOf2 és cert, també les fusiona si juntes aprofiten millor la memòria de textures.
    void addRegion(QRect &region, bool optimizeForPowersOf2);

private:

    /// L'overlay que s'ha de separar.
    const ImageOverlay &m_overlay;
    /// Llista de regions trobades.
    QList<QRect> m_regions;

};

}

#endif // UDGIMAGEOVERLAYREGIONFINDER_H
