/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGSHORTCUTS_H
#define UDGSHORTCUTS_H

#include "defaultsettings.h"

namespace udg {

class Shortcuts : public DefaultSettings {
public:
    Shortcuts();
    ~Shortcuts();

    void init();

    /// Declaració de claus
    static const QString SlicingMouseTool;
    static const QString SlicingWheelTool;
    static const QString WindowLevelTool;
    static const QString ZoomTool;
    static const QString SeedTool;
    static const QString MagicROITool;
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
    static const QString InvertVoiLutTool;
    static const QString AxialViewTool;
    static const QString SagitalViewTool;
    static const QString CoronalViewTool;
    static const QString RotateClockWiseTool;
    static const QString RotateCounterClockWiseTool;
    static const QString HorizontalFlipTool;
    static const QString VerticalFlipTool;
    static const QString ClearCurrentViewAnnotationsTool;
    static const QString TransDifferenceTool;
    static const QString EllipticalROITool;
    static const QString MagnifyingGlassTool;
    static const QString AutomaticSynchronizationTool;
    static const QString SynchronizeAllViewers;
    static const QString DesynchronizeAllViewers;
    static const QString Propagation;
    static const QString NextHangingProtocol;
    static const QString PreviousHangingProtocol;
    static const QString ToggleComparativeStudiesMode;

    static const QString SaveSingleScreenShot;
    static const QString SaveWholeSeriesScreenShot;
    
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

    static const QString ViewSelectedStudies;
    static const QString ImportToLocalDatabaseSelectedDICOMDIRStudies;
    static const QString DeleteSelectedLocalDatabaseStudies;
    static const QString StoreSelectedStudiesToPACS;
    static const QString SendSelectedStudiesToDICOMDIRList;
    static const QString RetrieveAndViewSelectedStudies;
    static const QString RetrieveSelectedStudies;

    static const QString MoveToPreviousDesktop;
    static const QString MoveToNextDesktop;
    static const QString MaximizeMultipleScreens;

    static const QString ExternalApplication1;
    static const QString ExternalApplication2;
    static const QString ExternalApplication3;
    static const QString ExternalApplication4;
    static const QString ExternalApplication5;
    static const QString ExternalApplication6;
    static const QString ExternalApplication7;
    static const QString ExternalApplication8;
    static const QString ExternalApplication9;
    static const QString ExternalApplication10;
    static const QString ExternalApplication11;
    static const QString ExternalApplication12;
};

}

#endif // SHORTCUTS_H
