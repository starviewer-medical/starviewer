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


#ifndef UDGQBASICSEARCHWIDGET_H
#define UDGQBASICSEARCHWIDGET_H

#include <ui_qbasicsearchwidgetbase.h>

class QString;

namespace udg {

class DicomMask;

/**
    Classe de cerca bàsica d'elemens
  */
class QBasicSearchWidget : public QWidget, private Ui::QBasicSearchWidgetBase {
Q_OBJECT
public:
    enum DefaultDateFlags { AnyDate, Today, Yesterday, LastWeek };
    QBasicSearchWidget(QWidget *parent = 0);
    ~QBasicSearchWidget();

    /// Neteja els camps de la cerca bàsica
    void clear();

    /// Activa o desactiva poder escollir la modalitat de serie al fer la cerca
    void setEnabledSeriesModality(bool enabled);

    /// Construeix la màscara de cerca
    DicomMask buildDicomMask();

    /// Ens marca les flags de cerca per data per defecte
    void setDefaultDate(DefaultDateFlags flag);

    /// Adds wildcards both front and back to the given string. If the string is empty it's not modified.
    static void addWildCards(QString &string);

private slots:
    /// Checkeig de les dates que canvien els QDateEdit per mantenir consistència
    /// @param date
    void checkFromDate(QDate date);
    void checkToDate(QDate date);

private:
    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Inicialitza els valors de la QBasicSearchWidget
    void initialize();

    /// Construeix la màscara de les dates
    void setStudyDateToDICOMMask(DicomMask *mask);

    /// Neteja el paràmetre de cerca de modalitat de serie
    void clearSeriesModality();

    /// S'executa quan es mostra el widget, fa que la primera vegada que es mostri es doni el focus al lineEdit PatientName
    void showEvent(QShowEvent *event);

private:
    /// Indica si s'ha mostra el widget
    bool m_widgetHasBeenShowed;
};

}

#endif
