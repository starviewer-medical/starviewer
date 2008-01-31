/***************************************************************************
 *   Copyright (C) 2005 by marc                                            *
 *   marc@localhost                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef UDGQSTUDYTREEWIDGET_H
#define UDGQSTUDYTREEWIDGET_H

#include "ui_qstudytreewidgetbase.h"
#include <QMenu>

namespace udg {

/** Aquesta classe  mostrar estudis i sèries d'una manera organitzada i fàcilment.
  * Aquesta classe és una modificació de la QTreeWidget que s'ha adaptat per poder visualitzar la informació de la cerca d'estudis, permetent consultar les series d'aquell estudi. Aquesta classe es sincronitzar amb la informació mostrada a QSeriesListWidget
@author marc
*/

class StudyList;
class DICOMStudy;
class SeriesList;
class DICOMSeries;
class ImageList;
class DICOMImage;

class QStudyTreeWidget : public QWidget , private Ui::QStudyTreeWidgetBase
{
Q_OBJECT
public:
    //Object Name s'utilitza per guardar El NomPacient, Serie + Identificador Sèrie i Imatge + Identificador Image
    enum ColumnIndex{ ObjectName = 0, PatientID = 1, PatientAge = 2, Description = 3, Modality = 4, Date = 5, Time = 6,
    PACSAETitle = 7, Institution = 8, UID = 9, StudyID = 10, ProtocolName = 11, AccNumber = 12, Type = 13,
    ImageNumber = 14, RefPhysName = 15, PPStartDate = 16, PPStartTime = 17, ReqProcID = 18, SchedProcStep = 19
    };

    /// Constructor de la classe
    QStudyTreeWidget( QWidget *parent = 0 );

    /** Mostra l'estudi pel ListWidget que se li passa per paràmetre
     *  @param StudyList a mostrar
     */
    void insertStudyList( StudyList * studyList );

    /** Inseriex la informació d'un estudi
     * @param Dades de l'estudi
     */
    void insertStudy( DICOMStudy * );

    /** Insereix un llista de sèries a l'estudi seleccionat actualment
     * @param seriesList series afegir
     */
    void insertSeriesList( SeriesList *seriesList );

    /**Insereix una serie d'un estudi, i emiteix un signal al QSeriesListWidget per a insereixi també la informació de la sèrie
     *@param informació de la serie
     */
    void insertSeries( DICOMSeries *serie );

    /** Insereix una llista d'imatges a la sèrie seleccionada actualment
     * @param imageList llista d'imatges afegir a la sèrie
     */
    void insertImageList( ImageList *imageList );

    /** Insereix una image a la sèrie que està seleccionada
     * @param image imatge a afegir
     */
    void insertImage( DICOMImage *image );

    /** removes study from the list
     * @param esbora l'estudi amb StudyUID de la llista
     */
    void removeStudy( QString StudyUID );

    /** Ordena per columna
     * @param Indica la columna per la que s'ha d'ordenar
     */
    void setSortColumn(int);

    /**
     * Cerca l'AETitle del PACS de l'estudi indicat dins dels estudis seleccionats a la llista
     * @param studyUID uid de l'estudi que volem saber l'AETitle
     * @return Retorna l'AETitle de l'estudi indicat, sempre que aquest estigui dins dels seleccionats
     */
    QString getStudyPACSAETitleFromSelectedStudies( QString studyUID );

    /** Retorna el UID Study de l'estudi seleccionat
     * @return UID de l'estudi seleccionat
     */
    QString getSelectedStudyUID();

    QStringList getSelectedStudiesUID();

    QStringList getStudySelectedSeriesUIDFromSelectedStudies( QString studyUID );
    QStringList getStudySelectedImagesUIDFromSelectedStudies( QString studyUID );

    /** Retorna el UID de la sèrie seleccionada, si en aquell moment no hi ha cap sèrie seleccionada, retorna un QString buit
     *  @return UID de la sèrie seleccionat
     */
    QString getSelectedSeriesUID();

        /** Retorna el UID de la imatge seleccionada, si en aquell moment no hi ha cap imatge seleccionada, retorna un QString buit
     *  @return UID de la imatge seleccionada
     */
    QString getSelectedImageUID();

    /// guarda de la mida de les columnes
    void saveColumnsWidth();

    ///ordena descendentment per la columna seleccionada
    void sort();


    /// Destructor de la classe
    ~QStudyTreeWidget();

protected:
    /** Mostra el menu contextual
     *  @param Dades de l'event sol·licitat
     */
    void contextMenuEvent(QContextMenuEvent *event);

signals :
    ///que s'activa al fer click sobre un estudi, demanem a la queryScreen que busqui la informacio de les series d'aquell estudi
    void expandStudy( QString , QString );

    void expandSeries( QString AETitlePACS, QString StudyUID, QString SeriesUID );

    ///signal que s'emet quan es vol descarregar l'estudi seleccionat a la QStudyTreeView
    void retrieve();

    ///signal que s'emet quan es vol esborrar un estudi seleccionat a la QStudyTreeView
    void deleteSelectedStudies();

    ///signal que s'emet quan es vol veure un estudi seleccionat a la QStudyTreeView
    void view();

    ///signal que s'emet quan es vol afegir una serie al QSeriesListWidget
    void addSeries(DICOMSeries *serie);

    ///signal que s'emet per netejar el QSeriesListWidget
    void clearSeriesListWidget();

    /// signal que s'emet quan es vol convertir els estudis seleccionats al format DicomDir
    void convertToDicomDir( QStringList studiesUIDList );

    ///signal que s'emet quan es vol guardar els estudis seleccionats al PACS
    void storeStudiesToPacs( QStringList studiesUIDList );

public slots:
    /** Si es selecciona una serie del QSeriesListWidget s'ha seleccionar la mateixa en el QStudyTreeWidget, al seleccionar una serie del SeriesIconView, salta aquest slot i selecciona la serie de l'estudi seleccionada al SeriesIconView
     * @param SeriesUID Uid de la serie seleccionada en QSeriesListWidget
     */
    void selectedSeriesIcon( QString );

    /** Quant seleccionem una objecte de la llista, emet un signal cap SeriesListWidget, perque visualitzi
     * totes les series de l'objecte
     * @param item sobre el que s'ha fet click
     */
    void clicked( QTreeWidgetItem * , int );

    /**  Al fer doble click sobre un objecte de la llista l'expandeix o retreu en funcio de si esta
     * expandit o retre
     * @param item sobre el que s'ha fet click
     */
    void doubleClicked( QTreeWidgetItem * , int );

    /// Neteja el TreeView
    void clear();

    ///Slot que converteix un estudi
    void createDicomDir();

    ///Slot que guardar els estudis seleccionats al PACS
    void storeStudies();

private:
    /// crea les connexions dels signals i slots
    void createConnections( );

    /// Assigna l'ampla a les columnes segons els paràmetres guardats a StarviewerSettings
    void setWidthColumns( );

    /// Creem el menu contextual, en funcio de a quin tab pertany activa unes o altres opcions del menu
    void createContextMenu( );

    /** Quant es consulten les sèries d'un estudi, es fa un acces al pacs demanant la informació d'aquelles series,si es tornen a consultar una segona vegada les sèries de l'estudi,no cal tornar a accedir al pacs perquè ja tenim la informació de la sèrie al TreeView, però s'ha d'actualitzar QSeriesListWidget amb  la informació de les sèries de l'estudi, com no tornem a accedir al pacs, la informació de les sèries li hem de passar d'algun mode, per això el que fem és invocar aquest mètode que crea reconstrueix l'objecte series, amb la principal informació de les sèries, i que fa un emit, que és capturat pel QSeriesInconView, per mostrar la informació de la sèrie (la connexió entre el QStudyTreeWidget i QSeriesListWidget es fa la constrcutor de la QueryScreen)
     * @param Apuntador a l'estudi al tree view
     */
    void setSeriesToSeriesListWidget( QTreeWidgetItem *item );

    /** formata l'edat per mostrar per pantalla
     * @param edat
     */
    QString formatAge( const QString );

    /**Formata la data
     * @param data de l'estudi
     */
    QString formatDate( const QString );

    /**Formata l'hora
     * @param Hora de l'estudi
     */
    QString formatHour( const QString );

    /** Retorna l'objecte TreeWidgetItem, que pertany a un estudi cercem, per studUID i PACS, ja que
     * un mateix estudi pot estar a més d'un PACS
     * @param studyUID uid de l'estudi a cercar
     * @param AETitle AEtitle de la màquina on està l'estudi
     */
    QTreeWidgetItem* getStudyItem( QString studyUID , QString AETitle );

private:
    /// Menu contextual
    QMenu m_contextMenu;

    /// strings per guardar valors de l'anterior element
    QString m_parentName , m_oldPacsAETitle , m_OldInstitution;

    QIcon m_openFolder , m_closeFolder , m_iconSeries;///< icones utilitzades com a root al TreeWidget

    /// Accions del menú de contexte
    QAction *m_viewAction, *m_retrieveAction, *m_deleteStudyAction, *m_sendToDICOMDIRListAction, *m_storeStudyAction;

};

}; // end namespace

#endif
