#ifndef SERIESTESTHELPER_H
#define SERIESTESTHELPER_H

#include <QString>

namespace udg {
    class Series;
}

namespace testing {

/// Classe que retorna Series per utilitzar per testing
class SeriesTestHelper
{
public:
    /// Construeix una sèrie amb seriesInstanceUID i hi afageix com a fill el número d'imatges indicades. El SOPInstanceUID de la Imatge
    /// serà 1 per la primera imatge, 2 per la segona imatge...
    static udg::Series* createSeriesByID(QString seriesInstanceUID, int numberOfImagesToAddToSeries = 0);
};

}
#endif // SERIESTESTHELPER_H
