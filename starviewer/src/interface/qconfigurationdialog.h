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

#ifndef UDGQCONFIGURATIONDIALOG_H
#define UDGQCONFIGURATIONDIALOG_H

#include "ui_qconfigurationdialogbase.h"
#include <QDialog>

#include <QMultiMap>

namespace udg {

/**
    Classe que implementa el diàleg de configuració de l'aplicació.
  */
class QConfigurationDialog : public QDialog, private ::Ui::QConfigurationDialogBase {
Q_OBJECT
public:
    QConfigurationDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);

    ~QConfigurationDialog();

protected:
    void closeEvent(QCloseEvent* event);

private slots:
    /// S'encarrega d'amagar/mostrar les opcions de configuració depenent si s'ha seleccionat o no l'opció
    /// de mostrar opcions avançades.
    void setViewAdvancedConfiguration();

private:
    /// Ens serveix per diferenciar entre configuració avançada i configuració bàsica
    enum ConfigurationType { BasicConfiguration, AdvancedConfiguration };

    /// Afegeix un nou widget de configuració al diàleg.
    void addConfigurationWidget(QWidget *widget, const QString &name, ConfigurationType type);

private:
    QMultiMap<ConfigurationType, QListWidgetItem*> m_configurationListItems;
    QList<QWidget*> m_configurationScreenWidgets;
};

}

#endif
