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
    static udg::Image* createImageByID(QString SOPInstanceUID);
};

}

#endif // IMAGETESTHELPER_H
