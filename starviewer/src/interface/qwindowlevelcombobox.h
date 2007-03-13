/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQWINDOWLEVELCOMBOBOX_H
#define UDGQWINDOWLEVELCOMBOBOX_H

#include <QComboBox>

namespace udg {

// FWD declarations
class QCustomWindowLevelDialog;

/**
Combo box personalitzat per a escollir valors de window level predeterminats i personalitzats

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QWindowLevelComboBox : public QComboBox
{
Q_OBJECT
public:
    QWindowLevelComboBox(QWidget *parent = 0);

    ~QWindowLevelComboBox();

    /// afegeix un window level
    void insertWindowLevelPreset( double window, double level, int position, QString description );

signals:
    /// Emet els valors de window level escollits
    void windowLevel( double window , double level );

    /// S'emet quan el valor a posar a de ser el que ens doni l'imatge per defecte
    void defaultValue();

public slots:
    /// Actualitza el window level i emet senyal
    void updateWindowLevel( double window , double level );

private:
    /// Diàleg per escollir un window level personalitzat
    QCustomWindowLevelDialog *m_customWindowLevelDialog;

    /// Omple el combo
    void populate();

    /// Crea les connexions
    void createConnections();

    /// Llista de valors de window Level
    std::vector<double *> m_windowLevelArray;

private slots:
    /// Seleccionem el window level que volem aplicar com a actiu
    void setActiveWindowLevel( int value );

};

}

#endif
