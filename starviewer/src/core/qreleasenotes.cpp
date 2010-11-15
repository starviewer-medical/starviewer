/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qreleasenotes.h"

#include <QCloseEvent>
#include <QUrl>

namespace udg {

QReleaseNotes::QReleaseNotes(QWidget *parent) 
 : QWidget(parent)
{
    setupUi(this);

    //no cal fer un metode a part per les connexions si només en tenim una
    connect(m_closePushButton, SIGNAL(clicked()), this, SLOT(close()));

    //Fer que la finestra sempre quedi davant i no es pugui fer res fins que no es tanqui
    setWindowModality(Qt::ApplicationModal);
}

QReleaseNotes::~QReleaseNotes()
{
}

bool QReleaseNotes::isDontShowAnymoreChecked()
{
    return m_dontShowCheckBox->isChecked();
}

void QReleaseNotes::setDontShowVisible(bool visible)
{
    m_dontShowCheckBox->setVisible(visible);
}

void QReleaseNotes::setUrl(QUrl url)
{
    m_viewWebView->setUrl(url);
}

void QReleaseNotes::closeEvent(QCloseEvent *event)
{
    //llençar el senyal de que es tanca la finestra
    closing();
    //i tancar la finestra
    event->accept();
}

}; // end namespace udg