#ifndef UDGSHORTCUTS_H
#define UDGSHORTCUTS_H

#include "defaultsettings.h"

namespace udg {

class Shortcuts : public DefaultSettings
{
public:
    Shortcuts();
    ~Shortcuts();

    void init();

    /// Declaració de claus
    static const QString SlicingTool;
    static const QString WindowLevelTool;
    static const QString ZoomTool;
    static const QString TranslateTool;
    static const QString SeedTool;
    static const QString MagicTool;
    static const QString ScreenShotTool;
    static const QString Rotate3DTool;
    static const QString DistanceTool;
    static const QString ROITool;
    static const QString ReferenceLinesTool;
    static const QString VoxelInformationTool;
    static const QString SynchronizeTool;
    static const QString EraserTool;
    static const QString Cursor3DTool;
    static const QString AngleTool;
    static const QString NonClosedAngleTool;
    static const QString EditorTool;
    static const QString ClippingPlanesTool;
    static const QString RestoreTool;
    static const QString InvertWindowLevelTool;
    static const QString AxialViewTool;
    static const QString SagitalViewTool;
    static const QString CoronalViewTool;
    static const QString RotateClockWiseTool;
    static const QString RotateCounterClockWiseTool;
    static const QString HorizontalFlipTool;
    static const QString VerticalFlipTool;

    static const QString NewWindow;
    static const QString OpenFile;
    static const QString OpenDICOMDIR;
    static const QString OpenDirectory;
    static const QString OpenLocalDatabaseStudies;
    static const QString OpenPACS;
    static const QString OpenExams;
    static const QString Preferences;
    static const QString CloseCurrentExtension;
    static const QString CloseApplication;
    static const QString FullScreen;


};

}

#endif // SHORTCUTS_H
