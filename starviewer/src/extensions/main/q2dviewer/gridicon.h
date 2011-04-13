#ifndef UDGGRIDICON_H
#define UDGGRIDICON_H

#include <QFrame>

// FWD declarations
class QLabel;
class QResizeEvent;

namespace udg {

/**
    Classe que representa un element del tipus icona per poder crear un menu per escollir una graella de visualitzadors

	@author Grup de Gràfics de Girona  (GGG) <vismed@ima.udg.es>
*/

class GridIcon : public QFrame {
Q_OBJECT
public:
    GridIcon(QWidget *parent = 0, const QString &iconType = QString());
    ~GridIcon();

protected:
    /// Tractament del resize per mantenir els tamanys de les icones
    void resizeEvent(QResizeEvent *event);

private:
    /// Label on posarem la icona
    QLabel *m_label;
};

}

#endif
