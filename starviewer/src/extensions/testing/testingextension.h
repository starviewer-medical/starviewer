/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef TESTINGEXTENSION_H
#define TESTINGEXTENSION_H

#include "ui_testingextension.h"

namespace udg{

class TestingExtension: public QWidget, private Ui::TestingExtension {
Q_OBJECT
public:
    TestingExtension(QWidget *parent = 0);
};

} // namespace udg
#endif
