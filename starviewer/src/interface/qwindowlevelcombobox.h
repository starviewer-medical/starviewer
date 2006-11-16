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

    /// valors del window level
    double m_window , m_level;
    
private slots:
    /// Li passarem l'índex de l'ítem del combo que s'hagi seleccionat per a fer el que calgui amb la selecció
    void processSelected( int value );
    
};

}

#endif
