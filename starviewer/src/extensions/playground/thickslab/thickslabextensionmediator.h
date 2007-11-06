/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef THICKSLABEXTENSIONMEDIATOR_H
#define THICKSLABEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "thickslabextension.h"

namespace udg{

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ThickSlabExtensionMediator: public ExtensionMediator
{
    Q_OBJECT
public:
    ThickSlabExtensionMediator(QObject *parent = 0);

    ~ThickSlabExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<ThickSlabExtension, ThickSlabExtensionMediator> registerThickSlabExtension;


} //udg namespace

#endif
