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

signals:
    ///Signal que s'emet quan s'ha canviat la mida del Widget
    void clicked(QDiagnosisTestResultWidget *);

protected:
    /// Comprova si s'ha fet click al frame que mostra la descripció del test, si és així es mostren/s'amaguen la drescripció de l'error del test i la solució
    void mouseReleaseEvent (QMouseEvent * event);

private:

    ///Retorna la icoona que s'ha de mostrar en funció del resultat del test
    QPixmap getIconLabel(const DiagnosisTestResult &diagnosisTestResult) const;

    ///Indica si el click s'ha fet a l'àrea del Frame que mostrar la descripció del test
    bool clickIsInAreaTestDescriptionFrame(QMouseEvent *event);

private:
    bool m_isExpanded;
    bool m_isExpandable;

};

}
#endif 
