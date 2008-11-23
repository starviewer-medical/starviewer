/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQCINECONTROLLER_H
#define UDGQCINECONTROLLER_H

#include "ui_qcinecontrollerbase.h"

namespace udg {

class QViewerCINEController;
class QViewer;
/**
Widget per controlar el CINE en els QViewer

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QCINEController : public QWidget, private Ui::QCINEControllerBase
{
Q_OBJECT
public:
    QCINEController(QWidget *parent = 0);

    ~QCINEController();

    void setQViewer( QViewer *viewer );

protected slots:
    void updateVelocityLabel( int value );
    void updateLoopStatus( bool enabled );

protected:
    QViewerCINEController *m_cineController;
};

}

#endif
