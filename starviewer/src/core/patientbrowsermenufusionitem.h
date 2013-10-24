#ifndef UDG_PATIENTBROWSERMENUFUSIONITEM_H
#define UDG_PATIENTBROWSERMENUFUSIONITEM_H

#include "patientbrowsermenubasicitem.h"

namespace udg {

/// Object to represent a fusion item of the browser menu
class PatientBrowserMenuFusionItem : public PatientBrowserMenuBasicItem
{
public:
    PatientBrowserMenuFusionItem(QObject *parent = 0);

    virtual QString getType();
};

} // namespace udg

#endif // UDG_PATIENTBROWSERMENUFUSIONITEM_H
