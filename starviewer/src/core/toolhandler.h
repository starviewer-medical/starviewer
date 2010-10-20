/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTOOLHANDLER_H
#define UDGTOOLHANDLER_H

#include <QObject>
#include <QColor>

namespace udg {

/**
    Classe general pels handlers de les representacions de les Tools
*/
class ToolHandler : public QObject {
Q_OBJECT
public:    
    ToolHandler(QObject *parent = 0);
    ~ToolHandler();

    /// Selecciona / Deselecciona el handler
    bool isSelected();
    void select();
    void deselect();

    /// Mètodes per configurar el color
    void setColor(QColor color);
    QColor getColor() const;

    /// Repintar
    virtual void update() = 0;

    /// HACK!!: Afegeix paràmetres per correcció de profunditat
    void setParams(int zCoordinate, double depth);

public slots:
    /// Gestiona els events rebuts
    virtual void handleEvents(unsigned long eventID, double *point) = 0;

    /// Comprova si el click del mouse li correspon
    virtual void isClickNearMe(double *point) = 0;

signals:
    /// Envia el handler perquè el RepresentationsLayer sàpiga a qui ha d'enviar els events d'edició
    void clicked(ToolHandler *handler);

    /// Envia la nova posició després del desplaçament
    void movePoint(ToolHandler *handler, double *newPoint);
    void moveAllPoints(double *movement);

    /// Envia l'angle de rotació
    void rotate(double angle);

    /// Sends signal to select representation
    void selectRepresentation();

protected:
    /// HACK!!: Corregeix error de profunditat del punt
    void transformPointsDepth(double *point);

protected:
    /// Distància màxima al handler per editar
    double m_maxDistance;

    /// Indica si el handler està seleccionat per a l'edició
    bool m_isSelected;

    /// Color del Handler
    QColor m_color;

    /// Mida de la rodoneta
    double m_sphereRadius;

    /// HACK!!: S'utilitzen per corregir errors de profunditat
    int m_zCoordinate;
    double m_depth;
    
};

}

#endif
