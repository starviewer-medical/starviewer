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

/** Converteix un directori determinat a dicomdir. Per a que un directori es pugui convertir a dicomdir, els fitxer dicom han de tenir 
    un nom com a molt de 8 caràcters.
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

    /** Indiquem si s'ha de comprovar si les imatges són del transfer syntax que indica la norma DICOM en funció del dispositiu de gravació escollit.
     * En funció del dispositiu escollit on es crea el dicomdir la normativa DICOM indica que el DICOMDIR només pot contenir imatges 
     * d'una determinada transfer syntax, per exemple pel profile STD-GEN-CD el  s'indica que només poden contenir imatges amb transfer syntax 
     * Explicit Little Endian, o en el cas de STD-GEN-DVD-JPEG s'accepta Explicit Little Endian i algunes transfer syntax de jpeg lossless i Lossy,
     * si alguna de les imatges no compleix el transfer syntax el DICOMDIR no és DICOM conformance.
     * Aquesta funció permet habilitar o deshabilitat comprovar el transfer syntax de les imatges, fent que en un DICOMDIR poguem o no tenir imatges 
     * que no siguin del transfer syntax que indica la normativa DICOM, si està habilitat i hi ha imatges que no compleixen el transfer syntax es dona error, 
     * i si ho deshabilitem  es podem generar DICOMDIR per un determinat profile encara que contingui imatges que no compleixen els transfer syntax acceptats 
     * per la normativa DICOM per aquell profile, ja que no es comprovarà el transfer syntax de les imatges
     *
     * Per defecte es comprova la transfer syntax de les imatges.
     * ATENCIÓ! Si no comprovem la transfer syntax podem generar DICOMDIR que no siguin DICOM conformance
     * A l'annex H del PS 3.11 es pot consulta cada profile quines transfer syntax suporta.
     */
    void setCheckTransferSyntax(bool checkTransferSyntax);

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
