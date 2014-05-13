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

#ifndef UDGTOOLCONFIGURATION_H
#define UDGTOOLCONFIGURATION_H

#include <QObject>

#include <QMap>
#include <QVariant>

namespace udg {

/**
    Classe encarregada de definir els atributs que configuren una tool. Els guarda de forma genèrica.
  */
class ToolConfiguration : public QObject {
Q_OBJECT
public:
    ToolConfiguration(QObject *parent = 0);
    ~ToolConfiguration();

    /// Afegeix un atribut a la tool
    void addAttribute(const QString &attributeName, const QVariant &value);

    /// Posa el valor a un atribut
    void setValue(const QString &attributeName, const QVariant &value);

    /// Obté el valor d'un atribut
    QVariant getValue(const QString &attributeName);

    /// Retorna cert si conté l'atribut, fals altrament
    bool containsValue(const QString &attributeName);

private:
    /// Map per guardar els atributs amb els seus corresponents valors
    QMap<QString, QVariant> attributeMap;
};

}

#endif
