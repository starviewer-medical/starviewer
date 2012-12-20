#ifndef UDGQ2DVIEWERLAYOUTCONFIGURATIONSCREEN_H
#define UDGQ2DVIEWERLAYOUTCONFIGURATIONSCREEN_H

#include "ui_q2dviewerlayoutconfigurationscreenbase.h"

namespace udg {

class StudyLayoutConfig;

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
    void populateMaxViewersComboBox();
    void initializeModalitiesToApplyHangingProtocolsByDefault();
    void initializeAutomaticLayoutSettings();

    void setCurrentStudyLayoutConfig(const StudyLayoutConfig &config);
private slots:
    /// Farà que s'apliquin els canvis corresponents en els settings
    void applyChanges();

    /// Actualitza el widget on es mostren els settings d'un layout de la modalitat escollida en el combo en aquell moment
    void updateLayoutSettingsWidgetForModality(const QString &modality);

    void updateSelectedAutomaticLayoutSettings();
    void restoreDefaultsForSelectedAutomaticLayoutSettings();
};

} // End namespace udg

#endif
