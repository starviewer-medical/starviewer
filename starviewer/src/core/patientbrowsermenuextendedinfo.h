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

#ifndef UDGPATIENTBROWSERMENUEXTENDEDINFO_H
#define UDGPATIENTBROWSERMENUEXTENDEDINFO_H

#include <QFrame>

class QVBoxLayout;

namespace udg {

class PatientBrowserMenuExtendedItem;

/**
    Class to show additional information for the patient browser menu.
  */
class PatientBrowserMenuExtendedInfo : public QFrame {
Q_OBJECT
public:
    PatientBrowserMenuExtendedInfo(QWidget *parent = 0);
    ~PatientBrowserMenuExtendedInfo();

    /// Event handler
    bool event(QEvent *event);

    /// Sets the items to be shown
    void setItems(const QList<PatientBrowserMenuExtendedItem *> &items);

signals:
    /// Signal emitted when a close event is received
    void closed();

private:
    /// Helper method to delete all layout's children
    void clearLayout(QLayout *layout, bool deleteWidgets = true);

private:
    /// Layout
    QVBoxLayout *m_layout;
};

}

#endif
