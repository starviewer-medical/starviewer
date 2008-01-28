/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qcinecontroller.h"
#include "qviewercinecontroller.h"

#include <QToolButton>

namespace udg {

QCINEController::QCINEController(QWidget *parent)
 : QWidget(parent), m_cineController(0)
{
    setupUi(this);
}

QCINEController::~QCINEController()
{
}

void QCINEController::setQViewer( QViewer *viewer )
{
    if( !m_cineController )
    {
        m_cineController = new QViewerCINEController();

        connect( m_loopCheckBox, SIGNAL( toggled(bool) ), m_cineController ,SLOT( enableLoop(bool) ) );
        connect( m_boomerangCheckBox, SIGNAL( toggled(bool) ), m_cineController, SLOT( enableBoomerang(bool) ) );
        connect( m_loopCheckBox, SIGNAL( toggled(bool) ), m_boomerangCheckBox, SLOT( setEnabled(bool) ) );

        m_playToolButton->setDefaultAction( m_cineController->getPlayAction() );

        connect( m_velocityControl, SIGNAL( valueChanged(int) ), m_cineController, SLOT( setVelocity(int) ) );
        connect( m_cineController, SIGNAL( velocityChanged(int) ), SLOT( updateVelocityLabel(int) ) );
        connect( m_cineController, SIGNAL( velocityChanged(int) ), m_velocityControl, SLOT( setValue(int) ) );
        m_cineController->setInputViewer( viewer );
    }
    else
        m_cineController->setInputViewer( viewer );
}

void QCINEController::updateVelocityLabel( int value )
{
    m_velocityLabel->setText( tr("%1 img/s").arg( value ) );
}

void QCINEController::updateLoopStatus( bool enabled )
{
    m_boomerangCheckBox->setEnabled( enabled );
    m_cineController->enableLoop( enabled );
    m_cineController->enableBoomerang( m_boomerangCheckBox->isChecked() );
}

}
