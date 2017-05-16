/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGTRANSLATETOOL_H
#define UDGTRANSLATETOOL_H

#include "tool.h"

namespace udg {

/**
    Eina per moure la posició de la càmera en escena d'un viewer
  */
class TranslateTool : public Tool {
Q_OBJECT
public:
    enum { None, Translating };
    TranslateTool(QViewer *viewer, QObject *parent = 0);
    ~TranslateTool();

    void handleEvent(unsigned long eventID);

protected slots:
    /// Comença el translate
    void startTranslate();

    /// Calcula el nou translate
    void doTranslate();

    /// Atura l'estat de translate
    void endTranslate();

private:
    /// Realitza la feina de desplaçament
    void pan();

private:
    /// Estat de la tool
    int m_state;
};

}

#endif
