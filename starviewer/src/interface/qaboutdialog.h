#ifndef UDGQABOUTDIALOG_H
#define UDGQABOUTDIALOG_H

#include "ui_qaboutdialogbase.h"

namespace udg {

/**
    Diàleg d'about de l'aplicació
  */
class QAboutDialog : public QDialog, private Ui::QAboutDialogBase {
Q_OBJECT
public:
    QAboutDialog(QWidget *parent = 0);
    ~QAboutDialog();
};

}

#endif
