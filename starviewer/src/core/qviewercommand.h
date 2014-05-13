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

#ifndef UDGQVIEWERCOMMAND_H
#define UDGQVIEWERCOMMAND_H

#include <QObject>

namespace udg {

/**
    Classe que segueix el patró Command i és la classe base de tots els Command dels QViewer.
    Serveix per encapsular comandes que s'han de fer sobre un viewer.

    Per implementar un command simplement cal heretar d'aquesta classe i implementar el mètode execute().
    Per evitar-ne un mal ús caldrà que les classes que heretin d'aquesta implementin un constructor amb tots
    els paràmetres necessaris per tal de cridar el mètode execute()
  */

class QViewerCommand : public QObject {
Q_OBJECT
public:
    virtual ~QViewerCommand();

public slots:
    virtual void execute() = 0;

protected:
    /// Fem que el constructor no es pugui cridar si no és heretant per obligar a tenir constructors
    /// que obliguin a passar els paràmetres necessàries pel execute().
    QViewerCommand(QObject *parent = 0);
};

} // End namespace udg

#endif // UDGQVIEWERCOMMAND_H
