
#ifndef UDGQADVANCEDSEARCHWIDGET_H
#define UDGQADVANCEDSEARCHWIDGET_H

#include <ui_qadvancedsearchwidgetbase.h>

class QString;

namespace udg {

class DicomMask;

/** Classe de cerca bàsica d'elemens
*/
class QAdvancedSearchWidget : public QWidget, private Ui::QAdvancedSearchWidgetBase {
Q_OBJECT
public:
    QAdvancedSearchWidget(QWidget *parent = 0);
    ~QAdvancedSearchWidget();

    /// Neteja els camps de la cerca bàsica
    void clear();
    /// Construeix la màscara de cerca
    DicomMask buildDicomMask();

private slots:

    /// Posa un * al tab el qual pertany el lineedit que se li ha donat valor, per a que l'usuari sàpiga quin tabs ha posat valors de cerca
    void updateAdvancedSearchModifiedStatus();

private:

    /// Crea les connexions entre signals i slots
    void createConnections();
};

}

#endif
