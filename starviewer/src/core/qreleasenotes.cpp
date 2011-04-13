#include "qreleasenotes.h"
#include "coresettings.h"

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

void QReleaseNotes::setUrl(const QUrl &url)
{
    m_viewWebView->setUrl(url);
}

void QReleaseNotes::closeEvent(QCloseEvent *event)
{
    Settings settings;
    //si el 'Don't show on future releases' esta marcat
    if (isDontShowAnymoreChecked())
    {
        //modificar els settings per que no es mostrin mai més  
        settings.setValue(CoreSettings::NeverShowNewVersionReleaseNotes, true);
    }
    //i tancar la finestra
    event->accept();
}

}; // end namespace udg