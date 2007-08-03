/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "extensioncontext.h"

#include "patient.h"
//#include "extensionhandler.h"

namespace udg {

ExtensionContext::ExtensionContext()
{
}


ExtensionContext::~ExtensionContext()
{
}

/*
ExtensionHandler* ExtensionContext::getExtensionHandler() const
{
    return m_extensionHandler;
}

void ExtensionContext::setExtensionHandler(ExtensionHandler *extensionHandler)
{
    m_extensionHandler = extensionHandler;
}
*/
Patient* ExtensionContext::getPatient() const
{
    return m_patient;
}

void ExtensionContext::setPatient(Patient *patient)
{
    m_patient = patient;
}

Identifier ExtensionContext::getMainVolumeID() const
{
    return m_mainVolumeID;
}

void ExtensionContext::setMainVolumeID(Identifier identifier)
{
    m_mainVolumeID = identifier;
}

}
