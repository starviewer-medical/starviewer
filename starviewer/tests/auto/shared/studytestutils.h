#ifndef STUDYTESTUTILS_H
#define STUDYTESTUTILS_H

#include <QString>

namespace udg {

class Study;

/// Classe que retorna Studies per utilitzar per testing
class StudyTestUtils
{
public:
    /// Torna un Study amb les dades passades per paràmetre, i el número de series Indicat.
    static Study* getTestStudy(QString instanceUID, QString studyID, int numberOfSeriesToAddToStudy = 0, int numberOfImagesToAddToSeries = 0);
};

}

#endif // STUDYTESTUTILS_H
