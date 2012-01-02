#ifndef UDGQ2DVIEWERCONFIGURATIONSCREEN_H
#define UDGQ2DVIEWERCONFIGURATIONSCREEN_H

#include "ui_q2dviewerconfigurationscreenbase.h"

namespace udg {

/**
    Pantalla de configuració del visor 2D
 */
class Q2DViewerConfigurationScreen : public QWidget, private Ui::Q2DViewerConfigurationScreenBase {
Q_OBJECT
public:
    Q2DViewerConfigurationScreen(QWidget *parent = 0);
    ~Q2DViewerConfigurationScreen();

private:
    /// Inicialitza el widget amb els valors corresponents
    void initialize();

    /// Actualitza els checkbox de les modalitats amb zoom per defecte segons els settings
    void initializeModalitiesWithZoomByDefault();

private slots:
    /// Actualitzen els corresponents settings
    void updateSliceScrollLoopSetting(bool enable);
    void updatePhaseScrollLoopSetting(bool enable);
    void updateReferenceLinesForMRSetting(bool enable);
    void updateReferenceLinesForCTSetting(bool enable);
    void updateModalitiesWithZoomByDefaultSetting();

    /// Farà que s'apliquin els canvis corresponents en els settings
    void applyChanges();
};

}

#endif
