/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGVIEWERSLAYOUT_H
#define UDGVIEWERSLAYOUT_H

#include "q2dviewerwidget.h"

#include <QWidget>

class QResizeEvent;

namespace udg {

/**
    Classe que permet crear un widget de visualitzadors amb diferents layouts

	@author Grup de Gràfics de Girona  (GGG) <vismed@ima.udg.es>
*/
class ViewersLayout : public QWidget {
Q_OBJECT
public:
    ViewersLayout(QWidget *parent = 0);
    ~ViewersLayout();

    /// Obtenir el visualitzador seleccionat
    Q2DViewerWidget* getSelectedViewer();

    /// Obtenir el nombre de visualitzadors
    int getNumberOfViewers();

    /// Obtenir el visualitzador numero "number".
    Q2DViewerWidget* getViewerWidget(int number);

public slots:
    ///Canviar el nombre de files i columnes
    void addColumns(int columns = 1);
    void addRows( int rows = 1);
    void removeColumns(int columns = 1);
    void removeRows(int rows = 1);
    void setGrid(int rows, int columns);
    void showRows(int rows);
    void hideRows(int rows);
    void showColumns(int columns);
    void hideColumns(int columns);
    void setGrid(const QStringList &positionsList);

    /// Afegeix un nou visualitzador
    Q2DViewerWidget* addViewer(const QString &position);

    /// Posem el widget seleccionat com a actual
    void setSelectedViewer(Q2DViewerWidget *viewer);

signals:
    /// Senyal que s'emet quan s'afegeix un visualitzador
    void viewerAdded(Q2DViewerWidget *viewer);

    /// Senyal que s'emet quan s'amaga un visualitzador
    void viewerRemoved(Q2DViewerWidget *viewer);

    /// Senyal que s'emet quan el visualitzador seleccionat canvia
    void selectedViewerChanged(Q2DViewerWidget *viewer);

protected:
	/// Tractament de l'event de canvi de tamany de la finestra
    void resizeEvent(QResizeEvent *event);

private:
    /// Obtenir un nou visualitzador
	Q2DViewerWidget* getNewQ2DViewerWidget();

    /// Restaurar els layouts
    void restoreLayouts();

private slots:
    /// Inicialitza els layouts
    void initLayouts();

    /// Elimina els layouts
    void removeLayouts();

private:
    /// Widget contenidor general
    QWidget *m_workingArea;

    /// Grids per mostrar diferents q2dviewers alhora.
    QGridLayout *m_gridLayout;
    QGridLayout *m_viewersLayout;

    /// Visualitzador seleccionat, també sempre en tindrem un
    Q2DViewerWidget *m_selectedViewer;

    /// Nombre de files i columnes pels layouts
    int m_visibleRows;
    int m_visibleColumns;
    int m_totalRows;
    int m_totalColumns;

    /// Renderers que tenim
    QVector<Q2DViewerWidget *> m_vectorViewers;

    QStringList m_positionsList;

    /// Grid regular o no regular
    bool m_isRegular;

    /// Nombre de visualitzadors visibles
    int m_numberOfVisibleViewers;
};

}

#endif
