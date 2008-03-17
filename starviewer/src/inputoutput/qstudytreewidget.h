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

class QString;

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
    RefPhysName = 14, PPStartDate = 15, PPStartTime = 16, ReqProcID = 17, SchedProcStep = 18
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
    QString getCurrentStudyUID();

    ///Retorna una llista amb l'UID del estudis seleccionats
    QStringList getSelectedStudiesUID();

    ///Retorna una llista amb l'UID de les sèries dels estudis seleccionats
    QStringList getStudySelectedSeriesUIDFromSelectedStudies( QString studyUID );

    ///Retorna una llista amb l'UID de les imatges dels estudis seleccionats
    QStringList getStudySelectedImagesUIDFromSelectedStudies( QString studyUID );

    /** Retorna el UID de la sèrie seleccionada, si en aquell moment no hi ha cap sèrie seleccionada, retorna un QString buit
     *  @return UID de la sèrie seleccionat
     */
    QString getCurrentSeriesUID();

        /** Retorna el UID de la imatge seleccionada, si en aquell moment no hi ha cap imatge seleccionada, retorna un QString buit
     *  @return UID de la imatge seleccionada
     */
    QString getCurrentImageUID();

    ///ordena descendentment per la columna seleccionada
    void sort();

    ///Estableix el menú contextual del Widget
    void setContextMenu( QMenu *contextMenu );

    ///Retorna el número de columnes del tree view
    int getNumberOfColumns();

    /** Assigna l'amplada a les columnes segons el paràmetre passat
     * @param columnNumber número de columna 
     * @param columnWidth amplada de la columna
     */
    void setColumnWidth( int columnNumber , int columnWidth );

    /** Retorna l'amplada de la columna
     * @param columnNumber número de columna
     * @return amplada de la columna 
     */
    int getColumnWidth( int columnNumber );

    /// Destructor de la classe
    ~QStudyTreeWidget();

protected:
    /** Mostra el menu contextual
     *  @param Dades de l'event sol·licitat
     */
    void contextMenuEvent(QContextMenuEvent *event);

signals :
    ///signal cada vegada que seleccionem un estudi diferent
    void currentStudyChanged();

    ///signal que s'emete quan canviem de sèrie seleccionada
    void currentSeriesChanged();

    ///signal que s'emet quan canviem d'imatge seleccionada
    void currentImageChanged();

    ///signal que s'emet quan es fa expandir un estudi
    void studyExpanded( QString studyUID , QString pacsAETitle );

    ///signal que s'emet qua es fa expandir una series
    void seriesExpanded(QString studyUID , QString seriesUID , QString pacsAETitle );

    ///signal que s'emet quan s'ha fet un doble click a un estudi
    void studyDoubleClicked();

    ///signal que s'emet quan s'ha fet un doble click a una sèrie
    void seriesDoubleClicked();

    ///signal que s'emet quan s'ha fet un doble click a una imatge
    void imageDoubleClicked();

public slots:
    /** Indique que ens marqui la sèrie amb el uid passat per paràmetre com a seleccionada
     * @param SeriesUID Uid de la serie seleccionada en QSeriesListWidget
     */
    void setCurrentSeries( QString seriesUID );

    /// Neteja el TreeView
    void clear();

private slots :

    ///Emet signal quan es selecciona un estudi o serie diferent a l'anterior
    void currentItemChanged( QTreeWidgetItem * current, QTreeWidgetItem * previous );

    ///Emet signal quan s'expandeix un item, i no té items fills
    void itemExpanded( QTreeWidgetItem *itemExpanded );

    ///Emet signal quan es col·lapsa un item, i no té items fills
    void itemCollapsed( QTreeWidgetItem *itemCollapsed );

    ///Emet signal qua es fa doble click sobre un item
    void doubleClicked( QTreeWidgetItem * , int );

private:
    /// crea les connexions dels signals i slots
    void createConnections( );

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
    QMenu *m_contextMenu;

    /// strings per guardar valors de l'anterior element
    QString m_oldCurrentStudyUID, m_oldCurrentSeriesUID, m_oldPacsAETitle , m_OldInstitution;

    QIcon m_openFolder , m_closeFolder , m_iconSeries;///< icones utilitzades com a root al TreeWidget

    ///Ens indica si l'item passat és un estudi
    bool isItemStudy( QTreeWidgetItem * );

    ///Ens indica si l'item passat és una sèrie
    bool isItemSeries( QTreeWidgetItem * );

    ///Ens indica si l'item passat és una imatge
    bool isItemImage( QTreeWidgetItem * );
};

}; // end namespace

#endif
