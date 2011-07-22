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
    static Image* getTestImage(QString SOPInstanceUID);
};

}

#endif // IMAGETESTHELPER_H
