#ifndef IMAGETESTUTILS_H
#define IMAGETESTUTILS_H

#include <QString>

namespace udg
{

class Image;

/// Classe que retorna Imatges per utilitzar per testing
class ImageTestUtils
{
public:
    static Image* getTestImage(QString instanceNumber, QString SOPInstanceUID);
};

}

#endif // IMAGETESTUTILS_H
