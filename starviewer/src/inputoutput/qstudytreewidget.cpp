#include "qstudytreewidget.h"

#include <QContextMenuEvent>

#include "logging.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "dicommask.h"

namespace udg {

QStudyTreeWidget::QStudyTreeWidget(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);

    m_studyTreeView->setColumnHidden(Type, true);
    // Amaguem la columna Hora, ja que ara es mostra la data i hora en un mateix columna per poder ordenar per data i hora els estudis
    m_studyTreeView->setColumnHidden(Time, true);
    //Hack: Degut a que es guarden als settings l'amplada de les columnes per IndexColumna, si afegim una nova columna, l'hem d'afegir al final no la podem afegir al seu lloc corresponent 
    //ja que sinó l'amplada de les columnes guardades als settings s'aplicaria malament. Imaginem que tenim les columnes PatientName, StudyID StudyDescription,  si afegim una nova columna al costat
    //de PatientName en aquesta nova columna se li aplicarà l'amplada que tenia StudyID, a StudyID la de StudyDescription i així successivament, per això el que fem si afegim una nova columna
    //s'afegeix al final del QTreeWidget i llavors la movem al seu lloc natural
    m_studyTreeView->header()->moveSection(PatientBirth, PatientAge);

    // Carreguem les imatges que es mostren el QStudyTreeWidget
    m_openFolder = QIcon(":/images/folderopen.png");
    m_closeFolder = QIcon(":/images/folderclose.png");
    m_iconSeries = QIcon(":/images/series.png");

    createConnections();

    m_studyTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // Indiquem que el nivell màxim que per defecte es pot expedir l'arbre Study/Series/Image és fins a nivell d'Image
    m_maximumExpandTreeItemsLevel = ImageLevel;

    initialize();
}

QStudyTreeWidget::~QStudyTreeWidget()
{
}

void QStudyTreeWidget::createConnections()
{
    connect(m_studyTreeView, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), SLOT(doubleClicked(QTreeWidgetItem *, int)));
    connect(m_studyTreeView, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
            SLOT (currentItemChanged (QTreeWidgetItem *, QTreeWidgetItem *)));
    connect(m_studyTreeView, SIGNAL(itemExpanded(QTreeWidgetItem *)), SLOT (itemExpanded(QTreeWidgetItem *)));
    connect(m_studyTreeView, SIGNAL(itemCollapsed(QTreeWidgetItem *)), SLOT (itemCollapsed(QTreeWidgetItem *)));
}

QTreeWidget* QStudyTreeWidget::getQTreeWidget() const
{
    return m_studyTreeView;
}

QList<QPair<DicomMask, DICOMSource> > QStudyTreeWidget::getDicomMaskOfSelectedItems()
{
    QList<QPair<DicomMask, DICOMSource> > dicomMaskDICOMSourceList;

    foreach (QTreeWidgetItem *item, m_studyTreeView->selectedItems())
    {
        QPair<DicomMask, DICOMSource> qpairDicomMaskDICOMSource;
        bool ok;

        if (isItemStudy(item))
        {
            // És un estudi
            qpairDicomMaskDICOMSource.first = DicomMask::fromStudy(m_addedStudiesByDICOMItemID[item->text(DICOMItemID).toInt()], ok);
            qpairDicomMaskDICOMSource.second = m_addedStudiesByDICOMItemID[item->text(DICOMItemID).toInt()]->getDICOMSource();

            dicomMaskDICOMSourceList.append(qpairDicomMaskDICOMSource);
        }
        else if (isItemSeries(item))
        {
            //Si l'estudi pare no està seleccionat
            if (!item->parent()->isSelected())
            {
                qpairDicomMaskDICOMSource.first = DicomMask::fromSeries(m_adddSeriesByDICOMItemID[item->text(DICOMItemID).toInt()], ok);
                qpairDicomMaskDICOMSource.second = m_adddSeriesByDICOMItemID[item->text(DICOMItemID).toInt()]->getDICOMSource();

                dicomMaskDICOMSourceList.append(qpairDicomMaskDICOMSource);
            }
        }
        else if (isItemImage(item))
        {
            //Si la sèrie pare i l'estudi pare no està seleccionat
            if (!item->parent()->isSelected() && !item->parent()->parent()->isSelected())
            {
                qpairDicomMaskDICOMSource.first = DicomMask::fromImage(m_addedImagesByDICOMItemID[item->text(DICOMItemID).toInt()], ok);
                qpairDicomMaskDICOMSource.second = m_addedImagesByDICOMItemID[item->text(DICOMItemID).toInt()]->getDICOMSource();

                dicomMaskDICOMSourceList.append(qpairDicomMaskDICOMSource);
            }
        }
        else
        {
            DEBUG_LOG("Texte no esperat: " + item->text(Type));
        }
    }

    return dicomMaskDICOMSourceList;
}

void QStudyTreeWidget::insertPatientList(QList<Patient*> patientList)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    foreach (Patient *patient, patientList)
    {
        insertPatient(patient);
    }

    QApplication::restoreOverrideCursor();
}

void QStudyTreeWidget::insertPatient(Patient *patient)
{
    if (patient->getNumberOfStudies() > 0)
    {
        if (getStudyQTreeWidgetItem(patient->getStudies().at(0)->getInstanceUID(), patient->getStudies().at(0)->getDICOMSource()) != NULL)
        {
            // Si l'estudi ja hi existeix a StudyTreeView l'esborrem
            removeStudy(patient->getStudies().at(0)->getInstanceUID());
        }

        m_studyTreeView->addTopLevelItems(fillPatient(patient));
        m_studyTreeView->clearSelection();
    }
}

QList<QTreeWidgetItem*> QStudyTreeWidget::fillPatient(Patient *patient)
{
    QList<QTreeWidgetItem*> qtreeWidgetItemList;

    foreach (Study *studyToInsert, patient->getStudies())
    {
        // Inserim l'estudi a la llista d'estudis
        m_addedStudiesByDICOMItemID[m_nextIDICOMItemIDOfStudy] = studyToInsert;

        QTreeWidgetItem *item = new QTreeWidgetItem();

        item->setText(DICOMItemID, QString::number(m_nextIDICOMItemIDOfStudy++));
        item->setIcon(ObjectName, m_closeFolder);
        item->setText(ObjectName, patient->getFullName());
        item->setText(PatientID, patient->getID());
        item->setText(PatientBirth, formatDateTime(patient->getBirthDate(), QTime()));
        item->setText(PatientAge, formatAge(studyToInsert->getPatientAge()));
        item->setText(Modality, studyToInsert->getModalitiesAsSingleString());
        item->setText(Description, studyToInsert->getDescription());
        // TODO:No hauria de ser l'estudi que tornés la data formatada? Problema necessitem que la data estigui en format yyyy/mm/dd per poder ordenar per data
        item->setText(Date, formatDateTime(studyToInsert->getDate(), studyToInsert->getTime()));
        item->setText(StudyID, tr("Study %1").arg(studyToInsert->getID()));
        item->setText(Institution, studyToInsert->getInstitutionName());
        item->setText(AccNumber, studyToInsert->getAccessionNumber());
        item->setText(UID, studyToInsert->getInstanceUID());
        // Indiquem de que es tracta d'un estudi
        item->setText(Type, "STUDY");
        item->setText(RefPhysName, studyToInsert->getReferringPhysiciansName());

        // Comprovem si el TreeItem s'ha d'expandir en funció del nivell màxim que ens han indicat que ens podem expandir
        if (m_maximumExpandTreeItemsLevel > StudyLevel)
        {
            // Degut que per cada item estudi tenim items fills que són series, i que consultar les series per cada estudi és
            // una operació costosa (per exemple quan es consulta al pacs) només inserirem les sèries per a que les pugui
            // consultar l'usuari quan es facin un expand d'estudi, però per a que apareixi el botó "+" de desplegar l'estudi inserim un item en blanc
            QTreeWidgetItem *expandableItem = new QTreeWidgetItem();

            expandableItem->setText(Type, "EXPANDABLE_ITEM");
            item->addChild(expandableItem);
        }
        qtreeWidgetItemList.append(item);
    }

    return qtreeWidgetItemList;
}

void QStudyTreeWidget::insertSeriesList(QString studyInstanceUID, QList<Series*> seriesList)
{
    if (seriesList.count() == 0)
    {
        return;
    }

    QList<QTreeWidgetItem*> qTreeWidgetItemSeriesList;
    QTreeWidgetItem *studyItem = getStudyQTreeWidgetItem(studyInstanceUID, seriesList.at(0)->getDICOMSource());

    foreach (Series *series, seriesList)
    {
        qTreeWidgetItemSeriesList.append(fillSeries(series));
        //Afegim a la sèrie quin és l'estudi pare, de manera que quan amb algun mètode retornem la sèrie, la retornem amb la informació completa
        series->setParentStudy(m_addedStudiesByDICOMItemID[studyItem->text(DICOMItemID).toInt()]);
    }

    studyItem->addChildren(qTreeWidgetItemSeriesList);
}

QTreeWidgetItem* QStudyTreeWidget::fillSeries(Series *series)
{
    m_adddSeriesByDICOMItemID[m_nextDICOMItemIDOfSeries]= series;

    QTreeWidgetItem *seriesItem = new QTreeWidgetItem();

    seriesItem->setText(DICOMItemID, QString::number(m_nextDICOMItemIDOfSeries++));
    seriesItem->setIcon(ObjectName, m_iconSeries);
    // Li fem un padding per poder ordenar la columna, ja que s'ordena per String
    seriesItem->setText(ObjectName, tr("Series %1").arg(paddingLeft(series->getSeriesNumber(), 4)));
    seriesItem->setText(Modality, series->getModality());

    // Treiem els espaics en blanc del davant i darrera
    seriesItem->setText(Description, series->getDescription().simplified());

    seriesItem->setText(Date, formatDateTime(series->getDate(), series->getTime()));
    seriesItem->setText(UID, series->getInstanceUID());
    // Indiquem que es tracta d'una sèrie
    seriesItem->setText(Type, "SERIES");

    seriesItem->setText(ProtocolName, series->getProtocolName());
    seriesItem->setText(PPStartDate, series->getPerformedProcedureStepStartDate());
    seriesItem->setText(PPStartTime, series->getPerformedProcedureStepStartTime());
    seriesItem->setText(ReqProcID, series->getRequestedProcedureID());
    seriesItem->setText(SchedProcStep, series->getScheduledProcedureStepID());

    // Comprovem si el TreeItem s'ha d'expandir en funció del nivell màxim que ens han indicat que ens podem expandir
    if (m_maximumExpandTreeItemsLevel > SeriesLevel)
    {
        // Degut que per cada item serie tenim items fills que són imatges, i que consultar les imatges per cada sèrie és
        // una operació costosa (per exemple quan es consulta al pacs) només inserirem les sèries per a que les pugui
        // consultar l'usuari quan es facin un expand de la sèrie, però per a que apareixi el botó "+" de desplegar la sèrie inserim un item en blanc
        QTreeWidgetItem *expandableItem = new QTreeWidgetItem();

        expandableItem->setText(Type, "EXPANDABLE_ITEM");
        seriesItem->addChild(expandableItem);
    }
    return seriesItem;
}

void QStudyTreeWidget::insertImageList(QString studyInstanceUID, QString seriesInstanceUID, QList<Image*> imageList)
{
    if (imageList.count() == 0)
    {
        return;
    }

    QTreeWidgetItem *seriesItem = getSeriesQTreeWidgetItem(studyInstanceUID, seriesInstanceUID, imageList.at(0)->getDICOMSource());
    if (seriesItem != NULL)
    {
        QList<QTreeWidgetItem*> qTreeWidgetItemImageList;
        QTreeWidgetItem *newImageItem;

        foreach (Image *image, imageList)
        {
            m_addedImagesByDICOMItemID[m_nextDICOMItemIDOfImage] = image;

            newImageItem = new QTreeWidgetItem();

            newImageItem->setText(DICOMItemID, QString::number(m_nextDICOMItemIDOfImage++));
            newImageItem->setIcon(ObjectName, m_iconSeries);
            // Li fem un padding per poder ordenar la columna, ja que s'ordena per String
            newImageItem->setText(ObjectName, tr("File %1").arg(paddingLeft(image->getInstanceNumber(), 4)));
            newImageItem->setText(UID, image->getSOPInstanceUID());
            newImageItem->setText(Type, "IMAGE");
            // Indiquem que es tracta d'una imatge
            qTreeWidgetItemImageList.append(newImageItem);

            image->setParentSeries(m_adddSeriesByDICOMItemID[seriesItem->text(DICOMItemID).toInt()]);
        }
        // Afegim la llista d'imatges
        seriesItem->addChildren(qTreeWidgetItemImageList);
    }
    else
    {
        DEBUG_LOG("NO S'HA POGUT TROBAR LA SERIE A LA QUE S'HAVIA D'INSERIR LA IMATGE");
    }
}

void QStudyTreeWidget::setMaximumExpandTreeItemsLevel(QStudyTreeWidget::ItemTreeLevels maximumExpandTreeItemsLevel)
{
    m_maximumExpandTreeItemsLevel = maximumExpandTreeItemsLevel;
}

QStudyTreeWidget::ItemTreeLevels QStudyTreeWidget::getMaximumExpandTreeItemsLevel()
{
    return m_maximumExpandTreeItemsLevel;
}

QString QStudyTreeWidget::formatAge(const QString age)
{
    QString text(age);

    if (text.length() > 0)
    {
        // Treiem el 0 de davant els anys, el PACS envia per ex: 047Y nosaltes tornem 47Y
        if (text.at(0) == '0')
        {
            text.replace(0, 1, " ");
        }
    }

    return text;
}

QString QStudyTreeWidget::formatDateTime(const QDate &date, const QTime &time)
{
    if (!date.isNull() && !time.isNull())
    {
        return date.toString(Qt::ISODate) + "   " + time.toString(Qt::ISODate);
    }
    else if (!date.isNull())
    {
        return date.toString(Qt::ISODate);
    }
    else
    {
        return "";
    }
}

void QStudyTreeWidget::clear()
{
    m_studyTreeView->clear();

    m_oldCurrentStudyUID = "";
    m_oldCurrentSeriesUID = "";

    qDeleteAll(m_addedImagesByDICOMItemID);
    qDeleteAll(m_adddSeriesByDICOMItemID);
    qDeleteAll(m_addedStudiesByDICOMItemID);

    m_addedStudiesByDICOMItemID.clear();
    m_adddSeriesByDICOMItemID.clear();
    m_addedImagesByDICOMItemID.clear();

    initialize();
}

void QStudyTreeWidget::setSortByColumn(QStudyTreeWidget::ColumnIndex col, Qt::SortOrder sortOrder)
{
    m_studyTreeView->sortItems(col, sortOrder);
    m_studyTreeView->clearSelection();
}

QStudyTreeWidget::ColumnIndex QStudyTreeWidget::getSortColumn()
{
    return (QStudyTreeWidget::ColumnIndex) m_studyTreeView->header()->sortIndicatorSection();;
}

Qt::SortOrder QStudyTreeWidget::getSortOrderColumn()
{
    return (Qt::SortOrder) m_studyTreeView->header()->sortIndicatorOrder();
}

QString QStudyTreeWidget::getCurrentStudyUID()
{
    if (m_studyTreeView->currentItem() != NULL)
    {
        if (isItemStudy(m_studyTreeView->currentItem()))
        {
            // És un estudi
            return m_studyTreeView->currentItem()->text(UID);
        }
        else if (isItemSeries(m_studyTreeView->currentItem()))
        {
            return m_studyTreeView->currentItem()->parent()->text(UID);
        }
        else if (isItemImage(m_studyTreeView->currentItem()))
        {
            return m_studyTreeView->currentItem()->parent()->parent()->text(UID);
        }
        else
        {
            return "";
        }
    }
    else
    {
        return "";
    }
}

//TODO No té en compte le DICOMSource
Study* QStudyTreeWidget::getStudy(QString studyInstanceUID)
{
    Study *study = NULL;

    // Busquem pels estudis UID seleccionats
    foreach (Study *studyInserted, m_addedStudiesByDICOMItemID)
    {
        if (studyInserted->getInstanceUID() == studyInstanceUID)
        {
            study = studyInserted;
            break;
        }
    }

    return study;
}

QString QStudyTreeWidget::getCurrentSeriesUID()
{
    if (m_studyTreeView->currentItem() != NULL)
    {
        if (isItemSeries(m_studyTreeView->currentItem()))
        {
            return m_studyTreeView->currentItem()->text(UID);
        }
        else if (isItemImage(m_studyTreeView->currentItem()))
        {
            return m_studyTreeView->currentItem()->parent()->text(UID);
        }
        else
        {
            return "";
        }
    }
    else
    {
        return "";
    }
}

QTreeWidgetItem* QStudyTreeWidget::getStudyQTreeWidgetItem(QString studyUID, DICOMSource studyDICOMSource)
{
    QList<QTreeWidgetItem*> qTreeWidgetItemsStudy(m_studyTreeView->findItems(studyUID, Qt::MatchExactly, UID));

    foreach(QTreeWidgetItem *studyItem, qTreeWidgetItemsStudy)
    {
        if (isItemStudy(studyItem) && m_addedStudiesByDICOMItemID[studyItem->text(DICOMItemID).toInt()]->getDICOMSource() == studyDICOMSource)
        {
            return studyItem;
        }
    }

    return NULL;
}

QTreeWidgetItem* QStudyTreeWidget::getSeriesQTreeWidgetItem(QString studyInstanceUID, QString seriesInstanceUID, DICOMSource seriesDICOMSource)
{
    QTreeWidgetItem *studyItem = getStudyQTreeWidgetItem(studyInstanceUID, seriesDICOMSource);

    if (!studyItem)
    {
        return NULL;
    }

    for (int index = 0; index < studyItem->childCount(); index++)
    {
        if (studyItem->child(index)->text(UID) == seriesInstanceUID)
        {
            return studyItem->child(index);
        }
    }

    return NULL;
}

QString QStudyTreeWidget::getCurrentImageUID()
{
    QString result;

    if (m_studyTreeView->currentItem() != NULL)
    {
        if (isItemImage(m_studyTreeView->currentItem()))
        {
            result = m_studyTreeView->currentItem()->text(UID);
        }
    }

    return result;
}

void QStudyTreeWidget::removeStudy(QString studyInstanceUIDToRemove)
{
    QList<QTreeWidgetItem*> qStudyList(m_studyTreeView->findItems(studyInstanceUIDToRemove, Qt::MatchExactly, UID));
    QTreeWidgetItem *item;

    for (int i = 0; i < qStudyList.count(); i++)
    {
        item = qStudyList.at(i);
        delete item;
    }

    m_studyTreeView->clearSelection();
    //No esborrem l'estudi del HashTable ja s'esborrarà quan netegem la HashTable
}

void QStudyTreeWidget::removeSeries(const QString &studyInstanceUID, const QString &seriesInstanceUID)
{
    foreach (QTreeWidgetItem *studyItem, m_studyTreeView->findItems(studyInstanceUID, Qt::MatchExactly, UID))
    {
        for (int index = 0; index < studyItem->childCount(); index++)
        {
            if (studyItem->child(index)->text(UID) == seriesInstanceUID)
            {
                if (studyItem->childCount() == 1)
                {
                    //Si l'estudi només té aquella sèrie ja la podem esborrar
                    delete studyItem;
                }
                else
                {
                    delete studyItem->takeChild(index);
                }
            }
        }

    }

    m_studyTreeView->clearSelection();
}

void QStudyTreeWidget::setCurrentSeries(QString seriesUID)
{
    // Busquem l'estudi a la que pertany la sèrie
    QList<QTreeWidgetItem*> qStudyList(m_studyTreeView->findItems(seriesUID, Qt::MatchRecursive, UID));

    // Només hauria de trobar una sèrie amb el mateix UID, sinó tindrem una inconsistència de DICOM, el series UID ha de ser únic
    if (qStudyList.count() > 0)
    {
        m_studyTreeView->setCurrentItem (qStudyList.at(0));
    }
}

void QStudyTreeWidget::sort()
{
    // Ordenem per la columna seleccionada
    m_studyTreeView->sortItems(m_studyTreeView->sortColumn(), Qt::AscendingOrder);
}

void QStudyTreeWidget::setContextMenu(QMenu *contextMenu)
{
    m_contextMenu = contextMenu;
}

void QStudyTreeWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if (!m_studyTreeView->selectedItems().isEmpty())
    {
        m_contextMenu->exec(event->globalPos());
    }
}

void QStudyTreeWidget::currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
    if (current != NULL)
    {
        if (getCurrentStudyUID() != m_oldCurrentStudyUID)
        {
            emit(currentStudyChanged());
            m_oldCurrentStudyUID = getCurrentStudyUID();
        }

        if (getCurrentSeriesUID() != m_oldCurrentSeriesUID)
        {
            m_oldCurrentSeriesUID = getCurrentSeriesUID();
            emit(currentSeriesChanged(m_oldCurrentSeriesUID));
        }

        // Sempre que canviem d'element segur que canviem d'imatge
        emit(currentImageChanged());
    }
    else
    {
        // Cas en que no hi cap item seleccionat
        emit notCurrentItemSelected();
    }
}

void QStudyTreeWidget::itemExpanded(QTreeWidgetItem *itemExpanded)
{
    // En el cas de que ens arribi l'item amb el text buit, no fem res
    // Això passa en situacions molt puntuals quan s'utilitza la tecla '*' per expandir l'item
    if (itemExpanded->text(UID).isEmpty())
    {
        return;
    }
    // El QTreeWidget després de fer doble click expandeix o col·lapsa l'item en funció del seu estat, a nosaltres no ens interessa
    // que es faci això, per aquest motiu en cas d'un signal de collapse o expand, el que fem és comprovar si per aquell item s'acaba
    // de fer doble click, si és així anul·lem l'acció de col·lapsar o expandir
    if (m_doubleClickedItemUID != itemExpanded->text(UID))
    {
        // Com que inserim un item en blanc per simular fills dels estudis i de les sèries cada vegada que ens fan un expand hem d'eliminar l'item en blanc i
        // emetem un signal per a que qui el reculli s'encarregui de fer els passos corresponents per expandir l'estudi o imatge amb el seus fills pertinents
        foreach (QTreeWidgetItem *childItem, itemExpanded->takeChildren())
        {
            delete childItem;
        }

        if (isItemStudy(itemExpanded))
        {
            // Canviem la icona per la de carpeta oberta quan l'item està expanded
            itemExpanded->setIcon(ObjectName, m_openFolder);

            Study *study = m_addedStudiesByDICOMItemID[itemExpanded->text(DICOMItemID).toInt()];
            emit (requestedSeriesOfStudy(study));
        }
        else if (isItemSeries(itemExpanded))
        {
            Series *series = m_adddSeriesByDICOMItemID[itemExpanded->text(DICOMItemID).toInt()];
            emit (requestedImagesOfSeries(series));
        }

        m_doubleClickedItemUID = "";
    }
    else
    {
        // Si s'ha fet un doble click a l'item hem d'anul·lar l'acció que ha fet qt de col·lapsar l'item per tan nosaltres el tornem a expandir
        // Molt important fer-lo abans de fer collapseItem, sinó entrariem en bucle pq s'emetria signal de collapseItem
        m_doubleClickedItemUID = "";
        m_studyTreeView->collapseItem(itemExpanded);
    }
}

void QStudyTreeWidget::itemCollapsed(QTreeWidgetItem *itemCollapsed)
{
    // El QTreeWidget després de fer doble click expandeix o col·lapsa l'item en funció del seu estat, a nosaltres no ens interessa
    // que es faci això, per aquest motiu en cas d'un signal de collapse o expand, el que fem és comprovar si per aquell item s'acaba
    // de fer doble click, si és així anul·lem l'acció de col·lapsar o expandir

    // Si l'item col·lapsat no se li acaba de fer un doble click
    if (m_doubleClickedItemUID != itemCollapsed->text(UID))
    {
        // Si és una estudi està collapsed, canviem la icona per la carpeta tancada
        if (isItemStudy(itemCollapsed))
        {
            itemCollapsed->setIcon(ObjectName, m_closeFolder);
        }

        m_doubleClickedItemUID = "";
    }
    else
    {
        // Si s'ha fet un doble click a l'item hem d'anul·lar l'acció que ha fet qt de col·lapsar l'item per tan nosaltres el tornem a expandir
        // Molt important fer-lo abans de fer expandItem, sinó entrariem en bucle, pq s'emetria signal d'expandItem
        m_doubleClickedItemUID = "";
        m_studyTreeView->expandItem (itemCollapsed);
    }

}

void QStudyTreeWidget::doubleClicked(QTreeWidgetItem *item, int)
{
    // Al fer doblec click al QTreeWidget ja expandeix o amaga automàticament l'objecte
    if (item == NULL)
    {
        return;
    }

    if (isItemStudy(item))
    {
        emit(studyDoubleClicked());
    }
    else if (isItemSeries(item))
    {
        emit(seriesDoubleClicked());
    }
    else if (isItemImage(item))
    {
        emit(imageDoubleClicked());
    }

    // Pel comportament del tree widget quan es fa un un doble click es col·lapsa o expandeix l'item en funció del seu estat, com que
    // nosaltres pel doble click no volem que s'expendeixi o es col·lapsi, guardem per quin element s'ha fet el doble click, per anul·laro quan es detecti
    // un signal d'expand o collapse item
    m_doubleClickedItemUID = item->text(UID);
}

bool QStudyTreeWidget::isItemStudy(QTreeWidgetItem *item)
{
    return item->text(Type) == "STUDY";
}

bool QStudyTreeWidget::isItemSeries(QTreeWidgetItem *item)
{
    return item->text(Type) == "SERIES";
}

bool QStudyTreeWidget::isItemImage(QTreeWidgetItem *item)
{
    return item->text(Type) == "IMAGE";
}

QString QStudyTreeWidget::paddingLeft(QString text, int length)
{
    QString paddedText;

    if (text.length() < length)
    {
        for (int index = text.length(); index < length; index++)
        {
            paddedText += " ";
        }
        paddedText += text;
    }
    else
    {
        paddedText = text;
    }

    return paddedText;
}

void QStudyTreeWidget::initialize()
{
    m_nextIDICOMItemIDOfStudy = 0;
    m_nextDICOMItemIDOfSeries = 0;
    m_nextDICOMItemIDOfImage = 0;
}

};
