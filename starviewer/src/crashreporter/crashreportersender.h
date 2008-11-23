/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGQCRASHREPORTERSENDER_H
#define UDGQCRASHREPORTERSENDER_H


#include <QString>
#include <QHash>

namespace udg {

/** Aquesta classe és l'encarregada de fer l'enviament del report al servidor socorro. Hi ha una implementació diferent per cada plataforma 
  * degut a que el Breakpad oferia una signatura diferent per a cada plataforma.
  */
class CrashReporterSender {
       
public:
    static bool sendReport(QString url, QString minidumpPath, QHash<QString,QString> &options);
    
};
    
};

#endif