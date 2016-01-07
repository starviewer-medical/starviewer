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

#ifndef UDGPATIENTBROWSERMENUEXTENDEDITEM_H
#define UDGPATIENTBROWSERMENUEXTENDEDITEM_H

#include <QWidget>
#include <QLabel>

namespace udg {

/**
    Classe per mostrar la informació addicional referent a una serie d'un estudi d'un pacient.
  */
class PatientBrowserMenuExtendedItem : public QFrame {
Q_OBJECT
public:
    PatientBrowserMenuExtendedItem(QWidget *parent = 0);
    ~PatientBrowserMenuExtendedItem();

    void setPixmap(const QPixmap &pixmap);
    void setText(const QString &text);

private:
    /// Crea el widget inicial sense cap informació
    void createInitialWidget();

private:
    /// Label que conté el text addicional a mostrar
    QLabel *m_text;

    /// Label que conté la icona a mostrar
    QLabel *m_icon;
};

}

#endif
