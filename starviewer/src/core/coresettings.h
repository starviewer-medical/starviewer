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
    static const QString screenShotToolFolderKey;
    static const QString screenShotToolFileExtensionKey;
    static const QString screenShotToolFilenameKey;
    static const QString logViewerGeometryKey;
    static const QString userHangingProtocolsPathKey;
    static const QString registerStatLogsKey;
    
    static const QString numberOfDirectionsForLowQualityObscurancesKey;
    static const QString functionForLowQualityObscurancesKey;
    static const QString variantForLowQualityObscurancesKey;
    static const QString CoreSettings::gradientRadiusForLowQualityObscurancesKey;

    static const QString numberOfDirectionsForMediumQualityObscurancesKey;
    static const QString functionForMediumQualityObscurancesKey;
    static const QString variantForMediumQualityObscurancesKey;
    static const QString gradientRadiusForMediumQualityObscurancesKey;

    static const QString numberOfDirectionsForHighQualityObscurancesKey;
    static const QString functionForHighQualityObscurancesKey;
    static const QString variantForHighQualityObscurancesKey;
    static const QString gradientRadiusForHighQualityObscurancesKey;
};

} // end namespace udg 

#endif

