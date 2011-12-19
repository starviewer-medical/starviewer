#include "qdiagnosistestresultwidget.h"

#include <QMouseEvent>

#include "diagnosistestresult.h"
#include "diagnosistest.h"

namespace udg {

QDiagnosisTestResultWidget::QDiagnosisTestResultWidget(DiagnosisTest *diagnosisTest, DiagnosisTestResult diagnosisTestResult)
{
    setupUi(this);

    m_diagnosisTestDescription->setText(diagnosisTest->getDescription());
    m_diagnosisTestResultDescription->setText(diagnosisTestResult.getDescription());
    m_diagnosisTestResultSolution->setText(diagnosisTestResult.getSolution());

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

void QDiagnosisTestResultWidget::mouseReleaseEvent(QMouseEvent * event)
{
    if (clickIsInAreaTestDescriptionFrame(event))
    {
        expandContractDiagnosisTestResult();
    }

    event->accept();
}

void QDiagnosisTestResultWidget::expandContractDiagnosisTestResult()
{
    if (!m_isExpandable)
    {
        return;
    }

    m_isExpanded = !m_isExpanded;
    m_descriptionSolutionDiagnosisTestFrame->setVisible(m_isExpanded);
 
    m_expandContractLabel->setPixmap(QPixmap(m_isExpanded ? ":/images/collapse.gif" : ":/images/expand.gif"));

    if (!m_isExpanded)
    {
        this->resize(m_diagnosisTestDescription->size());
    }
    else
    {
        this->adjustSize();
    }

    emit(resized());
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
