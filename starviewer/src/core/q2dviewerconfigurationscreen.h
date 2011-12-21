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
    
    /// Crea les connexions corresponents de signals i slots
    void createConnections();

private slots:
    /// Es cridaran quan es modifiquin els check box actualitzant els corresponents settings
    void updateSliceScrollLoopSetting(bool enable);
    void updatePhaseScrollLoopSetting(bool enable);
};

}

#endif
