/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQCONFIGURATIONDIALOG_H
#define UDGQCONFIGURATIONDIALOG_H

#include "ui_qconfigurationdialogbase.h"
#include <QDialog>

#include <QMultiMap>

namespace udg {

/**
    Classe que implementa el diàleg de configuració de l'aplicació.
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QConfigurationDialog : public QDialog, private ::Ui::QConfigurationDialogBase
{
Q_OBJECT
public:
    QConfigurationDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);

    ~QConfigurationDialog();
signals:
    /// S'emet quan es canvia alguna opció indicant com a paràmetres quina secció s'ha canviat i informació útil per qui el rep
    //TODO Semi-xapussa per resoldre el problema de comunicació entre el queryscreen i la seva configuració. Cal fer un disseny de tot això.
    //Veure el mètode de extensionhandler updateConfiguration
    void configurationChanged(const QString &section);
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
};

}

#endif
