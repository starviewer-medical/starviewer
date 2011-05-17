#ifndef UDGQCUSTOMWINDOWLEVELDIALOG_H
#define UDGQCUSTOMWINDOWLEVELDIALOG_H

#include "ui_qcustomwindowleveldialogbase.h"

namespace udg {

// FWD declarations
class WindowLevelPresetsToolData;

/**
Diàleg per escollir manualment els ajustaments pel window level
*/
class QCustomWindowLevelDialog : public QDialog, private Ui::QCustomWindowLevelDialogBase {
Q_OBJECT
public:
    QCustomWindowLevelDialog(QDialog *parent = 0);

    ~QCustomWindowLevelDialog();

public slots:
    /// Dóna el window level que mostrarà per defecte quan s'obri
    void setDefaultWindowLevel(double window, double level);

signals:
    /// Envia la senyal del window level escollit
    void windowLevel(double window, double level);

private:
    /// Crea les connexions
    void createConnections();

private slots:
    /// Comprova el window level quan diem ok, fa les validacions i envia els senyas pertinents
    void confirmWindowLevel();

};

}

#endif
