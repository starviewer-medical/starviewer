#ifndef UDGQDIAGNOSISTESTRESULTWIDGET_H
#define UDGQDIAGNOSISTESTRESULTWIDGET_H

#include "ui_qdiagnosistestresultwidgetbase.h"

#include <QWidget>

class QMouseEvent;

namespace udg {

class DiagnosisTest;
class DiagnosisTestResult;

/**
    Aquest widget ens mostrar un objecte DiagnosisTest amb el seu resultat DiagnosisTestResult
  */

class QDiagnosisTestResultWidget : public QWidget, private Ui::QDiagnosisTestResultWidgetBase {
Q_OBJECT
public:
    QDiagnosisTestResultWidget(DiagnosisTest *diagnosisTest, DiagnosisTestResult diagnosisTestResult);

    /// Expandeix el contingut
    void expand();

    /// Contrau el contingut
    void contract();

    /// Indica si es pot expandir. Es poden expandir tots excepte els que mostra un DiagnosisTestResult que no ha fallat
    bool isExpandable();

    // Assignar la mida del widget pare per poder calcular la mida correcte dels labels amb wordwrap
    void setParentWidgetWidth(int parentWidgetWidth);

signals:
    ///Signal que s'emet quan s'ha canviat la mida del Widget
    void clicked(QDiagnosisTestResultWidget *);

protected:
    /// Comprova si s'ha fet click al frame que mostra la descripció del test, si és així es mostren/s'amaguen la drescripció de l'error del test i la solució
    void mouseReleaseEvent (QMouseEvent * event);
    /// Retorna la mida adequada del widget
    QSize sizeHint() const;

private:

    ///Retorna la icoona que s'ha de mostrar en funció del resultat del test
    QPixmap getIconLabel(const DiagnosisTestResult &diagnosisTestResult) const;

    ///Indica si el click s'ha fet a l'àrea del Frame que mostrar la descripció del test
    bool clickIsInAreaTestDescriptionFrame(QMouseEvent *event);

    /// Calcula quantes línies de text ocupa un QString en un QLabel amb word wrap
    int countNumberOfLinesWithWordWrap(QTextEdit *textEdit) const;

private:
    bool m_isExpanded;
    bool m_isExpandable;

    /// Nombre de línies que ocupa la descripció del resultat del test en un label amb word wrap
    int m_diagnosisTestResultDescriptionLines;
    /// Nombre de línies que ocupa la solució del resultat del test en un label amb word wrap
    int m_diagnosisTestResultSolutionLines;

    // Mida del widget pare
    int m_parentWidgetWidth;
};

}
#endif 
