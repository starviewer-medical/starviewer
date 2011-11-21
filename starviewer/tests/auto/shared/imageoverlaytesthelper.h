#ifndef IMAGEOVERLAYTESTHELPER_H
#define IMAGEOVERLAYTESTHELPER_H

#include <QList>

class QRect;

namespace udg {
class ImageOverlay;
}

namespace testing {

/// Classe helper per ImageOverlay
class ImageOverlayTestHelper {
public:
    /// Crea diversos image overlays de diferents mides i amb dades i els retorna en una llista.
    static QList<udg::ImageOverlay> createImageOverlays();
    /// Crea les regions que engloben els suboverlays dels overlays creats per createImageOverlays().
    static QList< QList<QRect> > createSubOverlayRegions();
    /// Crea els suboverlays dels overlays creats per createImageOverlays().
    static QList< QList<udg::ImageOverlay> > createSubOverlays();
    /// Compara dos ImageOverlay i ens diu si són iguals
    static bool areEqual(const udg::ImageOverlay &overlay1, const udg::ImageOverlay &overlay2);    
};

}

#endif // IMAGEOVERLAYTESTHELPER_H
