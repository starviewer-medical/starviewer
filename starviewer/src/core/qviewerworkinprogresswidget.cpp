#include "qviewerworkinprogresswidget.h"

#include <QMovie>

namespace udg {

QViewerWorkInProgressWidget::QViewerWorkInProgressWidget(QWidget *parent)
    : QWidget(parent)
{
    this->setupUi(this);

    m_progressBarAnimation = new QMovie(this);
    m_progressBarAnimation->setFileName(QString::fromUtf8(":/images/downloading.gif"));
    m_progressBarLabel->setMovie(m_progressBarAnimation);

    this->reset();
}

void QViewerWorkInProgressWidget::reset()
{
    m_progressBarAnimation->stop();
    m_progressLabel->setText("");
    m_headerLabel->setText("");
}

void QViewerWorkInProgressWidget::setTitle(const QString &text)
{
    m_headerLabel->setText(text);
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

} // End namespace udg
