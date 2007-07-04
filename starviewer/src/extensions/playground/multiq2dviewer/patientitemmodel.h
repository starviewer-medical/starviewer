/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPATIENTITEMMODEL_H
#define UDGPATIENTITEMMODEL_H

#include <QAbstractItemModel>


namespace udg {

/**
Classe que representa un model del patró Model/View Programming de les Qt's,
per tal de representar en forma d'arbre les dades d'un pacient.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class PatientItemModel : public QAbstractItemModel {
Q_OBJECT
public:

    PatientItemModel( const QString &data, QObject *parent = 0 );

    ~PatientItemModel();

};

};

#endif
