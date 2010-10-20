/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPOLYLINEROITOOLREPRESENTATION_H
#define UDGPOLYLINEROITOOLREPRESENTATION_H

#include "roitoolrepresentation.h"

namespace udg {

/**
    Representació per la tool de ROI poligonal
*/
class PolylineROIToolRepresentation : public ROIToolRepresentation {
Q_OBJECT
public:
    PolylineROIToolRepresentation(Drawer *drawer, QObject *parent = 0);
    ~PolylineROIToolRepresentation();

private:
    void createHandlers();
};

}

#endif


