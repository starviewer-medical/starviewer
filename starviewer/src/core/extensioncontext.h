#ifndef UDGEXTENSIONCONTEXT_H
#define UDGEXTENSIONCONTEXT_H

#include <QStringList>

namespace udg {

class Patient;
class Volume;

/**
Contexte on s'executa una extensió. Permet accedir a informació que pot ser útil a l'extensió com l'objecte Patient, la finestra on s'executa, els "readers"..
*/
class ExtensionContext{
public:
    ExtensionContext();

    ~ExtensionContext();

    /// Get/set del pacient amb el que ha de treballar l'extensió.
    Patient* getPatient() const;
    void setPatient(Patient *patient);

    /// Mètode de conveniència que ens dóna un volum per defecte a partir del contexte assignat.
    /// Això ens servirà per obtenir un volum a partir tant del pacient o si no
    /// tenim pacient (cas mhd's) fer-ho amb el volumeIdentifier
    Volume* getDefaultVolume() const;

private:
    Patient* m_patient;

    QStringList m_defaultSelectedStudies;
    QStringList m_defaultSelectedSeries;
};

}

#endif
