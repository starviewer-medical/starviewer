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

#ifndef UDGPATIENTBROWSERMENUBASICITEM_H
#define UDGPATIENTBROWSERMENUBASICITEM_H

#include <QLabel>

namespace udg {

/**
    Object that represents an item of the browser menu
  */
class PatientBrowserMenuBasicItem : public QObject {

    Q_OBJECT
    Q_PROPERTY(QString text READ getText WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString identifier READ getIdentifier WRITE setIdentifier NOTIFY identifierChanged)
    Q_PROPERTY(QString type READ getType)

public:
    PatientBrowserMenuBasicItem(QObject *parent = 0);
    ~PatientBrowserMenuBasicItem(){}

    void setText(const QString &text);
    QString getText();

    void setIdentifier(const QString &identifier);
    QString getIdentifier() const;

    virtual QString getType();

signals:
    void textChanged();
    void identifierChanged();

protected:
    /// Identificador de l'ítem
    QString m_identifier;
    QString m_text;
};

}

#endif
