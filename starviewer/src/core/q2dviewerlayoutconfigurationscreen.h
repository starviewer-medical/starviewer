#ifndef UDGQ2DVIEWERLAYOUTCONFIGURATIONSCREEN_H
#define UDGQ2DVIEWERLAYOUTCONFIGURATIONSCREEN_H

#include "ui_q2dviewerlayoutconfigurationscreenbase.h"

namespace udg {

/**
    Pantalla de configuració de layouts del visor 2D
 */
class Q2DViewerLayoutConfigurationScreen : public QWidget, private Ui::Q2DViewerLayoutConfigurationScreenBase {
Q_OBJECT
public:
    Q2DViewerLayoutConfigurationScreen(QWidget *parent = 0);
    ~Q2DViewerLayoutConfigurationScreen();

private:
    /// Inicialitza el widget amb els valors corresponents
    void initialize();
};

} // End namespace udg

#endif
