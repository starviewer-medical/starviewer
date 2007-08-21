/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQREGISTRATIONFRAMEWORK_H
#define UDGQREGISTRATIONFRAMEWORK_H

#include "ui_qregistrationframeworkextensionbase.h"

namespace udg {

class Volume;

/**
    TODO Descripció de l'extensió
	@author Grup de GrÀics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QRegistrationFrameworkExtension : public QWidget , private Ui::QRegistrationFrameworkExtensionBase {
Q_OBJECT
public:
    QRegistrationFrameworkExtension( QWidget *parent = 0 );

    ~QRegistrationFrameworkExtension();

    /**
     * Li donem el volum d'entrada->pendent d'implementació
     * @param input El volum principal
     */
    void setInput( Volume *input ){}
};

} // end namespace udg

#endif
