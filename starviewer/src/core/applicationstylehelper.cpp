#include "applicationstylehelper.h"

#include <QDesktopWidget>
#include <QApplication>

#include "coresettings.h"

namespace udg {

ApplicationStyleHelper::ApplicationStyleHelper()
{
}

void ApplicationStyleHelper::recomputeStyleToScreenOfWidget(QWidget *widget)
{
    Settings settings;
    if (settings.getValue(CoreSettings::AutoToolTextSize).toBool())
    {
        QDesktopWidget *desktop = QApplication::desktop();
        //Per calcular el tamany de la lletra tindrem en compte
        //la resolució de la pantalla a on s'està executant l'Starviewer.
        const QRect screen = desktop->screenGeometry(widget);

        int textSize;
        if ((screen.width() * screen.height()) >= (5 * 1024 * 1024))
        {
            textSize = 24;
        }
        else if ((screen.width() * screen.height()) >= (3 * 1024 * 1024))
        {
            textSize = 17;
        }
        else
        {
            textSize = 14;
        }

        settings.setValue(CoreSettings::DefaultToolTextSize, textSize);
    }
}

int ApplicationStyleHelper::getToolsFontSize() const
{
    Settings settings;
    return settings.getValue(CoreSettings::DefaultToolTextSize).toInt();
}

} // Ens namespace udg
