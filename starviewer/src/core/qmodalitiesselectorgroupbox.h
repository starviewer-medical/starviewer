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

#ifndef UDGQMODALITIESSELECTORGROUPBOX_H
#define UDGQMODALITIESSELECTORGROUPBOX_H

#include "ui_qmodalitiesselectorgroupboxbase.h"

class QButtonGroup;

namespace udg {

/**
    Combo box per seleccionar modalitats
 */
class QModalitiesSelectorGroupBox : public QGroupBox, private Ui::QModalitiesSelectorGroupBoxBase {
Q_OBJECT
public:
    QModalitiesSelectorGroupBox(QWidget *parent = 0);
    ~QModalitiesSelectorGroupBox();

    /// Permet escollir quines opcions adicionals estan disponibles. Per defecte totes ho estan.
    void enableAllModalitiesCheckBox(bool enable);
    void enableOtherModalitiesCheckBox(bool enable);
    
    /// Desmarca tots els check box
    void clear();
    
    /// Indica si podem seleccionar una sola modalitat o múltiples
    void setExclusive(bool exlusive);

    /// Ens retorna una llista amb les modalitats seleccionades
    QStringList getCheckedModalities();

    /// Selecciona les modalitats de la llista. Si la modalitat no és vàlida no marca res.
    void checkModalities(const QStringList &modalities);

    /// Fa que el check box especial "All" es marqui o desmarqui
    void setAllModalitiesCheckBoxChecked(bool checked);

    /// Ens indica si el check box de "All" està marcat o no, independentment de si està habilitat o no
    bool isAllModalitiesCheckBoxChecked() const;

signals:
    /// Emitted when the checked modalities have changed when they have been clicked, by the user or programmatically
    void checkedModalitiesChanged(const QStringList &checkedModalities);

private:
    void initialize();

private slots:
    /// Called when a check box has been clicked
    void onCheckBoxClicked();

private:
    /// Grup per poder fer que els check box siguin exclusius o no
    QButtonGroup *m_buttonGroup;
};

} // End namespace udg

#endif
