#ifndef IMAGETESTHELPER_H
#define IMAGETESTHELPER_H

#include <QString>

namespace udg{
    class Image;
}

namespace testing
{

/// Classe que retorna Imatges per utilitzar per testing
class ImageTestHelper
{
public:
    static udg::Image* createImageByUID(QString SOPInstanceUID);
    static udg::Image* createImageWithSexAndHeightAndWeight(QString sex, double height, int weight);
    static udg::Image* createImageWithHeightAndWeight(double height, int weight);

    /// Elimina els objectes de l'estructura Patient/Study/Series/Image que pertany la serie
    static void cleanUp(udg::Image *image);
};

}

#endif // IMAGETESTHELPER_H
