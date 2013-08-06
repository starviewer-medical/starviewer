#ifndef UDGSYNCHRONIZETOOLDATA_H
#define UDGSYNCHRONIZETOOLDATA_H

#include "tooldata.h"

namespace udg {

/**
    Classe per guardar les dades de la tool de sincronització.
  */
class SynchronizeToolData : public ToolData {
Q_OBJECT
public:
    SynchronizeToolData();
    ~SynchronizeToolData();

    /// Obtenir l'increment de llesca
    double getIncrement();

    /// Obtenir la vista que s'ha incrementat
    QString getIncrementView();

public slots:
    /// Per canviar les dades de sincronitzacio de llesques
    void setIncrement(double value, QString view);

signals:
    /// Signal que s'emet quan les dades de l'increment de llesca canvien
    void sliceChanged();

private:
    /// Dades per la sincronització de l'slicing. Distància recorreguda
    double m_increment;

    /// Vista on ha de tenir efecte l'increment
    QString m_incrementView;
};

}

#endif
