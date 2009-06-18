/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qpopuprisrequestsscreen.h"

#include <QTimer>
#include <QDesktopWidget>
#include <QRect>
#include <QApplication>

#include "qpopuprisrequestsscreen.h"
#include "operation.h"
#include "logging.h"
#include "starviewerapplication.h"

namespace udg {

QPopUpRisRequestsScreen::QPopUpRisRequestsScreen( QWidget *parent ): QDialog( parent )
{
    setupUi(this);
	this->setWindowFlags(Qt::SubWindow | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    m_qTimer = new QTimer();
    connect(m_qTimer,SIGNAL(timeout()),SLOT(timeoutTimer()));
}


void QPopUpRisRequestsScreen::setAccessionNumber(QString accessionNumber)
{
    QString popUpText = tr("%1 has received a request from a RIS to retrieve the study with accession number").arg(ApplicationNameString);
    m_labelRisRequestDescription->setText(popUpText + " " + accessionNumber + ".");
}

void QPopUpRisRequestsScreen::showEvent(QShowEvent * )
{
	//Es situa el PopUp a baix a l'esquerre de la pantalla on està la interfície activa del Starviewer
	QDesktopWidget desktopWidget;

	QRect screenGeometryActiveWindow = desktopWidget.availableGeometry(QApplication::activeWindow()); //Agafem les dimensions de la pantalla on està la finestra activa de l'starviewer
    
	this->move(screenGeometryActiveWindow.x() + screenGeometryActiveWindow.width() - this->width() - 10, screenGeometryActiveWindow.y() + screenGeometryActiveWindow.height() - this->height() -10);

	//Activem el timer per amagar el Popup 5 segons després de fer-lo apareixer
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
