#ifndef UDGANGLETOOL_H
#define UDGANGLETOOL_H

#include "tool.h"
#include <QPointer>

namespace udg {

class Q2DViewer;
class DrawerPolyline;
class DrawerText;

/**
    Tool per calcular angles
  */
class AngleTool : public Tool {
Q_OBJECT
public:
    /// Possibles estats de la tool
    enum { CenterFixed, FirstPointFixed, None };

    AngleTool(QViewer *viewer, QObject *parent = 0);
    ~AngleTool();

    void handleEvent(long unsigned eventID);

private:

    /// Ens permet anotar el primer vèrtex de l'angle.
    void annotateFirstPoint();

    /// Gestiona quin punt de l'angle estem dibuixant. Es cridarà cada cop que
    /// haguem fet un clic amb el botó esquerre del mouse.
    void handlePointAddition();

    /// Ens simula el segment de l'angle segons els punts annotats
    void simulateCorrespondingSegmentOfAngle();

    /// Dibuixa l'arc de circumferència que hi ha entre els dos segments
    /// quan estem definint l'angle
    void drawCircle();

    /// Ajustem el primer segment i creem la polilínia de l'arc de circumferència
    void fixFirstSegment();

    /// Acabem el dibuix de l'angle afegint l'annotació textual i eliminant l'arc de circumferència
    void finishDrawing();

    /// Calcula la correcta posició del caption de l'angle segons els punts de l'angle
    void placeText(DrawerText *angleText);

    /// Elimina la representacio temporal de la tool
    void deleteTemporalRepresentation();

private slots:
    /// Inicialitza l'estat de la tool
    void initialize();

private:
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Polilínia per dibuixar l'angle
    QPointer<DrawerPolyline> m_mainPolyline;

    /// Polilínia de la circumferència de l'angle.
    QPointer<DrawerPolyline> m_circlePolyline;

    /// Estat de la tool
    int m_state;

    /// Angle que formen en tot moment els dos segments
    double m_currentAngle;
};

}

#endif
