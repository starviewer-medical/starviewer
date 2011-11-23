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

    /// Posar el FrameOfReferenceUID actualment seleccionat
    void setSelectedUID(QString uid);

    /// Obtenir el FrameOfReferenceUID actiu
    QString getSelectedUID();

    /// Posa el codi de grup al frameOfReferenceUID uid
    void setGroupForUID(QString uid, int group);

    /// Obté el codi de grup al frameOfReferenceUID uid
    int getGroupForUID(QString uid);

    /// Obté el nombre de grups
    int getNumberOfGroups();

    /// Obté la vista activa
    QString getSelectedView();

    /// Obté el grup actiu
    int getSelectedGroup();

    /// Obté el desplaçament que s'ha realitzat en l'ultim canvi de llesca
    double getDisplacement();

    /// Actualitza la el frameOfReferenceUID i la vista activa
    void updateActiveViewer(QString uid, QString view);

public slots:

    /// Posa la posició pel frameOfReferenceUID i la vista view. Si existeix el sobreescriu, altrament l'afegeix.
    void setPosition(QString frameOfReferenceUID, QString view, double position[3], double displacement);

private:

    /// Taula hash que guarda per cada frame of reference UID un altre hash, que guarda per cada vista la posició on es troba.
    QHash<QString, QHash <QString, double*> > m_positionForEachFrameOfReferenceAndReconstruction;

    /// Taula hash que guarda per cada frame of reference UID el grup al qual pertany
    QHash<QString, int> m_UIDgroup;

    /// FrameOfReferenceUID actiu
    QString m_selectedUID;

    /// Quantitat de grups
    int m_numberOfGroups;

    /// Vista activa
    QString m_selectedView;

    /// Ultim desplaçament realitzat
    double m_lastDisplacement;

};

}

#endif // UDGAUTOMATICSYNCHRONIZATIONTOOLDATA_H
