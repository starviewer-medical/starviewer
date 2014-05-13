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

#ifndef UDG_PATIENTBROWSERMENUGROUP_H
#define UDG_PATIENTBROWSERMENUGROUP_H

#include <QObject>
#include <QStringList>
#include "patientbrowsermenubasicitem.h"

namespace udg {

/// Object that represents a group of basic items
class PatientBrowserMenuGroup : public QObject
{
    Q_OBJECT
    typedef QPair<QString, QString> Blabla;
    Q_PROPERTY(QString caption READ getCaption WRITE setCaption NOTIFY captionChanged)
    Q_PROPERTY(QList<QObject*> elements READ getElementsAsQObject NOTIFY elementsChanged)
    Q_PROPERTY(QList<QObject*> fusionElements READ getFusionElementsAsQObject NOTIFY fusionElementsChanged)
public:
    explicit PatientBrowserMenuGroup(QObject *parent = 0);
    
    /// Sets and gets the caption of the group
    void setCaption(const QString &caption);
    QString getCaption() const;

    /// Sets and gets the items
    void setElements(const QList<PatientBrowserMenuBasicItem*> &elements);
    QList<PatientBrowserMenuBasicItem *> getElements() const;
    QList<QObject*> getElementsAsQObject() const;

    /// Sets and gets the fusion items
    void setFusionElements(const QList<PatientBrowserMenuBasicItem*> &elements);
    QList<PatientBrowserMenuBasicItem*> getFusionElements() const;
    QList<QObject*> getFusionElementsAsQObject() const;

signals:
    void captionChanged();
    void elementsChanged();
    void fusionElementsChanged();

private:
    /// Caption of the group
    QString m_caption;

    /// List of items
    QList<PatientBrowserMenuBasicItem*> m_elements;

    /// List of fusion items
    QList<PatientBrowserMenuBasicItem*> m_fusionElements;
};

} // namespace udg

#endif // UDG_PATIENTBROWSERMENUGROUP_H
