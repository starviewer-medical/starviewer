#include "qdiagnosistestresultwidget.h"

#include <QMouseEvent>

#include "diagnosistestresult.h"
#include "diagnosistest.h"

namespace udg {

QDiagnosisTestResultWidget::QDiagnosisTestResultWidget(DiagnosisTest *diagnosisTest, DiagnosisTestResult diagnosisTestResult)
{
    setupUi(this);

    m_diagnosisTestDescription->setText(diagnosisTest->getDescription());
    m_diagnosisTestResultDescription->setText(QString("<b>Description: </b> %1").arg(diagnosisTestResult.getDescription().replace('\n',"<br>")));
    m_diagnosisTestResultSolution->setText(QString("<b>Solution: </b> %1").arg( diagnosisTestResult.getSolution().replace('\n',"<br>")));

    m_iconDiagnosticResultLabel->setPixmap(getIconLabel(diagnosisTestResult));
    
    m_isExpandable = diagnosisTestResult.getState() != DiagnosisTestResult::Ok;
    m_expandContractLabel->setVisible(m_isExpandable);

    if (m_isExpandable)
    {
        m_testDescriptionFrame->setCursor(QCursor(Qt::PointingHandCursor));
    }

    m_isExpanded = false;
    m_descriptionSolutionDiagnosisTestFrame->setVisible(false);
}

void QDiagnosisTestResultWidget::expand()
{
    if (m_isExpandable)
    {
        m_descriptionSolutionDiagnosisTestFrame->setVisible(true);
        m_expandContractLabel->setPixmap(QPixmap(":/images/collapse.png"));

        this->adjustSize();

        m_isExpanded = true;

    }
}

bool QDiagnosisTestResultWidget::isExpandable()
{
    return m_isExpandable;
}

void QDiagnosisTestResultWidget::contract()
{
    m_descriptionSolutionDiagnosisTestFrame->setVisible(false);
    m_expandContractLabel->setPixmap(QPixmap(":/images/expand.png"));

    this->resize(m_diagnosisTestDescription->size());

    m_isExpanded = false;
}

void QDiagnosisTestResultWidget::mouseReleaseEvent(QMouseEvent * event)
{
    emit(clicked(this));
    event->accept();
}

QPixmap QDiagnosisTestResultWidget::getIconLabel(const DiagnosisTestResult &diagnosisTestResult) const
{
    QString iconPath;

    switch (diagnosisTestResult.getState())
    {
        case DiagnosisTestResult::Ok:
            iconPath = ":/images/button_ok.png";
            break;
        case DiagnosisTestResult::Warning:
            iconPath = ":/images/small-warning.png";
            break;
        case DiagnosisTestResult::Error:
        case DiagnosisTestResult::Invalid:
        default:
            iconPath = ":/images/cancel.png";
            break;
    }

    return  QPixmap(iconPath);
}

bool QDiagnosisTestResultWidget::clickIsInAreaTestDescriptionFrame(QMouseEvent *event)
{
    return m_testDescriptionFrame->x() <= event->x() && event->x() <= (m_testDescriptionFrame->x() + m_testDescriptionFrame->width()) &&
        m_testDescriptionFrame->y() <= event->y() && event->y() < (m_testDescriptionFrame->y() + m_testDescriptionFrame->height());
}

}
