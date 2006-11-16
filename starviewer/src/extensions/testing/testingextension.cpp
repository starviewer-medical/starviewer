/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "testingextension.h"

#include <QDebug>

namespace udg{

TestingExtension::TestingExtension(QWidget *parent)
    :QWidget(parent)
{
    setupUi(this);
    qDebug() << "TestingExtension::TestingExtension: Eii, que em creooo!!";  
}

}
