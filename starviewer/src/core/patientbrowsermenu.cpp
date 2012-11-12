#include "patientbrowsermenu.h"

#include "patient.h"
#include "patientbrowsermenuextendeditem.h"
#include "patientbrowsermenulist.h"
#include "series.h"
#include "image.h"
#include "volume.h"
#include "volumerepository.h"
#include "applicationstylehelper.h"
#include "screenmanager.h"

namespace udg {

PatientBrowserMenu::PatientBrowserMenu(QWidget *parent)
 : QWidget(parent), m_patientBrowserList(0), m_patientAdditionalInfo(0)
{
    ApplicationStyleHelper style;
    style.setScaledFontSizeTo(this);

    // Inicialment no sabem en quina pantalla es pot desplegar el menú
    m_currentScreenID = m_leftScreenID = m_rightScreenID = -1;
}

PatientBrowserMenu::~PatientBrowserMenu()
{
}

// Mètode per indicar a qSort com ordenar una llista d'estudis de més recent a més antic
bool studyNewerThan(Study *study1, Study *study2)
{
    return study1->getDateTime() > study2->getDateTime();
}

void PatientBrowserMenu::setPatient(Patient *patient)
{
    createWidgets();
    QString caption;
    QString label;
    // TODO Hauríem de tenir els mètodes perquè Patient ens retornés els estudis ordenats en 
    // ascendent o descendent en comptes de fer-ho així
    QList<Study*> descendingSortedStudies = patient->getStudies();
    qSort(descendingSortedStudies.begin(), descendingSortedStudies.end(), studyNewerThan);
    foreach (Study *study, descendingSortedStudies)
    {
        // Extreiem el caption de l'estudi
        caption = tr("Study %1 %2 [%3] %4")
            .arg(study->getDate().toString(Qt::ISODate))
            .arg(study->getTimeAsString())
            .arg(study->getModalitiesAsSingleString())
            .arg(study->getDescription());

        // Per cada sèrie de l'estudi extreurem el seu label i l'identificador
        QList<QPair<QString, QString> > itemsList;
        foreach (Series *series, study->getViewableSeries())
        {
            label = tr(" Series %1: %2 %3 %4 %5")
                        .arg(series->getSeriesNumber().trimmed())
                        .arg(series->getProtocolName().trimmed())
                        .arg(series->getDescription().trimmed())
                        .arg(series->getBodyPartExamined())
                        .arg(series->getViewPosition());

            if (series->getNumberOfVolumes() > 1)
            {
                QString volumeID;
                int volumeNumber = 1;
                foreach (Volume *volume, series->getVolumesList())
                {
                    QPair<QString, QString> itemPair;
                    // Label
                    itemPair.first = label + " (" + QString::number(volumeNumber) + ")";
                    volumeNumber++;
                    // Identifier
                    itemPair.second = QString::number(volume->getIdentifier().getValue());
                    // Afegim el parell a la llista
                    itemsList << itemPair;
                }
            }
            // Només tenim un sol volum per la sèrie
            else
            {
                Volume *volume = series->getFirstVolume();
                QPair<QString, QString> itemPair;
                // Label
                itemPair.first = label;
                // Identifier
                itemPair.second = QString::number(volume->getIdentifier().getValue());
                // Afegim el parell a la llista
                itemsList << itemPair;
            }
        }
        // Afegim les sèries agrupades per estudi
        m_patientBrowserList->addItemsGroup(caption, itemsList);
    }

    connect(m_patientBrowserList, SIGNAL(isActive(QString)), SLOT(updateActiveItemView(QString)));
    connect(m_patientBrowserList, SIGNAL(selectedItem(QString)), SLOT(processSelectedItem(QString)));
}

void PatientBrowserMenu::updateActiveItemView(const QString &identifier)
{
    Identifier id(identifier.toInt());
    Volume *volume = VolumeRepository::getRepository()->getVolume(id);
    if (volume)
    {
        // Actualitzem les dades de l'item amb informació adicional
        m_patientAdditionalInfo->setPixmap(volume->getThumbnail());
        Series *series = volume->getImage(0)->getParentSeries();
        m_patientAdditionalInfo->setText(QString(tr("%1 \n%2 \n%3\n%4 Images"))
                                        .arg(series->getDescription().trimmed())
                                        .arg(series->getModality().trimmed())
                                        .arg(series->getProtocolName().trimmed())
                                        .arg(volume->getNumberOfFrames())
                                         );
        placeAdditionalInfoWidget();
    }
}

void PatientBrowserMenu::popup(const QPoint &point, const QString &identifier)
{
    // Marquem l'ítem actual de la llista
    m_patientBrowserList->markItem(identifier);

    // Determinem quines són les pantalles que tenim
    ScreenManager screenManager;
    m_currentScreenID = screenManager.getScreenID(point);
    ScreenLayout screenLayout = screenManager.getScreenLayout();
    m_leftScreenID = screenLayout.getScreenOnTheLeftOf(m_currentScreenID);
    m_rightScreenID = screenLayout.getScreenOnTheRightOf(m_currentScreenID);
    
    // Mirem si l'alçada  del widget excedeix l'alçada de la pantalla
    // En cas que sigui així anem afegint noves columnes per redistribuir els elements fins que l'alçada sigui menor a la de la pantalla
    QRect currentScreenGeometry = screenLayout.getScreen(m_currentScreenID).getAvailableGeometry();
    while (m_patientBrowserList->sizeHint().height() > currentScreenGeometry.height())
    {
        m_patientBrowserList->addColumn();
    }
    
    // Calculem l'alineament del menú
    bool rightAligned = shouldAlignMenuToTheRight(currentScreenGeometry);
    
    // Calculem quanta part de la llista queda fora segons la geometria de la pantalla on se'ns ha demanat obrir el menú
    // i així poder determinar la posició del widget per tal que no surti fora de la pantalla i es pugui veure el seu contingut.
    QSize outside;
    computeListOutsideSize(point, outside, rightAligned);

    // TODO Què fem si la pantalla adicional té una alçada més petita que l'actual? Recalculem tot el menú perquè tingui l'alçada menor?
    // De moment això no es tindrà en compte
    
    // Si els valors són positius caldrà moure la posició original com a mínim tot el que ens sortim de les fronteres
    // TODO En teoria aquest Margin no seria necessari si fèssim servir frameGeometry() en comptes de sizeHint()
    const int Margin = 5;
    int menuXPosition = point.x();
    int menuYPosition = point.y();
    if (outside.width() > 0)
    {
        menuXPosition -= outside.width() + Margin;
    }
    if (outside.height() > 0)
    {
        menuYPosition -= outside.height() + Margin;
    }

    // Movem la finestra del menu al punt que toca
    m_patientBrowserList->move(menuXPosition, menuYPosition);

    // Col·loquem el widget amb la informació adicional a la dreta o l'esquerra del principal segons l'espai disponible
    placeAdditionalInfoWidget();
    m_patientAdditionalInfo->show();
    // TODO: HACK si no mostrem l'"Aditional info" abans que el "browser list" després no processa els events
    // correctament i quan seleccionem una sèrie no arriba el signal enlloc i no es pot seleccionar cap sèrie
    // relacionat amb el ticket #555 Això només passa amb qt 4.3, amb qt 4.2 no tenim aquest
    // problema. Amb qt 4.2 podem fer show en l'ordre que volguem. El que fem per evitar flickering és mostrar-lo sota mateix
    // del "browser list" i així no es nota tant
    // Això, a partir de Qt 4.7 sembla que només passa a Mac. Semblaria que això és el símptoma d'un altre problema: que es mostri com un popup?
    m_patientBrowserList->show();

    // TODO No es té en compte si després d'haver mogut de lloc el widget aquest ja es veu correctament,
    // ja que es podria donar el cas que el widget no hi cabés a tota la pantalla
    // Caldria millorar el comportament en certs aspectes, com per exemple, redistribuir les files i columnes
    // del llistat si aquest no hi cap a la pantalla, per exemple.
}

bool PatientBrowserMenu::shouldAlignMenuToTheRight(const QRect &currentScreenGeometry)
{
    bool rightAligned = true;
    // Cal comprovar si la llista resultant és més ample
    if (m_patientBrowserList->sizeHint().width() > currentScreenGeometry.width())
    {
        if (m_leftScreenID == -1)
        {
            if (m_rightScreenID == -1)
            {
                // No podem expandir més, no tenim més pantalles.
                // TODO Explorar top i bottom?
            }
            else
            {
                // Tenim pantalla per expandir-nos a la dreta
                rightAligned = false;
            }
        }
        else
        {
            if (m_rightScreenID == -1)
            {
                // Tenim pantalla per expandir-nos a l'esquerra
                rightAligned = true;
            }
            else
            {
                // Tenim pantalles a esquerra i dreta. Cal escollir quina és la més adequada.
                // Opció 1) La pantalla amb la mateixa alçada o major per mantenir aspecte
                ScreenLayout screenLayout = ScreenManager().getScreenLayout();
                QRect leftScreenGeometry = screenLayout.getScreen(m_leftScreenID).getGeometry();
                QRect rightScreenGeometry = screenLayout.getScreen(m_rightScreenID).getGeometry();

                if (leftScreenGeometry.height() >= rightScreenGeometry.height())
                {
                    rightAligned = true;
                }
                else
                {
                    rightAligned = false;
                }
            }
        }
    }

    return rightAligned;
}

void PatientBrowserMenu::computeListOutsideSize(const QPoint &popupPoint, QSize &out, bool rightAligned)
{
    // Obtenim la geometria de la pantalla on se'ns ha demanat obrir el menú
    ScreenLayout screenLayout = ScreenManager().getScreenLayout();
    QRect currentScreenGeometry = screenLayout.getScreen(m_currentScreenID).getAvailableGeometry();
    QPoint currentScreenGlobalOriginPoint = currentScreenGeometry.topLeft();

    // Calculem les mides dels widgets per saber on els hem de col·locar
    // TODO Aquestes mesures no són les més exactes. Les adequades haurien de basar-se en els valors de QWidget::frameGeometry()
    // tal com s'explica a http://doc.trolltech.com/4.7/application-windows.html#window-geometry
    // El problema és que cal que abans haguem fet un show() per saber les mides de veritat. Una opció també seria implementar
    // aquest posicionament quan es produeix un resize d'aquests widgets. Aquesta és una de les raons per la qual veiem el menú
    // adicional amb una petita ombra quan es queda a la dreta del principal.
    int mainMenuApproximateWidth = m_patientBrowserList->sizeHint().width();
    int wholeMenuApproximateHeight = qMax(m_patientBrowserList->sizeHint().height(), m_patientAdditionalInfo->sizeHint().height());

    // Calculem les fronteres per on ens podria sortir el menú (lateral dret/esquerre)
    if (rightAligned)
    {
        int globalRight = currentScreenGlobalOriginPoint.x() + currentScreenGeometry.width();
        int widgetRight = mainMenuApproximateWidth + popupPoint.x();
        out.setWidth(widgetRight - globalRight);
    }
    else
    {
        int globalLeft = currentScreenGlobalOriginPoint.x();
        int widgetLeft = popupPoint.x();
        out.setWidth(widgetLeft - globalLeft);
    }
    
    // Calculem les fronteres per on ens podria sortir el menú (alçada)
    int globalBottom = currentScreenGlobalOriginPoint.y() + currentScreenGeometry.height();
    int widgetBottom = wholeMenuApproximateHeight + popupPoint.y();
    out.setHeight(widgetBottom - globalBottom);
}

void PatientBrowserMenu::placeAdditionalInfoWidget()
{
    ScreenLayout screenLayout = ScreenManager().getScreenLayout();
    QRect currentScreenGeometry = screenLayout.getScreen(m_currentScreenID).getAvailableGeometry();
    QPoint currentScreenGlobalOriginPoint = currentScreenGeometry.topLeft();

    // TODO Aquesta mesura no és la més exacta. L'adequada hauria de basar-se en els valors de QWidget::frameGeometry()
    int mainMenuApproximateWidth = m_patientBrowserList->sizeHint().width();
    int menuXPosition = m_patientBrowserList->pos().x();
    int menuYPosition = m_patientBrowserList->pos().y();
    // Cal fer un adjust size abans perquè les mides del widget quedin actualitzades correctament
    m_patientAdditionalInfo->adjustSize();
    int additionalInfoWidgetApproximateWidth = m_patientAdditionalInfo->frameGeometry().width();

    int menuXShift = 0;
    if (menuXPosition + mainMenuApproximateWidth + additionalInfoWidgetApproximateWidth > currentScreenGlobalOriginPoint.x() + currentScreenGeometry.width())
    {
        // En cas que la combinació de menús en sí, ja ocupi tota la pantalla comprobem en quina pantalla és millor col·locar el menú addicional
        if (mainMenuApproximateWidth + additionalInfoWidgetApproximateWidth >= currentScreenGlobalOriginPoint.x() + currentScreenGeometry.width())
        {
            if (m_leftScreenID != -1)
            {
                // A l'esquerra
                menuXShift = -additionalInfoWidgetApproximateWidth;
            }
            else if (m_rightScreenID != -1)
            {
                // A la dreta
                menuXShift = mainMenuApproximateWidth;
            }
            else
            {
                // No tenim pantalles ni a esquerre ni a dreta, li indiquem a l'esquerra
                menuXShift = -additionalInfoWidgetApproximateWidth;
            }
        }
        else
        {
            // Tenim espai a l'esquerra, dins de la mateixa pantalla
            menuXShift = -additionalInfoWidgetApproximateWidth;
        }
    }
    else
    {
        // Tenim espai a la dreta, dins de la mateixa pantalla
        menuXShift = mainMenuApproximateWidth;
    }

    m_patientAdditionalInfo->move(menuXPosition + menuXShift, menuYPosition);
}

void PatientBrowserMenu::processSelectedItem(const QString &identifier)
{
    m_patientAdditionalInfo->hide();
    m_patientBrowserList->hide();

    if (m_patientBrowserList->getMarkedItem() != identifier)
    {
        Identifier id(identifier.toInt());
        emit selectedVolume(VolumeRepository::getRepository()->getVolume(id));
    }
}

void PatientBrowserMenu::createWidgets()
{
    if (m_patientAdditionalInfo)
    {
        delete m_patientAdditionalInfo;
    }

    if (m_patientBrowserList)
    {
        delete m_patientBrowserList;
    }

    m_patientAdditionalInfo = new PatientBrowserMenuExtendedItem(this);
    m_patientBrowserList = new PatientBrowserMenuList(this);

    m_patientAdditionalInfo->setWindowFlags(Qt::Popup);
    m_patientBrowserList->setWindowFlags(Qt::Popup);

    connect(m_patientAdditionalInfo, SIGNAL(closed()), m_patientBrowserList, SLOT(close()));
    connect(m_patientBrowserList, SIGNAL(closed()), m_patientAdditionalInfo, SLOT(close()));
}

}
