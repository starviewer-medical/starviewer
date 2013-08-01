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

    /// Creats a series of a given modality and with the number of given images.
    static udg::Series* createSeriesByModality(QString modality, int numberOfImagesToAddToSeries = 0);

    /// Returns a random modality string
    static QString getRandomModality();
    
    /// Elimina els objectes de l'estructura Patient/Study/Series/Image que pertany la serie
    static void cleanUp(udg::Series *series);
};

}
#endif // SERIESTESTHELPER_H
