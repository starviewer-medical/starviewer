/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQCUSTOMWINDOWLEVELDIALOG_H
#define UDGQCUSTOMWINDOWLEVELDIALOG_H

#include "ui_qcustomwindowleveldialogbase.h"

namespace udg {

/**
Diàleg per escollir manualment els ajustaments pel window level

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QCustomWindowLevelDialog : public QDialog , private Ui::QCustomWindowLevelDialogBase
{
Q_OBJECT
public:
    QCustomWindowLevelDialog( QDialog *parent = 0 );

    ~QCustomWindowLevelDialog();

public slots:
    /// dóna el window level que mostrarà per defecte quan s'obri
    void setDefaultWindowLevel( double window , double level );
private:
    /// Crea les connexions 
    void createConnections();

private slots:
    /// Comprova el window level quan diem ok, fa les validacions i envia els senyas pertinents
    void confirmWindowLevel();

signals:
    /// envia la senyal del window level escollit
    void windowLevel( double window , double level );

};

}

#endif
