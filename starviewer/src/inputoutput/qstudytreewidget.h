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

#ifndef UDGQSTUDYTREEWIDGET_H
#define UDGQSTUDYTREEWIDGET_H

#include "ui_qstudytreewidgetbase.h"

#include <QMenu>
#include <QList>

#include "dicomsource.h"

// Forward declarations
class QString;

namespace udg {

// Forward declarations
class Study;
class Patient;
class Series;
class Image;
class DicomMask;

/**
    Aquesta classe mostrar estudis i sèries d'una manera organitzada i fàcilment.
    Aquesta classe és una modificació de la QTreeWidget que s'ha adaptat per poder visualitzar la informació de la cerca d'estudis/series/imatges.
    La classe manté la llista d'Study/Series/Image que se l'insereixen, i s'eliminen al invocar el mètode clean de la classe, per tant cal recordar
    que les classes que n'invoquin mètodes ue retornen punters a Study/Series/Image seran responsables de fer-ne una còpia si necessiten
    mantenir l'objecte viu una vegada fet un clean.

    Aquesta classe té dos comportaments en funció de setUseDICOMSourceToDiscriminateStudies, que ens permet establir si s'han de discriminar els estudis a part de amb el UID
    per DICOMSource també. En funció del lo establert es considerarà els estudis amb els mateix UID però diferent DICOMSource com un mateix estudi (duplicats) o s'els considesrarà
    com estudis diferents.
*/
class QStudyTreeWidget : public QWidget, private Ui::QStudyTreeWidgetBase {
Q_OBJECT
public:
    enum ItemTreeLevels { StudyLevel = 0, SeriesLevel = 1, ImageLevel = 2 };

    // Object Name s'utilitza per guardar El NomPacient, Serie + Identificador Sèrie i Imatge + Identificador Image
    enum ColumnIndex { ObjectName = 0, PatientID = 1, PatientAge = 2, Description = 3, Modality = 4, Date = 5, Time = 6,
    DICOMItemID = 7, Institution = 8, UID = 9, StudyID = 10, ProtocolName = 11, AccNumber = 12, Type = 13,
    RefPhysName = 14, PPStartDate = 15, PPStartTime = 16, ReqProcID = 17, SchedProcStep = 18, PatientBirth = 19 };

    explicit QStudyTreeWidget(QWidget *parent = nullptr);

    /// Indica si s'ha d'utilitzar el DICOMSource a part del InstanceUID per discriminar els estudis i considerar-lo dusplicats. Per defecte s'utilitza el DICOMSource per
    /// discrimnar els estudis
    void setUseDICOMSourceToDiscriminateStudies(bool discrimateStudiesByDicomSource);
    bool getUseDICOMSourceToDiscriminateStudies();

    /// Mostrar els estudis passats per paràmetres. Si algun dels estudis ja existeix en sobreescriu la informació. En funció del valor establert per setUseDICOMSourceToDiscriminateStudies
    /// estudis amb el mateix UID però diferent DICOMSource es podran considerar duplicats
    void insertPatientList(QList<Patient*> patientList);

    /// Insereix el pacient al QStudyTreeWiget. Si el pacient amb aquell estudi ja existeix en sobreescriu la informació
    void insertPatient(Patient *patient);

    /// Insereix un llista de sèries a l'estudi seleccionat actualment.
    void insertSeriesList(const QString &studyIstanceUID, QList<Series*> seriesList);

    /// Insereix una llista d'imatges a la sèrie seleccionada actualment
    void insertImageList(const QString &studyInstanceUID, const QString &seriesInstanceUID, QList<Image*> imageList);

    /// Removes study from the list
    void removeStudy(const QString &studyInstanceUIDToRemove, const DICOMSource &dicomSourceStudyToRemove = DICOMSource());

    /// Esborra la sèrie del QStudyTreeWidget, si és l'única sèrie de l'estudi també esborra l'estudi, no té sentit tenir una estudi sense
    ///  series al TreeWidget
    void removeSeries(const QString &studyInstanceUIDToRemove, const QString &seriesInstanceUIDToRemove, const DICOMSource &dicomSourceSeriesToRemove = DICOMSource());

    /// Per cada element seleccionat Study/Series/Imatge retorna la seva DicomMask. Si tenim el cas que tenim seleccionat un estudi, i d'aquest estudi tenim
    /// seleccionada dos sèries o alguna de les imatges només ens retonarà l'estudi, ja que l'acció que s'hagi de fer sobre els elements seleccionats
    /// si l'estudi està seleccionant no té sentit que es faci sobre les sèries i les imatges, passa el mateix si tenim seleccionada un sèrie i imatges d'aquesta.
    /// Per exemple si tenim seleccionat un estudi i dos de les seves sèries i l'usuari indica que vol descarregar-lo no té sentit descarregar l'estudi i llavors les dos sèries
    //Crec que QStudyTreeWidget no hauria de tenir coneixement de que per fer qualsevol acció sobre una font DICOM com DICOMDIR, BD, PACS...  necessita
    //un DICOMMask amb DICOMSource, però ara mateix necessitem aquest mètode ja que retorna ens retorna aglutinat a tots elements seleccionats independentment
    //de que siguin Study, Series, Image en un objecte comú el DICOMMask, sinó les classe que utilitzen aquest QStudyTreeWidget haurien de comprovar quins són els
    //estudis seleccionats, series seleccionades i imatges seleccionades invocant un mètode per cada tipus d'objecte.
    QList<QPair<DicomMask, DICOMSource> > getDicomMaskOfSelectedItems();

    /// Indica/Retorna la columna i direcció per la que s'ordena llista
    void setSortByColumn(QStudyTreeWidget::ColumnIndex sortColumn, Qt::SortOrder sortOrder);
    QStudyTreeWidget::ColumnIndex getSortColumn();
    Qt::SortOrder getSortOrderColumn();

    /// Ordena descendentment per la columna seleccionada
    void sort();

    /// Retorna l'estudi/series que tingui el UID i el DICOMSource passat per paràmetre. L'objecte retornat es destruirà quan s'invoqui el mètode
    /// clean d'aquesta classe, per tant si aquest objecte pot ser utilitzat després d'invocar el mètode clean és responsabilitat de la classe que el cridi
    /// fer-ne una còpia
    Study* getStudy(const QString &studyInstanceUID, const DICOMSource &dicomSourceOfStudy = DICOMSource());
    Series* getSeries(const QString &studyInstanceUID, const QString &seriesInstanceUID, const DICOMSource &dicomSourceOfSeries = DICOMSource());

    /// Estableix el menú contextual del Widget
    void setContextMenu(QMenu *contextMenu);

    /// Retorna el QTreeWidget que conté el widget
    QTreeWidget* getQTreeWidget() const;

    /// Assigna/Obté el nivell màxim fins el que es poden expandir els items que es mostren a QStudyTreeWiget, per defecte s'expandeix fins a nivell d'Image
    void setMaximumExpandTreeItemsLevel(QStudyTreeWidget::ItemTreeLevels maximumExpandTreeItemsLevel);
    QStudyTreeWidget::ItemTreeLevels getMaximumExpandTreeItemsLevel();

public slots:
    /// Indique que ens marqui la sèrie amb el uid passat per paràmetre com a seleccionada
    void setCurrentSeries(const QString &studyInstanceUID, const QString &seriesInstanceUID, const DICOMSource &dicomSource = DICOMSource());

    /// Neteja el TreeView, i esborra els Study*/Series*/Images* inserits
    void clear();

signals:
    /// Signal cada vegada que seleccionem un estudi diferent. L'Study passat com a paràmetre es destrueix al invocar el mètode clean, d'aquesta classe
    /// és responsanbilitat de la classe que rep el signal fer-ne una còpia per evitar problemes si ha d'utilitzar l'Study després després que s'hagi fet un clean
    void currentStudyChanged(Study *currentStudy);

    /// Signal que s'emete quan canviem de sèrie seleccionada. La Serie passada com a paràmetre es destrueix al invocar el mètode clean, d'aquesta classe
    /// és responsanbilitat de la classe que rep el signal fer-ne una còpia per evitar problemes si ha d'utilitzar l'Study després després que s'hagi fet un clean
    void currentSeriesChanged(Series *currentSeries);

    //TODO:Els signals de DoubleClicked no haurian de retornar l'estudi/series/imatge clickada =
    /// Signal que s'emet quan s'ha fet un doble click a un estudi
    void studyDoubleClicked();

    /// Signal que s'emet quan s'ha fet un doble click a una sèrie
    void seriesDoubleClicked();

    /// Signal que s'emet quan s'ha fet un doble click a una imatge
    void imageDoubleClicked();

    /// Signal que s'emet quan es passa de tenir un item seleccionat a no tenir-ne cap de seleccionat
    void notCurrentItemSelected();

    ///Ens indica quan un usuari ha sol·licitat veure les sèries/imatges d'un estudi/sèrie
    void requestedSeriesOfStudy(Study *studyRequestedSeries);
    void requestedImagesOfSeries(Series *seriesRequestedImage);

protected:
    /// Mostra el menu contextual
    void contextMenuEvent(QContextMenuEvent *event) override;

    void keyPressEvent(QKeyEvent*) override;

private:
    /// Crea les connexions dels signals i slots
    void createConnections();

    /// Inicialitza les variables necessàries del QWidget
    void initialize();

    /// Retorna l'objecte QTreeWidgetItem que mostra a l'estudi que compleix els paràmetres passats
    QTreeWidgetItem* getStudyQTreeWidgetItem(const QString &studyUID, const DICOMSource &studyDICOMSource);

    /// Retorna l'Objecte QTtreeWidgeItem que és de l'estudi i series
    QTreeWidgetItem* getSeriesQTreeWidgetItem(const QString &studyUID, const QString &seriesUID, const DICOMSource &seriesDICOMSource);

    /// Ens indica si l'item passat és un Study/Series/Image
    bool isItemStudy(QTreeWidgetItem *);
    bool isItemSeries(QTreeWidgetItem *);
    bool isItemImage(QTreeWidgetItem *);

    /// Retorna llista QTreeWidgetItem resultant dels estudis que té el pacient
    QList<QTreeWidgetItem*> fillPatient(Patient *);

    /// Donada una sèrie emplena un QTreeWidgetItem per mostrar la informaciío de la sèrie
    QTreeWidgetItem* fillSeries(Series *serie);

    /// Retorna Study/Series/Image a partir del seu DICOMItemID si no el troba retorna null
    Study* getStudyByDICOMItemID(int studyDICOMItemID);
    Series* getSeriesByDICOMItemID(int seriesDICOMItemID);
    Image* getImageByDICOMItemID(int imageDICOMItemID);

    /// Retorna Study/Series/Image a la que pertany un QTreeWidgeITem. Si a getStudyByQTreeWidgetItem li passem un QTreeWidgetItem que és una imatge ens retornarà l'estudi al
    /// que pertany la imatge,  si getSeriesByQTreeWidgetItem li passem un item que és un Study ens retornarà null
    Study* getStudyByQTreeWidgetItem(QTreeWidgetItem *qTreeWidgetItem);
    Series* getSeriesByQTreeWidgetItem(QTreeWidgetItem *qTreeWidgetItem);
    Image* getImageByQTreeWidgetItem(QTreeWidgetItem *qTreeWidgetItem);

    /// Formata l'edat per mostrar per pantalla
    QString formatAge(const QString &age) const;

    /// Formata la data i hora passada a ISO 8601 extended (YYYY-MM-DD HH:MM:SS) Amb aquest format de data es pot ordenar els estudis per data/hora
    /// Si l'hora no té valor només retorna la data, i si ni Data i Hora tenen valor retorna string buit
    QString formatDateTime(const QDate &date, const QTime &time) const;

    /// Crea un QTreeWidgetItem buit, per a que aparegui l'icona + per poder desplegar estudi/series amb el mouse
    QTreeWidgetItem* createDummyQTreeWidgetItem();

    /// Indica si el QTreeWidgetItem és un Dummy utilitzat per poder expandir Estudis/Series quan encara no n'hem consultat els seus elements fills Series/Imatges
    bool isDummyQTreeWidgetItem(QTreeWidgetItem *);

private slots:
    /// Emet signal quan es selecciona un estudi o serie diferent a l'anterior
    void currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    /// Emet signal quan s'expandeix un item, i no té items fills
    void itemExpanded(QTreeWidgetItem *itemExpanded);

    /// Emet signal quan es col·lapsa un item, i no té items fills
    void itemCollapsed(QTreeWidgetItem *itemCollapsed);

    /// Emet signal qua es fa doble click sobre un item
    void doubleClicked(QTreeWidgetItem *, int);

private:
    static const QString TypeTextForDummyExpandableItems;

    int m_nextIDICOMItemIDOfStudy;
    int m_nextDICOMItemIDOfSeries;
    int m_nextDICOMItemIDOfImage;

    QList<Patient*> m_addedPatients;
    QHash<int, Study*> m_addedStudiesByDICOMItemID;
    QHash<int, Series*> m_adddSeriesByDICOMItemID;
    QHash<int, Image*> m_addedImagesByDICOMItemID;

    /// Menu contextual
    QMenu *m_contextMenu;

    /// Strings per guardar valors de l'anterior element
    Study *m_oldCurrentStudy;
    Series *m_oldCurrentSeries;

    bool m_qTreeWidgetItemHasBeenDoubleClicked;
    bool m_useDICOMSourceToDiscriminateStudies;

    /// Icones utilitzades com a root al TreeWidget
    QIcon m_iconOpenStudy, m_iconCloseStudy, m_iconOpenSeries, m_iconCloseSeries, m_iconDicomFile;

    QStudyTreeWidget::ItemTreeLevels m_maximumExpandTreeItemsLevel;
};

} // end namespace

#endif
