/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPATIENTFILLER_H
#define UDGPATIENTFILLER_H

#include <QStringList>

namespace udg {

class PatientFillerInput;
class PatientFillerStep;
class Patient;
class DICOMTagReader;

/**
Classe encarregada de a partir d'un Patient i un conjunt d'arxius classificar-los dins de l'estructura Patient i omplir les dades corresponents segons el tipus d'arxiu que es tracti.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PatientFiller : public QObject {
    Q_OBJECT
public:
    PatientFiller(QObject * parent = 0);

    ~PatientFiller();

public slots:

    ///Procés encarregat de fer un processat de tots els mòduls per un fitxer DICOM passat.
    ///Es presuposa que el fitxer DICOM passat no està buit.
    void processDICOMFile(DICOMTagReader *dicomTagReader);

    ///Procés encarregat d'executar quelcom que s'hagi de fer un cop rebut tots els fitxers.
    void finishDICOMFilesProcess();

    /// Mètode que processa tots els fitxers que se li passin de cop, retornant la llista d'objectes
    /// Patient que es generin.
    QList<Patient*> processDICOMFileList(QStringList dicomFiles);

signals:
    /// Senyal que s'emet regularment al fer un fill o fillUntil indicant el % realitzat.
    void progress(int);

    /// Senyal que s'emet quan el finishDICOMFilesProcess ha acabat.
    void patientProcessed(Patient *);

private:
    /// S'encarrega de registrar els mòduls/steps que processaran l'input.
    // \TODO això en un futur ho farà una classe registradora, ara es fa tot aquí per conveniència
    void registerSteps();

    /// Processa un PatientFillerStep amb les dades d'input
    void processPatientFillerStep(PatientFillerStep *patientFillerStep, PatientFillerInput *input);

private:
    /// Registre d'steps
    QList<PatientFillerStep *> m_registeredSteps;

    /// S'encarrega de guardar l'input durant tota l'execucció dels mòduls. S'utilitza 
    /// en cas que es processin fitxer individualment.
    PatientFillerInput *m_patientFillerInput;

    // Contador per saber el núm. d'imatge que estem tractant.
    int m_imageCounter;
};

}

#endif
