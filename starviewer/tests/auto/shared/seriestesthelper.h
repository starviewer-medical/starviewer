#ifndef SERIESTESTHELPER_H
#define SERIESTESTHELPER_H

#include <QString>

namespace udg {

class Series;

/// Classe que retorna Series per utilitzar per testing
class SeriesTestHelper
{
public:
    /// Construeix una sèrie amb seriesInstanceUID i seriesNumber passat per paràmetre i hi afageix com a fill el número d'imatges indicades. El SOPInstanceUID de la Imatge
    /// i l'instance number serà 1 per la primera imatge, 2 per la segona imatge...
    static Series* getTestSeries(QString seriesInstanceUID, QString seriesNumber, int numberOfImagesToAddToSeries = 0);
};

}
#endif // SERIESTESTHELPER_H
