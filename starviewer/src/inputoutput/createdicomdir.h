/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCREATEDICOMDIR_H
#define UDGCREATEDICOMDIR_H

#include "dcddirif.h"     /* for class DicomDirInterface */

class QString;

namespace udg {

class Status;

/** Converteix un directori determinat a dicomdir. Per a que un directori es pugui convertir a dicomdir, els fitxer dicom han de tenir un nom com a molt de 8 caràcters, i han d'estar guardats en little endian
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class CreateDicomdir
{
public:

    enum recordDeviceDicomDir { HardDisk , CdRom , DvdRom  , UsbPen };

    CreateDicomdir();
    ~CreateDicomdir();

    /** Especifica a quin dispositiu crearem el dicomdir. És important especificar el dispositiu correctament, sinó ens podem trobar, que no ens crei correctament el dicomdir
     * @param deviceToCreateDicomdir Dispositiu on crearem el dicomdir (harddisk,cdDvd,usb)
     */
    void setDevice( recordDeviceDicomDir deviceToCreateDicomdir );

    /** Permet especificar si volem treballar amb Mode Estricte. Amb Mode estricte es comprova
     * - Que totes les imatges els tags de tipus 1 tinguin una longitut més gran de 0
     * - Que la codificació de la informació dels píxels de les imatges compleixi l'estàndard DICOM
     * - Que es compleixi la codificació de la resolució espacial en l'estànard DICOM
     * - Si una imatge no té ID de pacient se l'inventa
     * @param enabled indica si s'activa el mode esctricte
     */
    void setStrictMode(bool enabled);

    /** Crea el fitxer DicomDir amb l'estructura dels estudis del directori passat per paràmetre
     * @param dicomdirPath directori a convertir a dicomdir
     * @return estat de finalització del mètode
     */
    Status create( QString dicomdirPath );

private:

    DicomDirInterface::E_ApplicationProfile m_optProfile;
    DicomDirInterface m_ddir;
};

}

#endif
