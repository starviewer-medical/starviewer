/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGEXTENSIONCONTEXT_H
#define UDGEXTENSIONCONTEXT_H

#include "identifier.h"

namespace udg {

//class ExtensionHandler;
class Patient;

/**
Contexte on s'executa una extensió. Permet accedir a informació que pot ser útil a l'extensió com l'objecte Patient, la finestra on s'executa, els "readers"..

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ExtensionContext{
public:
    ExtensionContext();

    ~ExtensionContext();

/*   TODO Per temes de dependències de directoris, no es pot fer dependre de ExtensionHandler. Es deixa el codi per
     quan es tregui la dependència.
    /// get/set del extensionhandler. És un mètode que està destinat a desaparèixer.
    ExtensionHandler* getExtensionHandler() const;
    ExtensionHandler* setExtensionHandler(ExtensionHandler *extensionHandler);
*/
    /// get/set del pacient amb el que ha de treballar l'extensió.
    Patient* getPatient() const;
    void setPatient(Patient *patient);

    /// get/set del pacient amb el que ha de treballar l'extensió. És un mètode que està destinat a desaparèixer.
    Identifier getMainVolumeID() const;
    void setMainVolumeID(Identifier identifier);

private:
    //ExtensionHandler* m_extensionHandler;
    Identifier m_mainVolumeID;
    Patient* m_patient;
};

}

#endif
