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
#include <QList>

// forward declarations
class QString;

namespace udg {

// forward declarations
class Study;
class Patient;
class Series;
class Image;

/** Aquesta classe  mostrar estudis i sèries d'una manera organitzada i fàcilment.
  * Aquesta classe és una modificació de la QTreeWidget que s'ha adaptat per poder visualitzar la informació 
  * de la cerca d'estudis, permetent consultar les series d'aquell estudi. 
  * Aquesta classe es sincronitza amb la informació mostrada a QSeriesListWidget
  * @author marc
  */
class QStudyTreeWidget : public QWidget , private Ui::QStudyTreeWidgetBase
{
Q_OBJECT
public:
    //Object Name s'utilitza per guardar El NomPacient, Serie + Identificador Sèrie i Imatge + Identificador Image
    enum ColumnIndex{ ObjectName = 0, PatientID = 1, PatientAge = 2, Description = 3, Modality = 4, Date = 5, Time = 6,
    PACSId = 7, Institution = 8, UID = 9, StudyID = 10, ProtocolName = 11, AccNumber = 12, Type = 13,
    RefPhysName = 14, PPStartDate = 15, PPStartTime = 16, ReqProcID = 17, SchedProcStep = 18
    };

    QStudyTreeWidget( QWidget *parent = 0 );
    ~QStudyTreeWidget();

    ///Mostrar els estudis passats per paràmetres (Els pacients passats per paràmetre ha de contenir només un estudi)
    void insertPatientList(QList<Patient*> patientList);

    ///Insereix el pacient al QStudyTreeWiget
    void insertPatient(Patient* patient);

    /** Insereix un llista de sèries a l'estudi seleccionat actualment
     * @param seriesList series afegir
     */
    void insertSeriesList(QString studyIstanceUID, QList<Series*> seriesList);

    /** Insereix una llista d'imatges a la sèrie seleccionada actualment
     * @param imageList llista d'imatges afegir a la sèrie
     */
    void insertImageList(QString studyInstanceUID, QString seriesInstanceUID, QList<Image*> imageList);

    /** removes study from the list
     * @param esbora l'estudi amb StudyUID de la llista
     */
    void removeStudy( QString StudyUID );

    /** Ordena per columna
     * @param Indica la columna per la que s'ha d'ordenar
     */
    void setSortColumn(int);

    /** Retorna el UID Study de l'estudi seleccionat
     * @return UID de l'estudi seleccionat
     */
    QString getCurrentStudyUID();

    ///Retorna una llista amb l'UID del estudis seleccionats
    QStringList getSelectedStudiesUID();

    ///Retorna una llista amb els estudis seleccionats
    QList<Study*> getSelectedStudies();

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

    /// Retorna el QTreeWidget que conté el widget
    QTreeWidget *getQTreeWidget() const;

protected:
    /** Mostra el menu contextual
     *  @param Dades de l'event sol·licitat
     */
    void contextMenuEvent(QContextMenuEvent *event);

signals :
    ///signal cada vegada que seleccionem un estudi diferent
    void currentStudyChanged();

    ///signal que s'emete quan canviem de sèrie seleccionada
    void currentSeriesChanged( const QString &seriesUID );

    ///signal que s'emet quan canviem d'imatge seleccionada
    void currentImageChanged();

    ///signal que s'emet quan es fa expandir un estudi
    void studyExpanded(QString studyUID);

    ///signal que s'emet qua es fa expandir una series
    void seriesExpanded(QString studyUID, QString seriesUID);

    ///signal que s'emet quan s'ha fet un doble click a un estudi
    void studyDoubleClicked();

    ///signal que s'emet quan s'ha fet un doble click a una sèrie
    void seriesDoubleClicked();

    ///signal que s'emet quan s'ha fet un doble click a una imatge
    void imageDoubleClicked();

    ///signal que s'emet quan es passa de tenir un item seleccionat a no tenir-ne cap de seleccionat
    void notCurrentItemSelected();

public slots:
    /** Indique que ens marqui la sèrie amb el uid passat per paràmetre com a seleccionada
     * @param SeriesUID Uid de la serie seleccionada en QSeriesListWidget
     */
    void setCurrentSeries( QString seriesUID );

    /// Neteja el TreeView
    void clear();

private slots:
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
    void createConnections();

    /** formata l'edat per mostrar per pantalla
     * @param edat
     */
    QString formatAge( const QString );

    /**Formata la data
     * @param data de l'estudi
     */
    QString formatDate(const QString &date);

    /**Formata l'hora
     * @param Hora de l'estudi
     */
    QString formatHour(const QString &hour);

    /** Retorna l'objecte TreeWidgetItem, que pertany a un estudi cercem, per studUID i PACS, ja que
     * un mateix estudi pot estar a més d'un PACS
     * @param studyUID uid de l'estudi a cercar
     */
    QTreeWidgetItem* getStudyItem(QString studyUID);

    ///Retorna l'Objecte QTtreeWidgeItem que és de l'estudi i series
    QTreeWidgetItem* getSeriesQTreeWidgetItem(QString studyUID, QString seriesUID);

private:
    ///Ens indica si l'item passat és un estudi
    bool isItemStudy( QTreeWidgetItem * );

    ///Ens indica si l'item passat és una sèrie
    bool isItemSeries( QTreeWidgetItem * );

    ///Ens indica si l'item passat és una imatge
    bool isItemImage( QTreeWidgetItem * );

    ///Afegeix espais a l'esquerre del text fins arribar l'allargada passada per paràmetre
    QString paddingLeft( QString text , int length );

    /**Dona una sèrie emplena un QTreeWidgetItem en format sèrie
     *@param informació de la serie
     */
    QTreeWidgetItem *fillSeries(Series *serie);

    ///Retorna llista QTreeWidgetItem resultant dels estudis que té el pacient
    QList<QTreeWidgetItem*> fillPatient(Patient *);

private:
    /// Menu contextual
    QMenu *m_contextMenu;

    /// Strings per guardar valors de l'anterior element
    QString m_oldCurrentStudyUID, m_oldCurrentSeriesUID, m_OldInstitution;

    QString m_doubleClickedItemUID;

    /// Icones utilitzades com a root al TreeWidget
    QIcon m_openFolder , m_closeFolder , m_iconSeries;

    QList<Patient*> m_insertedPatientList;
};

}; // end namespace

#endif
