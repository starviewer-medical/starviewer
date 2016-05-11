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

#ifndef UDGPATIENTBROWSERMENULIST_H
#define UDGPATIENTBROWSERMENULIST_H

#include <QWidget>
#include <QKeyEvent>

class QGridLayout;
class QQuickWidget;

namespace udg {

class PatientBrowserMenuGroup;
class PatientBrowserMenuBasicItem;

/**
    Widget que representa el contingut del menú en un llistat d'ítems
    Aquests ítems s'organitzaran en files i columnes dins d'un grup al que li donarem un "caption"
  */
class PatientBrowserMenuList : public QWidget {
Q_OBJECT

public:
    PatientBrowserMenuList(QWidget *parent = 0);
    ~PatientBrowserMenuList();

    /// Afegeix un grup d'ítems amb le caption donat
    void addItemsGroup(const QString &caption, const QList<QPair<QString, QString> > &itemsList);

    /// Marquem com a seleccionat l'ítem que tingui l'identificador donat
    void markItem(const QString &identifier);

    /// Retorna l'identificador de l'ítem que tinguem marcat com a seleccionat
    QString getMarkedItem() const;

    /// Tractament dels events
    bool event(QEvent *event);

    /// Sets the maximum height of the widget
    void setMaximumHeight(int maxh);

    /// Sets the width of the widget
    void setMaximumWidth(int width);

	virtual QSize sizeHint() const;
signals:
    /// Senyal que s'emet quan el mouse entra en el widget i ens notifica el seu identificador
    void isActive(QString);

    /// Senyal que s'emet quan no hi ha cap item activat
    void isNotActive();

    /// Aquest senyal s'emet quan s'escull una ítem i ens notifica el seu identificador
    void selectedItem(QString);

    /// Signal que s'emet quan es rep l'event de tancar el menu
    void closed();

private slots:
    void updateSize();

private:
    /// Ítem que tenim marcat
    QString m_markedItem;

    /// List of item groups of the series
    QList<QObject*> m_groups;

    /// Patient browser menu list based on qml
    QQuickWidget *m_qmlView;
};

}

#endif
