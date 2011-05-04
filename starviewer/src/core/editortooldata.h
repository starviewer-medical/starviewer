#ifndef UDGEDITORTOOLDATA_H
#define UDGEDITORTOOLDATA_H

#include "tooldata.h"

namespace udg {

class EditorToolData : public ToolData {
Q_OBJECT
public:
    EditorToolData(QObject *parent = 0);
    ~EditorToolData();

    void setVolumeVoxels(int vol);

    int getVolumeVoxels();

private:
    /// Hi guardem la posició de la llavor
    int m_volumeCont;

};

}

#endif
