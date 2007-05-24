/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qchooseoneobjectdialog.h"

#include <QFileInfo>
#include <QDir>
#include <QDebug>

namespace udg {

QChooseOneObjectDialog::QChooseOneObjectDialog(QWidget *parent)
 : QDialog(parent)
{
    this->setupUi( this );
}


QChooseOneObjectDialog::~QChooseOneObjectDialog()
{
}

void QChooseOneObjectDialog::setObjectsList(const QStringList &list)
{
    m_objectList = list;
    QStringList newList;

    foreach(QString filename, list)
    {
        QFileInfo fileInfo(filename);
        filename.remove( fileInfo.dir().path() );
        newList << filename;
        qDebug() << "filename=" << filename;
    }

    m_filesList->addItems(newList);
}

QString QChooseOneObjectDialog::getChoosed() const
{
    qDebug() << "filename choosed = " << m_objectList[m_filesList->currentIndex()];
    return m_objectList[m_filesList->currentIndex()];
}

}
