#ifndef UDGOPTIMALVIEWERSGRIDESTIMATOR_H
#define UDGOPTIMALVIEWERSGRIDESTIMATOR_H

#include <QPair>
#include <QMap>

namespace udg {

/**
    Classe per calcular quin és el grid de visors òptim segons els nombre de visors mínims necessaris.
    Hi ha un nombre màxim de visors que es poden crear fixat actualment en 48.
    // TODO La classe hauria d'evolucionar perquè es puguin donar millors layouts òptims, segons orientació de pantalla,
    número de pantalles, etc.
  */
class OptimalViewersGridEstimator {
public:
    OptimalViewersGridEstimator();
    ~OptimalViewersGridEstimator();

    /// Donat un nombre mínim de cel·les necessàries, ens dóna la parella de files i columnes òptima
    QPair<int, int> getOptimalGrid(int minimumCells);

private:
    /// Mapa que ens diu pel nombre de viewers, quantes files i columnes són les idònies
    QMap<int, QPair<int, int> > m_gridsMap;

    /// Nombre màxim de viewers que es poden crear. Vindrà determinat pel contingut de m_gridsMap;
    int m_maximumNumberOfViewers;
};

} // End namespace udg

#endif
