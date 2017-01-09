/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGGRIDICON_H
#define UDGGRIDICON_H

#include <QFrame>

namespace udg {

/**
    Classe que representa un element del tipus icona per poder crear un menu per escollir una graella de visualitzadors
  */
class GridIcon : public QFrame {
Q_OBJECT
public:
    explicit GridIcon(QWidget* parent = 0);
    explicit GridIcon(const QString &iconType, QWidget *parent = 0);
    ~GridIcon();

    /// Updates the current icon to match the given iconType.
    void setIconType(QString iconType = QString());

};

}

#endif
