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

#ifndef UDGTOOL_H
#define UDGTOOL_H

#include <QObject>

namespace udg {

class ToolConfiguration;
class ToolData;
class QViewer;
class Q2DViewer;

/**
    Classe base per a totes les tools
  */
class Tool : public QObject {
Q_OBJECT
public:
    Tool(QViewer *viewer, QObject *parent = 0);
    ~Tool();

    /// Li assignem una configuracio, si la que te per defecte no ens val
    /// @param configuration Configuracio que li volem assignar
    virtual void setConfiguration(ToolConfiguration *configuration);

    /// Retorna la configuracio actual de la tool
    /// TODO hauria de ser const o no?
    ToolConfiguration* getConfiguration() const;

    /// Assigna les dades
    /// @param data dades que assignem a la tool
    virtual void setToolData(ToolData *data);

    /// Ens retorna les dades de la tool
    /// @return Dades de la tool
    /// TODO hauria de ser const o no?
    virtual ToolData* getToolData() const;

    /// Ens determina si aquesta tool té dades compartides o no
    /// @return Cert si té dades compartides, fals altrament
    /// TODO podem implementar de dues maneres aquest mètode.
    /// 1.- Cada classe defineix amb la variable m_hasSharedData si té dades compartides o no
    /// 2.- Definim aquest mètode virtual i cada classe el reimplementa retornant el valor que volem
    /// El mètode 1 sembla millor perquè no cal mantenir tantes línies de codi, encara que és més fàcil de tenir
    /// problemes ja que és més fàcil donar valors a la variable per error o no inicialitzar-la correctament
    bool hasSharedData() const;

    /// Ens determina si la tool té dades persistents o no
    bool hasPersistentData() const;

    /// Retorna el nom de la tool
    QString toolName();

    /// Decideix què s'ha de fer per cada event rebut
    virtual void handleEvent(unsigned long eventID) = 0;

protected:
    /// Viewer sobre el que s'executa la tool
    QViewer *m_viewer;

    /// Configuracio de la tool
    ToolConfiguration *m_toolConfiguration;

    /// Dades de la tool
    ToolData *m_toolData;

    /// Indica si les seves dades hauran de ser o no compartides
    bool m_hasSharedData;

    /// Nom de la tool TODO podem fer servir QMetaObject::className()? i ens estalviem aquesta variable?
    QString m_toolName;

    /// Indica si la tool té dades persistents
    bool m_hasPersistentData;
};

}

#endif
