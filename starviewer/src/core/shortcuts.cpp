#include "shortcuts.h"
#include "settingsregistry.h"
#include <QList>
#include <QKeySequence>

namespace udg {

// Definició de les claus
const QString ShortcutsBase("Shortcuts/");
const QString Shortcuts::SlicingTool( ShortcutsBase + "SlicingTool" );
const QString Shortcuts::WindowLevelTool( ShortcutsBase + "WindowLevelTool" );
const QString Shortcuts::ZoomTool( ShortcutsBase + "ZoomTool" );
const QString Shortcuts::TranslateTool( ShortcutsBase + "TranslateTool" );
const QString Shortcuts::SeedTool( ShortcutsBase + "SeedTool" );
const QString Shortcuts::MagicTool( ShortcutsBase + "MagicTool" );
const QString Shortcuts::ScreenShotTool( ShortcutsBase + "ScreenShotTool" );
const QString Shortcuts::Rotate3DTool( ShortcutsBase + "Rotate3DTool" );
const QString Shortcuts::DistanceTool( ShortcutsBase + "DistanceTool" );
const QString Shortcuts::ROITool( ShortcutsBase + "ROITool" );
const QString Shortcuts::ReferenceLinesTool( ShortcutsBase + "ReferenceLinesTool" );
const QString Shortcuts::VoxelInformationTool( ShortcutsBase + "VoxelInformationTool" );
const QString Shortcuts::SynchronizeTool( ShortcutsBase + "SynchronizeTool" );
const QString Shortcuts::EraserTool( ShortcutsBase + "EraserTool" );
const QString Shortcuts::Cursor3DTool( ShortcutsBase + "Cursor3DTool" );
const QString Shortcuts::AngleTool( ShortcutsBase + "AngleTool" );
const QString Shortcuts::NonClosedAngleTool( ShortcutsBase + "NonClosedAngleTool" );
const QString Shortcuts::EditorTool( ShortcutsBase + "EditorTool" );
const QString Shortcuts::ClippingPlanesTool( ShortcutsBase + "ClippingPlanesTool" );
const QString Shortcuts::RestoreTool( ShortcutsBase + "RestoreTool" );
const QString Shortcuts::InvertWindowLevelTool( ShortcutsBase + "InvertWindowLevelTool" );
const QString Shortcuts::AxialViewTool( ShortcutsBase + "AxialViewTool" );
const QString Shortcuts::SagitalViewTool( ShortcutsBase + "SagitalViewTool" );
const QString Shortcuts::CoronalViewTool( ShortcutsBase + "CoronalViewTool" );
const QString Shortcuts::RotateClockWiseTool( ShortcutsBase + "RotateClockWiseTool" );
const QString Shortcuts::RotateCounterClockWiseTool( ShortcutsBase + "RotateCounterClockWiseTool" );
const QString Shortcuts::HorizontalFlipTool( ShortcutsBase + "HorizontalFlipTool" );
const QString Shortcuts::VerticalFlipTool( ShortcutsBase + "VerticalFlipTool" );
const QString Shortcuts::ClearCurrentViewAnnotationsTool( ShortcutsBase + "ClearCurrentViewAnnotationsActionTool" );
const QString Shortcuts::TransDifferenceTool( ShortcutsBase + "TransDifferenceActionTool" );

const QString Shortcuts::NewWindow( ShortcutsBase + "NewWindow" );
const QString Shortcuts::OpenFile( ShortcutsBase + "OpenFile" );
const QString Shortcuts::OpenDICOMDIR( ShortcutsBase + "OpenDICOMDIR" );
const QString Shortcuts::OpenDirectory( ShortcutsBase + "OpenDirectory" );
const QString Shortcuts::OpenLocalDatabaseStudies( ShortcutsBase + "OpenLocalDatabaseStudies" );
const QString Shortcuts::OpenPACS( ShortcutsBase + "OpenPACS" );
const QString Shortcuts::OpenExams( ShortcutsBase + "OpenExams" );
const QString Shortcuts::Preferences( ShortcutsBase + "Preferences" );
const QString Shortcuts::CloseCurrentExtension( ShortcutsBase + "CloseViewer" );
const QString Shortcuts::CloseApplication( ShortcutsBase + "CloseApp" );
const QString Shortcuts::FullScreen( ShortcutsBase + "FullScreen" );
const QString Shortcuts::KeyImageNoteTool( ShortcutsBase + "KeyImageNoteTool" );

Shortcuts::Shortcuts()
{
}

Shortcuts::~Shortcuts()
{
}

void Shortcuts::init()
{
    SettingsRegistry *settingsRegistry = SettingsRegistry::instance();

    QList<QVariant> shortcutsList;
    shortcutsList.append( QString("S") );
    settingsRegistry->addSetting( SlicingTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("W") );
    settingsRegistry->addSetting( WindowLevelTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("Z") );
    settingsRegistry->addSetting( ZoomTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("T") );
    settingsRegistry->addSetting( TranslateTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("X") );
    settingsRegistry->addSetting( SeedTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("M") );
    settingsRegistry->addSetting( MagicTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("") );//\TODO
    settingsRegistry->addSetting( ScreenShotTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("Q") );
    settingsRegistry->addSetting( Rotate3DTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("D") );
    settingsRegistry->addSetting( DistanceTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("Ctrl+K") );
    settingsRegistry->addSetting( KeyImageNoteTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("R") );
    settingsRegistry->addSetting( ROITool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("L") );
    settingsRegistry->addSetting( ReferenceLinesTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("V") );
    settingsRegistry->addSetting( VoxelInformationTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("") ); //\TODO
    settingsRegistry->addSetting( SynchronizeTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("E") );
    settingsRegistry->addSetting( EraserTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("C") );
    settingsRegistry->addSetting( Cursor3DTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("A") );
    settingsRegistry->addSetting( AngleTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("Shift+A") ); //\TODO
    settingsRegistry->addSetting( NonClosedAngleTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("N") );//\TODO
    settingsRegistry->addSetting( EditorTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("B") );
    settingsRegistry->addSetting( ClippingPlanesTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("Ctrl+R") );
    settingsRegistry->addSetting( RestoreTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("I") );
    settingsRegistry->addSetting( InvertWindowLevelTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("Ctrl+1") );//\TODO
    settingsRegistry->addSetting( AxialViewTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("Ctrl+2") );//\TODO
    settingsRegistry->addSetting( SagitalViewTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("Ctrl+3") );//\TODO
    settingsRegistry->addSetting( CoronalViewTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("G") );
    settingsRegistry->addSetting( RotateClockWiseTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("F") );
    settingsRegistry->addSetting( RotateCounterClockWiseTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("H") );
    settingsRegistry->addSetting( HorizontalFlipTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("J") );
    settingsRegistry->addSetting( VerticalFlipTool, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("Ctrl+E") );
    settingsRegistry->addSetting( ClearCurrentViewAnnotationsTool, shortcutsList );
    
    shortcutsList.clear();
    shortcutsList.append( QString("") ); //\TODO
    settingsRegistry->addSetting( TransDifferenceTool, shortcutsList );
    
    shortcutsList.clear();
    shortcutsList.append( QKeySequence( QKeySequence::New ).toString() );
    settingsRegistry->addSetting( NewWindow, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QKeySequence( QKeySequence::Open ).toString() );
    settingsRegistry->addSetting( OpenFile, shortcutsList );
    
    shortcutsList.clear();
    shortcutsList.append( QString("Ctrl+D") );
    settingsRegistry->addSetting( OpenDICOMDIR, shortcutsList );
    
    shortcutsList.clear();
    shortcutsList.append( QString("Ctrl+Shift+D") );
    settingsRegistry->addSetting( OpenDirectory, shortcutsList );
    
    shortcutsList.clear();
    shortcutsList.append( QString("Ctrl+L") );
    settingsRegistry->addSetting( OpenLocalDatabaseStudies, shortcutsList );
    
    shortcutsList.clear();
    shortcutsList.append( QString("Ctrl+P") );
    settingsRegistry->addSetting( OpenPACS, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("Ctrl+E") );
    settingsRegistry->addSetting( OpenExams, shortcutsList );
    
    shortcutsList.clear();
    shortcutsList.append( QString("") ); //\TODO
    settingsRegistry->addSetting( Preferences, shortcutsList );
    
    shortcutsList.clear();
    shortcutsList.append( QString("Ctrl+W") );
    settingsRegistry->addSetting( CloseCurrentExtension, shortcutsList );
    
    shortcutsList.clear();
    shortcutsList.append( QString("Ctrl+Q") );
    settingsRegistry->addSetting( CloseApplication, shortcutsList );

    shortcutsList.clear();
    shortcutsList.append( QString("Ctrl+Return") );
    settingsRegistry->addSetting( FullScreen, shortcutsList );

}

} // end namespace udg
