/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "testingextensionmediator.h"

namespace udg{

TestingExtensionMediator::TestingExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}


TestingExtensionMediator::~TestingExtensionMediator()
{
}

DisplayableID TestingExtensionMediator::getExtensionID() const
{
    return DisplayableID("TestingExtension",tr("Testing"));
}

bool TestingExtensionMediator::initializeExtension(QWidget* extension, ExtensionHandler* extensionHandler, Identifier mainVolumeID)
{
    return ( m_extension = qobject_cast<TestingExtension*>(extension) );
}

} //udg namespace
