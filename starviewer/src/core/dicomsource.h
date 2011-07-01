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
class DICOMSource {
public:

    ///Afegeix/treu ID de PACS d'on podem obtenir l'objecte DICOM. Si el PACS que afegim ja existeix en el DICOMSource no s'hi afegeix
    void addRetrievePACS(const PacsDevice &pacsDeviceToAdd);
    void removeRetrievePACS(const PacsDevice &pacsDeviceToRemove);

    ///Retorna els ID del PACS d'on podem descarregar l'objecte DICOM
    QList<PacsDevice> getRetrievePACS() const;

    //Afegeix el DICOMSource el PACS d'un altre DICOMSource
    void addPACSDeviceFromDICOMSource(const DICOMSource &DICOMSourceToAdd);

    bool operator==(const DICOMSource &DICOMSourceToCompare);

private:
    QList<PacsDevice> m_retrievePACS;
};
};
#endif
