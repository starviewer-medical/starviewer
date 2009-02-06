/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qpopuprisrequestsscreen.h"

#include <QTimer>

#include "operation.h"
#include "starviewersettings.h"
#include "logging.h"

namespace udg {

QPopUpRisRequestsScreen::QPopUpRisRequestsScreen( QWidget *parent ): QDialog( parent )
{
    setupUi(this);
    this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    m_qTimer = new QTimer();
    connect(m_qTimer,SIGNAL(timeout()),SLOT(timeoutTimer()));
}


void QPopUpRisRequestsScreen::setAccessionNumber(QString accessionNumber)
{
    m_labelRisRequestDescription->setText(m_labelRisRequestDescription->text() + " " + accessionNumber);
}

void QPopUpRisRequestsScreen::showEvent(QShowEvent * )
{
    m_qTimer->start(5000);
}

void QPopUpRisRequestsScreen::timeoutTimer()
{
    this->setVisible(false);
}

QPopUpRisRequestsScreen::~QPopUpRisRequestsScreen()
{
    m_qTimer->stop();
}

};
