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
    m_progressLabel->setText(tr(" (%1\%)").arg(progress, 3));
}

void QViewerWorkInProgressWidget::showEvent(QShowEvent *event)
{
    m_progressBarAnimation->start();
}

void QViewerWorkInProgressWidget::hideEvent(QHideEvent *event)
{
    m_progressBarAnimation->setPaused(true);
}

void QViewerWorkInProgressWidget::resetProgressWidgets()
{
    m_progressBarLabel->show();
    m_progressLabel->setText("");
}

} // End namespace udg
