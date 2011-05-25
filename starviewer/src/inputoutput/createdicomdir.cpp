#include "createdicomdir.h"

#include <QString>
#include <QDir>
// Make sure OS specific configuration is included first
#include <osconfig.h>
#include <dctk.h>
// For class DicomDirInterface
#include <dcddirif.h>
// For class OFStandard
#include <ofstd.h>
// For class OFCondition
#include <ofcond.h>

#include "status.h"
#include "logging.h"

#if defined (HAVE_WINDOWS_H) || defined(HAVE_FNMATCH_H)
#define PATTERN_MATCHING_AVAILABLE
#endif

namespace udg {

CreateDicomdir::CreateDicomdir()
{
    // Permet gravar al discdur i tb usb's
    m_optProfile = DicomDirInterface::AP_GeneralPurpose;
    // La normativa dicom indica que el nom dels fitxers de dicomdir han de tenir una longitud de 8 caràcters i han d'estar en majúscules. Linux per a
    // sistemes de fitxer vfat com podrian ser els pendrive i per a nom de fitxers de 8 o menys caràcters sempre mostra els noms en minúscules, independenment
    // de que nosaltres els guardem en majúscules, degut això quan volem crear un dicomdir en un dispositiu vfat es copien els noms de les imatges en
    // minúscules, al generar el dicomdir ens dona problemes, per què es troben imatges en minúscules i DICOM no ho permet. Per solucionar aquests casos
    // hem d'activar de dcmtk enableMapFilenamesMode, que si es troba amb fitxers en minúscules ho ignora i crea el dicomdir.

    m_ddir.enableMapFilenamesMode(OFTrue);
}

CreateDicomdir::~CreateDicomdir()
{
}

void CreateDicomdir::setDevice(recordDeviceDicomDir deviceToCreateDicomdir)
{
    // Indiquem que el propòsit d'aquest dicomdir
    switch (deviceToCreateDicomdir)
    {
        case recordDeviceDicomDir(HardDisk):
            m_optProfile = DicomDirInterface::AP_GeneralPurpose;
            break;
        case recordDeviceDicomDir(CdRom):
            m_optProfile = DicomDirInterface::AP_GeneralPurpose;
            break;
        case recordDeviceDicomDir(DvdRom):
            m_optProfile = DicomDirInterface::AP_GeneralPurposeDVD;
            break;
        case recordDeviceDicomDir(UsbPen):
            m_optProfile = DicomDirInterface::AP_USBandFlash;
            break;
        default:
            m_optProfile = DicomDirInterface::AP_GeneralPurpose;
            break;
    }
}

void CreateDicomdir::setStrictMode(bool enabled)
{
    if (enabled)
    {
        // Rebutgem imatges que contingui tags de tipus 1 amb longitut 0
        m_ddir.enableInventMode(OFFalse);
        // Rebutja Imatges que no compleixin l'estàndard dicom en la codificació de la informació dels pixels
        m_ddir.disableEncodingCheck(OFFalse);
        // Rebutja imatges que no compleixin l'estàndard dicom en la codificació de la informació dels pixels
        m_ddir.disableResolutionCheck(OFFalse);
        // Rebutgem imatges que no tinguin PatientID
        m_ddir.enableInventPatientIDMode(OFFalse);

        INFO_LOG("Es creara el DICOMDIR en mode estricte de compliment del DICOM");
    }
    else
    {
        // No volem mode estricte

        // Si una imatge, no té algun tag de nivell 1, que són els tags obligatoris i que no poden tenir longitut 1, al crear el dicomdir se'ls inventa
        m_ddir.enableInventMode(OFTrue);
        // Accepta Imatges que no compleixin l'estàndard dicom en la codificació de la informació dels pixels
        m_ddir.disableEncodingCheck(OFTrue);
        // Accepta Imatges que no compleixi la resolució espacial
        m_ddir.disableResolutionCheck(OFTrue);
        // En cas que una pacient no tingui PatientID se l'inventa
        m_ddir.enableInventPatientIDMode(OFTrue);

        INFO_LOG("Es creara el DICOMDIR en mode permisiu en el compliment del DICOM");
    }
}

void CreateDicomdir::setCheckTransferSyntax(bool checkTransferSyntax)
{
    // Atenció el nom del mètode disableTransferSyntaxCheck és enganyós, perquè per desactivar el check dels transfer syntax, s'ha d'invocar passant-li
    // la variable amb el valor false, i per indicar que s'ha de comprovar la transfer syntax s'ha de cridar amb la variable a cert
    m_ddir.disableTransferSyntaxCheck(checkTransferSyntax);
}

Status CreateDicomdir::create(QString dicomdirPath)
{
    // Nom del fitxer dicomDir
    QString outputDirectory = dicomdirPath + "/DICOMDIR";
    // Create list of input files
    OFList<OFString> fileNames;
    const char *opt_pattern = NULL;
    const char *opt_fileset = DEFAULT_FILESETID;
    const char *opt_descriptor = NULL;
    const char *opt_charset = DEFAULT_DESCRIPTOR_CHARSET;
    OFCondition result;
    E_EncodingType opt_enctype = EET_ExplicitLength;
    E_GrpLenEncoding opt_glenc = EGL_withoutGL;

    Status state;

    // Busquem el fitxers al dicomdir. Anteriorment a la classe ConvertoToDicomdir s'han d'haver copiat els fitxers dels estudis seleccionats,
    // al directori dicomdir destí
    OFStandard::searchDirectoryRecursively("", fileNames, opt_pattern, qPrintable(QDir::toNativeSeparators(dicomdirPath)));

    // Comprovem que el directori no estigui buit
    if (fileNames.empty())
    {
        ERROR_LOG("El directori origen està buit");
        state.setStatus(" no input files: the directory is empty ", false, 1301);
        return state;
    }

    // Creem el dicomdir
    result = m_ddir.createNewDicomDir(m_optProfile, qPrintable(QDir::toNativeSeparators(outputDirectory)), opt_fileset);

    if (!result.good())
    {
        ERROR_LOG("Error al crear el DICOMDIR. ERROR : " + QString(result.text()));
        state.setStatus(result);
        return state;
    }

    // Set fileset descriptor and character set
    result = m_ddir.setFilesetDescriptor(opt_descriptor, opt_charset);
    if (result.good())
    {
        OFListIterator(OFString) iter = fileNames.begin();
        OFListIterator(OFString) last = fileNames.end();

        // Iterem sobre la llista de fitxer i els afegim al dicomdir
        while ((iter != last) && result.good())
        {
            // Afegim els fitxers al dicomdir
            result = m_ddir.addDicomFile(qPrintable(QString((*iter).c_str()).toUpper()), qPrintable(QDir::toNativeSeparators(dicomdirPath)));
            if (result.good())
            {
                iter++;
            }
        }

        if (!result.good())
        {
            ERROR_LOG("Error al convertir a DICOMDIR el fitxer : " + dicomdirPath + "/" + (*iter).c_str() + result.text());
            result = EC_IllegalCall;
        }
        else
        {
            // Escribim el dicomDir
            result = m_ddir.writeDicomDir(opt_enctype, opt_glenc);
        }
    }

    return state.setStatus(result);
}

}
