#ifndef UDGAUTOMATICSYNCHRONIZATIONTOOLDATA_H
#define UDGAUTOMATICSYNCHRONIZATIONTOOLDATA_H

#include "tooldata.h"
#include <QHash>

namespace udg {

/**
    Classe per guardar les dades de la tool automatica de sincronització.
*/

class AutomaticSynchronizationToolData : public ToolData {
Q_OBJECT
public:

    /// Constructor i destructor
    AutomaticSynchronizationToolData();
    ~AutomaticSynchronizationToolData();

    /// Retorna la posició referent al frameOfReferenceUID i la vista view. Si no 
    double* getPosition(QString frameOfReferenceUID, QString view) const;

    /// Retorna cert, si existeix una posició pel frameOfReferenceUID i la vista view
    bool hasPosition(QString frameOfReference, QString view);

public slots:

    /// Posa la posició pel frameOfReferenceUID i la vista view. Si existeix el sobreescriu, altrament l'afegeix.
    void setPosition(QString frameOfReferenceUID, QString view, double position[3]);

private:

    /// Taula Hash que guarda per cada frame of reference UID un altre hash, que guarda per cada vista la posició on es troba.
    QHash<QString, QHash <QString, double*> > m_positionForEachFrameOfReferenceAndReconstruction;

};

}

#endif // UDGAUTOMATICSYNCHRONIZATIONTOOLDATA_H
