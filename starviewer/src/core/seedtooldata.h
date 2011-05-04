#ifndef UDGSEEDTOOLDATA_H
#define UDGSEEDTOOLDATA_H

#include "tooldata.h"

#include <QVector>

namespace udg {

class DrawerPoint;
class Volume;

class SeedToolData : public ToolData {
Q_OBJECT
public:
    SeedToolData(QObject *parent = 0);
    ~SeedToolData();

    void setSeedPosition(QVector<double> pos);

    QVector<double> getSeedPosition();
    DrawerPoint* getPoint();
    void setPoint(DrawerPoint* p = NULL);
    Volume* getVolume();
    void setVolume(Volume* vol);

private:
    /// Hi guardem la posició de la llavor
    QVector<double> m_position;

    /// Punt que es dibuixa
    DrawerPoint *m_point;

    /// Volume que es visualitza
    Volume* m_volume;

};

}

#endif
