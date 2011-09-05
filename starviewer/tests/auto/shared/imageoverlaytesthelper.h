#ifndef IMAGEOVERLAYTESTHELPER_H
#define IMAGEOVERLAYTESTHELPER_H

namespace udg {
class ImageOverlay;
}

namespace testing {

/// Classe helper per ImageOverlay
class ImageOverlayTestHelper {
public:
    /// Compara dos ImageOverlay i ens diu si són iguals
    static bool areEqual(const udg::ImageOverlay &overlay1, const udg::ImageOverlay &overlay2);    
};

}

#endif // IMAGEOVERLAYTESTHELPER_H
