#ifndef UDGCORESETTINGS_H
#define UDGCORESETTINGS_H

#include "defaultsettings.h"

namespace udg {

class CoreSettings : public DefaultSettings
{
public:
    CoreSettings();
    ~CoreSettings();

    void init();

    /// Declaració de claus
    static const QString ScreenShotToolFolder;
    static const QString ScreenShotToolFileExtension;
    static const QString ScreenShotToolFilename;
    static const QString LogViewerGeometry;
    static const QString UserHangingProtocolsPath;
    static const QString RegisterStatLogs;
    static const QString DefaultToolTextSize;
    static const QString AutoToolTextSize;
    
    static const QString NumberOfDirectionsForLowQualityObscurances;
    static const QString FunctionForLowQualityObscurances;
    static const QString VariantForLowQualityObscurances;
    static const QString GradientRadiusForLowQualityObscurances;

    static const QString NumberOfDirectionsForMediumQualityObscurances;
    static const QString FunctionForMediumQualityObscurances;
    static const QString VariantForMediumQualityObscurances;
    static const QString GradientRadiusForMediumQualityObscurances;

    static const QString NumberOfDirectionsForHighQualityObscurances;
    static const QString FunctionForHighQualityObscurances;
    static const QString VariantForHighQualityObscurances;
    static const QString GradientRadiusForHighQualityObscurances;

    static const QString LanguageLocale;

    /// Indica quina llibreria fem servir per llegir arxius DICOM
    static const QString DICOMImageReaderLibrary;

    /// Per fer que no es vegin més les Release Notes
    static const QString ShowReleaseNotesFirstTime;

    /// Comporbar si hi ha una nova versió
    static const QString LastVersionChecked;
    static const QString LastVersionCheckedDate;
    /// Cada quants dies es comprovarà si hi ha una versió nova
    static const QString CheckVersionInterval; 
    /// Per fer que no es vegin més les release notes d'una nova versió online
    static const QString NeverShowNewVersionReleaseNotes;
    /// Permetre que no es comprobin versions online
    static const QString DontCheckNewVersionsOnline;
};

} // end namespace udg 

#endif

