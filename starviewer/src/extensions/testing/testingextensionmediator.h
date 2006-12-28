/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef TESTINGEXTENSIONMEDIATOR_H
#define TESTINGEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "testingextension.h"

namespace udg{

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class TestingExtensionMediator: public ExtensionMediator
{
public:
    TestingExtensionMediator(QObject *parent = 0);

    ~TestingExtensionMediator();

    virtual bool setExtension(QWidget* extension);
    
    virtual DisplayableID getExtensionID() const;

private:
    TestingExtension* m_extension;

};

static InstallExtension<TestingExtension, TestingExtensionMediator> registerTestingExtension;


} //udg namespace

#endif
