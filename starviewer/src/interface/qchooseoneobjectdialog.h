/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCHOOSEONEDIALOG_H
#define UDGCHOOSEONEDIALOG_H

#include "ui_qchooseoneobjectdialogbase.h"

using namespace Ui; //\TODO S'hauria de veure perquè s'ha de posar això per fer herència!!

namespace udg {

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QChooseOneObjectDialog : public QDialog, private QChooseOneObjectDialogBase
{
Q_OBJECT
public:
    QChooseOneObjectDialog(QWidget *parent = 0);

    ~QChooseOneObjectDialog();

    void setObjectsList(const QStringList &list);

    /// Retorna el nom de l'objecte escollit
    QString getChoosed() const;
private:
    QStringList m_objectList;
};

}

#endif
