/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qchooseoneobjectdialog.h"

#include "logging.h"

#include <QFileInfo>
#include <QDir>

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
        filename.remove( QDir::toNativeSeparators( fileInfo.dir().path() ) );
        newList << filename;
        DEBUG_LOG( "filename=" + filename );
    }

    m_filesList->addItems(newList);
}

QString QChooseOneObjectDialog::getChoosed() const
{
    DEBUG_LOG( "filename choosed = " + m_objectList[m_filesList->currentIndex()] );
    return m_objectList[m_filesList->currentIndex()];
}

}
