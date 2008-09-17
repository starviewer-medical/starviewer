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
    Classe que s'encarrega de "omplir" un Patient a partir de fitxers DICOM. Bàsicament té dos modes d'operació: "asíncron" i "síncron".

    En el mode d'operació asíncron el que podem fer és anar enviant fitxers dicom a processar i, un cop no tenim més fitxers, indiquem que
    que finalitzi el procés per obtenir el resultat amb finishDICOMFilesProcess. El resultat ens el donarà en el signal
    patientProcessed(Patient*).

    En canvi, en el mode d'operació síncron, simplement cal cridar el mètode processDICOMFileList passant la llista de fitxers que
    volem processar.

    El mètode asíncron està pensant per poder processar fitxers DICOM un a un a mesura que els anem obtenit en threads diferents, siguent
    recomenable el mètode síncron en qualsevol altra situació.

    No es poden utilitzar els dos mètodes de processament alhora en el mateix fitxer.

    \TODO De moment hi ha la limitiació de que es pressuposa que totes les imatges que se li passen són del mateix pacient.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PatientFiller : public QObject {
    Q_OBJECT
public:
    PatientFiller(QObject * parent = 0);

    ~PatientFiller();

public slots:

    ///Processem un fitxer DICOM. Ens permet anar passant fitxers un a un i, un cop acabem, cridar el mètode finishDICOMFilesProcess
    ///per obtenir el resultat a partir del signal patientProcessed.
    ///Es presuposa que el fitxer DICOM passat no està buit.
    void processDICOMFile(DICOMTagReader *dicomTagReader);

    ///Indica que ja hem acabat de processar fitxers i ja podem obtenir el resultat final, és a dir, l'estructura Patient omplerta.
    ///Aquesta se'ns dona a partir del signal patientProcessed.
    void finishDICOMFilesProcess();
    
    ///Abortem el procés asíncron d'omplir un Patient a partir de fitxers DICOM. La seva tasca és eliminar les estructures internes
    ///que els fillers poguessin està utilitant.
    void abortDICOMFilesProcess();

    ///Processa tots els fitxers que se li passin de cop, retornant la llista d'objectes Patient que es generin.
    QList<Patient*> processDICOMFileList(QStringList dicomFiles);

signals:
    /// Senyal que s'emet cada vegada que es processa un fitxer indicant quin és dintre del "lot" a processar.
    void progress(int);

    /// Senyal que s'emet quan en el mode asíncron s'ha acabat de processar totes les images passades.
    void patientProcessed(Patient *);

    /// Senyal que s'emet quan en el mode asíncron s'ha acabat l'abortament del processat.
    void DICOMFilesProcessAborted();

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
