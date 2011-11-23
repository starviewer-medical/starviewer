#ifndef UDGAUTOMATICSYNCHRONIZATIONMANAGER_H
#define UDGAUTOMATICSYNCHRONIZATIONMANAGER_H

#include "tool.h"

namespace udg {

class AutomaticSynchronizationToolData;
class ViewersLayout;

/**
    Tool de sincronització automatica entre visualitzadors.
*/

class AutomaticSynchronizationManager : public QObject {
Q_OBJECT

public:

    /// Constructor i destructor
    AutomaticSynchronizationManager(AutomaticSynchronizationToolData *toolData, ViewersLayout *layout, QObject *parent = 0, QObject *parent);
    ~AutomaticSynchronizationManager();

private:

    AutomaticSynchronizationToolData * m_toolData;
    ViewersLayout * m_viewersLayout;

};

}

#endif
