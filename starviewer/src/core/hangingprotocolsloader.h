/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGHANGINGPROTOCOLSLOADER_H
#define UDGHANGINGPROTOCOLSLOADER_H

#include <QObject>

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class HangingProtocolsLoader : public QObject
{
Q_OBJECT
public:
    HangingProtocolsLoader( QObject *parent = 0 );

    ~HangingProtocolsLoader();

    /// Càrrega de hanging protocols per defecte
    void loadDefaults();

private:

    /// Crea un hanging protocol per les mamografies del CAC Güell
    void loadMamoGuell();

    /// Crea un hanging protocol per la mamografia del Treta
    void loadMamoTrueta();
};

}

#endif
