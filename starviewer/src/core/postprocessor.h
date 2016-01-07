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

#ifndef POSTPROCESSOR_H
#define POSTPROCESSOR_H

namespace udg {

class Volume;

/**
    Classe base per tots els postprocessadors que s'executen després de la lectura del volum.
 */
class Postprocessor {

public:
    virtual ~Postprocessor();

    /// Aplica el postprocés sobre el volum donat.
    virtual void postprocess(Volume *volume) = 0;

};

} // namespace udg

#endif // POSTPROCESSOR_H
