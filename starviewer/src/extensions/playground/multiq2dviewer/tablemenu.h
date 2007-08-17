/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTABLEMENU_H
#define UDGTABLEMENU_H

#include <QWidget>
#include <QGridLayout>

#include <itemmenu.h>

namespace udg {

/**
Classe per implementar el menu per seleccionar els grids com si es crees una taula.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class TableMenu : public QWidget
{
Q_OBJECT
public:
    TableMenu();

    ~TableMenu();

private:

    /// Nombre de columnes actuals
    int m_columns;
    
    /// Nombre de files actuals
    int m_rows;

    /// Grid per mostrar els elements de la taula
    QGridLayout * m_gridLayout;

    /// Afegir una columna a la taula
    void addColumn();

    /// Afegir una fila a la taula
    void addRow();

public slots:

    /// Mètode que cada vegada que es seleccioni un dels items comprova si cal afegir files o columnes
    void verifySelected( ItemMenu * selected );

};

}

#endif
