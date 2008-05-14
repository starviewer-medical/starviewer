/**************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGLESIONVISUALIZATIONEXTENSIONMEDIATOR_H
#define UDGLESIONVISUALIZATIONEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qlesionvisualizationextension.h"

namespace udg {

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class LesionVisualizationExtensionMediator : public ExtensionMediator
{
    Q_OBJECT
public:
    LesionVisualizationExtensionMediator(QObject *parent = 0);

    ~LesionVisualizationExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QLesionVisualizationExtension, LesionVisualizationExtensionMediator> registerLesionVisualizationExtension;

}

#endif
