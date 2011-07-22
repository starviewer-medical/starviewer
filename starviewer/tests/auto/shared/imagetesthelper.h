#ifndef IMAGETESTHELPER_H
#define IMAGETESTHELPER_H

#include <QString>

namespace udg
{

class Image;

/// Classe que retorna Imatges per utilitzar per testing
class ImageTestHelper
{
public:
    static Image* createImageByID(QString SOPInstanceUID);
};

}

#endif // IMAGETESTHELPER_H
