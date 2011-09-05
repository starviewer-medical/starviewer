#ifndef IMAGEOVERLAYHELPER_H
#define IMAGEOVERLAYHELPER_H

namespace udg {
class ImageOverlay;
}

namespace testing {

/// Classe helper per ImageOverlay
class ImageOverlayHelper {
public:
    /// Compara dos ImageOverlay i ens diu si són iguals
    static bool areEqual(const udg::ImageOverlay &overlay1, const udg::ImageOverlay &overlay2);    
};

}

#endif // IMAGEOVERLAYHELPER_H
