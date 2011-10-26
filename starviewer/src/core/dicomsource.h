#ifndef DICOMSOURCE_H
#define DICOMSOURCE_H

#include <QStringList>

#include"pacsdevice.h"

namespace udg
{

///Aquesta classe ens indica quina és la font d'un objecte DICOM.
///Ara mateix només indica de quin PACS prové o podem descarregar un objecte DICOM, però en un futur també hi afegirem de quina font provés BD, DICOMDIR, mhdm ...
///Independentment de la font de l'objecte DICOM tots podran tenir com a informació el PACS d'on es pot descarregar, per exemple la font pot ser la BD i tenir
///la informació d'on podem tornar a descarregar l'estudi
///
///Si un dos PACS tenen el mateix AETTitle, Address i QueryPort encara que tinguin altres camps diferents com pot ser el ID DICOMSource els considerà que
///són el mateix PACS
class DICOMSource {
public:

    ///Afegeix/treu el PACS d'on podem obtenir l'objecte DICOM. Si el PACS que afegim ja existeix en el DICOMSource no s'hi afegeix
    ///Considera que és el mateix PACS si tenen el mateix AETitle, address i queryPort independentment del ID i els altres camps
    void addRetrievePACS(const PacsDevice &pacsDeviceToAdd);
    void removeRetrievePACS(const PacsDevice &pacsDeviceToRemove);

    ///Retorna els PACS d'on podem descarregar/consulta l'objecte DICOM
    QList<PacsDevice> getRetrievePACS() const;

    //Afegeix el DICOMSource el PACS d'un altre DICOMSource
    void addPACSDeviceFromDICOMSource(const DICOMSource &DICOMSourceToAdd);

    bool operator==(const DICOMSource &DICOMSourceToCompare);

private:
    /// Indica si està afegit ja un mateix PACS, docs pacs són el mateix quan tenen el mateix AETitle, Address i QueryPort
    //TODO: Aquest codi està duplicat a PacsDeviceManager, però com PacsDeviceManager (isContainedSamePacsDevice) està a inputoutput no pot
    //utilitzar aquest mètode, sinó tindríem dependència cíclica
    bool isAddedSamePacsDevice(const PacsDevice pacsDevice) const;

private:
    QList<PacsDevice> m_retrievePACS;
};
}
#endif
