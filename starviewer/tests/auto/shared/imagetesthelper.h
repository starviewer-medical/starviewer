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

    /// Elimina els objectes de l'estructura Patient/Study/Series/Image que pertany la serie
    static void cleanUp(udg::Image *image);
};

}

#endif // IMAGETESTHELPER_H
