/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientitemmodel.h"

#include <QAction>

namespace udg {

PatientItemModel::PatientItemModel( const QString &data, QObject *parent )
 : QAbstractItemModel ( parent )
{

}

PatientItemModel::~PatientItemModel()
{
}


}
