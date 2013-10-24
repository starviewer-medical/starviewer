#include "patientbrowsermenufusionitem.h"

namespace udg {

PatientBrowserMenuFusionItem::PatientBrowserMenuFusionItem(QObject *parent)
: PatientBrowserMenuBasicItem(parent)
{
}

QString PatientBrowserMenuFusionItem::getType()
{
    return "FusionItem";
}

} // namespace udg
