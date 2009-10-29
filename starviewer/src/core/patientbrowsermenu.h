/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPATIENTBROWERMENU_H
#define UDGPATIENTBROWERMENU_H

#include <QWidget>

class QPoint;

namespace udg {

class Patient;
class Volume;
class PatientBrowserMenuExtendedItem;
class PatientBrowserMenuList;

/**
    Widget en forma de menú que desplega la informació del pacient que li donem 
    d'entrada de forma que poguem navegar pels seus estudis i corresponents volums

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PatientBrowserMenu : public QWidget
{
Q_OBJECT
public:
    PatientBrowserMenu(QWidget *parent = 0);
    ~PatientBrowserMenu();

    /// Assignem un pacient per representar al menu
    void setPatient(Patient *patient);

public slots:
    /// Fem que es mostri el menú en la posició indicada
    void popup(const QPoint &point, const QString &identifier = QString() );

signals:
    /// Senyal que notifica el volum corresponent a l'ítem escollit
    void selectedVolume(Volume *);

private slots:
    /// Actualitza les vistes relacionades amb l'ítem actiu (aquell pel qual passa el ratolí per sobre)
    void updateActiveItemView(const QString &identifier);

    /// Donat l'identificador de l'ítem fa les accions pertinents. 
    /// En aquest cas s'encarrega d'obtenir el volum seleccionat per l'usuari i notificar-ho
    void processSelectedItem(const QString &identifier);

private:
    /// Actualitza la posició de la informació addicional
    void updatePosition();

private:
    /// Atribut que guarda el punter al menú basic que representa les dades del pacient
    PatientBrowserMenuList *m_patientBrowserList;

    /// Atribut que guarda el punter al menú amb informació addicional de l'ítem seleccionat
    PatientBrowserMenuExtendedItem *m_patientAdditionalInfo;

    /// Pacient que ens proporciona les dades d'entrada
    Patient *m_patient;
};

}

#endif
