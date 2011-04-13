#ifndef UDGEXTENSIONMANAGER_H
#define UDGEXTENSIONMANAGER_H

#include <QObject>

namespace udg {

/**
Classe que es crea a la finestra principal per gestionar les extensions.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ExtensionManager : public QObject
{
Q_OBJECT
public:
    ExtensionManager(QObject *parent = 0);

    ~ExtensionManager();

};

}

#endif
