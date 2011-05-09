#ifndef UDGVOLUMEBUILDER_H
#define UDGVOLUMEBUILDER_H

#include <QString>

namespace udg {

class Volume;

/**
Classe base per la generació de nous objectes Volume que contindrà un conjunt d'imatges assignades a una nova sèrie
i aquesta a un estudi (nou o no depenén del tipus de generador).

Es dóna la possiblitat d'assignar una descripció a la nova sèrie que es generarà (\sa setSeriesDescription()).
*/

class VolumeBuilder {
public:
    VolumeBuilder();

    ~VolumeBuilder();

    /// Mètode encarregat de generar el nou Volume.
    virtual Volume* build() = 0;

    /// Assignar/Obtenir la descripció que es vol assignar a la nova sèrie que es generarà.
    void setSeriesDescription(QString description);
    QString getSeriesDescription() const;

protected:

    QString m_seriesDescription;
};

};  // end namespace udg

#endif // VOLUMEBUILDER_H
