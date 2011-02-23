#include "qviewerworkinprogresswidget.h"

#include "applicationstylehelper.h"

#include <QMovie>

namespace udg {

QViewerWorkInProgressWidget::QViewerWorkInProgressWidget(QWidget *parent)
    : QWidget(parent)
{
    this->setupUi(this);

    m_progressBarAnimation = new QMovie(this);
    m_progressBarAnimation->setFileName(":/images/downloading.gif");
    m_progressBarLabel->setMovie(m_progressBarAnimation);

    ApplicationStyleHelper styleHelper;
    styleHelper.setScaledSizeTo(m_progressBarAnimation);
    styleHelper.setScaledFontSizeTo(this);

    this->reset();
}

void QViewerWorkInProgressWidget::reset()
{
    this->resetProgressWidgets();
    m_headerLabel->setText("");
    m_errorLabel->setText("");
}

void QViewerWorkInProgressWidget::setTitle(const QString &text)
{
    m_headerLabel->setText(text);
}

void QViewerWorkInProgressWidget::showError(const QString &errorText)
{
    this->resetProgressWidgets();
    m_progressBarLabel->hide();
    m_errorLabel->setText(errorText);
}

void QViewerWorkInProgressWidget::updateProgress(int progress)
{
    this->startAnimationByProgress(progress);
    m_progressLabel->setText(tr(" (%1\%)").arg(progress, 3));
}

void QViewerWorkInProgressWidget::startAnimationByProgress(int progress)
{
    // Per evitar que l'animació consumeixi recursos quan no és necessita, només l'activem quan s'està realment fent alguna cosa.
    if (progress >= 0 && progress < 100)
    {
        // No cal controlar si ja està running o no, ho fa la mateixa crida
        m_progressBarAnimation->start();
    }
    else
    {
        m_progressBarAnimation->stop();
    }
}

void QViewerWorkInProgressWidget::resetProgressWidgets()
{
    m_progressBarLabel->show();
    m_progressBarAnimation->stop();
    m_progressLabel->setText("");
}

} // End namespace udg
