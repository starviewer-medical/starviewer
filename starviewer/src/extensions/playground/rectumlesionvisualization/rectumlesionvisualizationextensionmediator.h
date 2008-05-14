/**************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGRECTUMLESIONVISUALIZATIONEXTENSIONMEDIATOR_H
#define UDGRECTUMLESIONVISUALIZATIONEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qrectumlesionvisualizationextension.h"

namespace udg {

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class RectumLesionVisualizationExtensionMediator : public ExtensionMediator
{
    Q_OBJECT
public:
    RectumLesionVisualizationExtensionMediator(QObject *parent = 0);

    ~RectumLesionVisualizationExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QRectumLesionVisualizationExtension, RectumLesionVisualizationExtensionMediator> registerRectumLesionVisualizationExtension;

}

#endif
