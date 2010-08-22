/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPRESENTATIONSTATEATTACHER_H
#define UDGPRESENTATIONSTATEATTACHER_H

#include <QObject>
#include <QString>
#include <QColor>

class DVPresentationState;
class DcmDataset;

namespace udg {

class Q2DViewer;
class Image;
class Q2DViewerBlackBoard;
/**
Classe que llegeix i aplica transformacions d'un Presentation State

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PresentationStateAttacher : public QObject
{
Q_OBJECT;
public:
    PresentationStateAttacher();
    ~PresentationStateAttacher();
    
    /// Canvia el fitxer i l'aplica al volum actual
    void setFile(const QString &fileName);

    /// Canvia al visor sobre al que apliquem el PR
    void setQ2DViewer(Q2DViewer *viewer);

private:
    /// Retorna cert si la imatge actual esta referenciada al Presentation State
    bool isImageReferencedInPresentationState(Image *image);

    /// Aplica el Presentation State a la imatge.
    void applyPresentationState();

    /// Específic de les transformacions espacials
    void applySpatialTransformation();

    /// Específic de l'area de display
    void applyDisplayedAreaTransformation(Image *image);

    /// Aplica les annotacions gràfiques i textuals DEFINIDES EN L'ESPAI DE PIXEL a aplicar ABANS de les transformacions espacials
    void applyGraphicAnnotations(Image *image);

    /// Ens dóna el color recomenat de les anotacions d'un layer
    QColor getRecommendedColor(int layer);

    /// aplica les annotacions gràfiques i textuals RELATIVES A LA DISPLAYED AREA a aplicar DESPRÉS de les transformacions espacials
    void applyPostSpatialTransformation(Image *image);

    /// processa els objectes de text del presentation state d'un layer assignats a una llesca
    void processTextObjects(int layer, int slice);

private:
    /// Objecte Dcmtk per a tractar amb Presentation States
    DVPresentationState *m_presentationStateHandler;
    
    /// El viewer sobre el qual apliquem el Presentation State
    Q2DViewer *m_2DViewer;
    
    /// Coordenades DE MÓN de l'area de display que ens dóna el presentation state
    double m_displayedAreaWorldTopLeft[2];
    double m_displayedAreaWorldBottomRight[2];

    /// Per gestionar les anotacions
    Q2DViewerBlackBoard *m_board;

    /// Retorna cert si hem aplicat transformacions espaials
    bool isSpatialTransformationApplied;
};

}
#endif
