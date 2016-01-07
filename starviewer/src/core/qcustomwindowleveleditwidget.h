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

#ifndef UDGQCUSTOMWINDOWLEVELEDITWIDGET_H
#define UDGQCUSTOMWINDOWLEVELEDITWIDGET_H

#include "ui_qcustomwindowleveleditwidgetbase.h"

#include "windowlevel.h"

namespace udg {

/// Widget que permet afegir, modificar i eliminar els WW/WL definits per l'usuari.
/// Un cop acabat s'actualitza el CustomWindowLevelRepository i es reescriu el fitxer. S'hauria de discutir
/// si és aquesta classe l'encarregada de modificar el fitxer.
class QCustomWindowLevelEditWidget : public QDialog, private Ui::QCustomWindowLevelEditWidgetBase {
Q_OBJECT
public:
    QCustomWindowLevelEditWidget(QWidget *parent = 0);
    ~QCustomWindowLevelEditWidget();

    /// Assigna els valors per defecte que es posarant quan s'afegeixi un custom
    void setDefaultWindowLevel(const WindowLevel &windowLevel);

private:
    /// Crea les connexions internes
    void createConnections();

    /// Colca els CustomWindowLevels del repository a la taula
    void loadCustomWindowLevelPresets();

    /// Valida la informació de la taula abans d'actualitzar el repository
    bool validate() const;

    /// Afegeix una nova fila a la taula amb els valors passats per paràmetre
    QTreeWidgetItem* addWindowLevelItem(const WindowLevel &windowLevel);

private slots:
    /// Afegeix una nova fila assignant els valors per defecte guardats als atributs m_defaultWindowWidth i m_defaultWindowLevel
    void addNewWindowLevel();

    /// Elimina les files que estan seleccionades
    void removeWindowLevel();

    /// Actualitza el CustomWindowLevelsRepository si la informació és correcte (no hi ha descripcions repetides ni buides)
    void updatePresetsIfAreValid();

private:
    /// WW/WL per defecte que es posaran al afegir un nova fila
    WindowLevel m_defaultWindowLevel;
};

}

#endif
