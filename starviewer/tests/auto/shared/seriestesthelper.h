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

    /// Construeix una sèrie amb UID 1 i hi afageix com a fill el número d'imatges indicades El SOPInstanceUID de la Imatge
    /// serà 1 per la primera imatge, 2 per la segona imatge...
    static udg::Series* createSeries(int numberOfImagesToAddToSeries = 0);

    /// Construeix una sèrie amb seriesInstanceUID i hi afageix com a fill el número d'imatges indicades. El SOPInstanceUID de la Imatge
    /// serà 1 per la primera imatge, 2 per la segona imatge...
    static udg::Series* createSeriesByUID(QString seriesInstanceUID, int numberOfImagesToAddToSeries = 0);

    /// Elimina els objectes de l'estructura Patient/Study/Series/Image que pertany la serie
    static void cleanUp(udg::Series *series);
};

}
#endif // SERIESTESTHELPER_H
