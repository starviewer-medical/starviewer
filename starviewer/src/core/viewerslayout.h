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
    Classe que permet distribuir sobre un widget una sèrie Q2DViewerWidgets 
    amb diferents layouts i geometries de forma versàtil.

    @author Grup de Gràfics de Girona  (GGG) <vismed@ima.udg.es>
*/
class ViewersLayout : public QWidget {
Q_OBJECT
public:
    ViewersLayout(QWidget *parent = 0);
    ~ViewersLayout();

    /// Obtenir el visualitzador seleccionat
    Q2DViewerWidget* getSelectedViewer();

    /// Ens retorna en nombre de viewers totals que conté el gestor de layouts,
    /// independentment de si són visibles o no.
    int getNumberOfViewers() const;

    /// Ens retorna el visor amb índex "number". Si number està fora de rang, 
    /// ens retornarà un punter nul.
    Q2DViewerWidget* getViewerWidget(int number);

public slots:
    /// Canviar el nombre de files i columnes
    void addColumns(int columns = 1);
    void addRows(int rows = 1);
    void removeColumns(int columns = 1);
    void removeRows(int rows = 1);
    void setGrid(int rows, int columns);
    void showRows(int rows);
    void hideRows(int rows);
    void showColumns(int columns);
    void hideColumns(int columns);

    /// Afegeix un nou visualitzador amb la geometria indicada
    Q2DViewerWidget* addViewer(const QString &geometry);

    /// Marquem com a seleccionat el viewer passat per paràmetre
    void setSelectedViewer(Q2DViewerWidget *viewer);

    /// Neteja el layout, eliminant tots els visors i geometries, 
    /// deixant-lo en l'estat inicial, com si acabéssim de crear l'objecte
    void cleanUp();

signals:
    /// Senyal que s'emet quan s'afegeix un visualitzador
    void viewerAdded(Q2DViewerWidget *viewer);

    /// Senyal que s'emet quan s'amaga un visualitzador, és a dir, 
    // deixa de ser visible però segueix existint
    void viewerHidden(Q2DViewerWidget *viewer);

    /// Senyal que s'emet quan el visualitzador seleccionat canvia
    void selectedViewerChanged(Q2DViewerWidget *viewer);

protected:
    /// Tractament de l'event de canvi de tamany de la finestra
    /// Quan rebem aquest event, redimensionem els viewers amb la geometria adequada
    void resizeEvent(QResizeEvent *event);

private:
    /// Crea i retorna un nou visor configurat adequadament
    Q2DViewerWidget* getNewQ2DViewerWidget();

    /// Coloca el viewer donat en la posició i mides proporcionats
    /// @param viewer Visor que volem posicionar i ajustar dins del layout
    /// @param geometry String amb les posicions i mides realitives corresponents al viewer
    /// El format de geometry seran les coordenades x,y de la cantonada esquerra superior i 
    /// ample i alçada del visor, expresats com a valors dins del rang 0.0..1.0. Cada valor anirà separat per '\\'
    /// Per exemple, un viewer que ocupa la meitat de la pantalla s'expressaria amb un string "0\\0\\0.5\\1.0"
    void setViewerGeometry(Q2DViewerWidget *viewer, const QString &geometry);

    /// Inicialitza els objectes que fem servir per distribuir els visors
    /// Només cal cridar-lo al constructor
    void initLayouts();

    /// Fa les accions necessàries per amagar el viewer indicat del layout actual
    void hideViewer(Q2DViewerWidget *viewer);

private:
    /// Grid per gestionar les distribucions regulars de visors
    QGridLayout *m_viewersLayout;

    /// Visualitzador selecciona. Sempre en tindrem un.
    Q2DViewerWidget *m_selectedViewer;

    /// Nombre de files i columnes pels layouts
    int m_visibleRows;
    int m_visibleColumns;
    int m_totalRows;
    int m_totalColumns;

    /// Array amb tots els viewers que podem manipular
    /// Visors dins del grid regular (distribuits dins del gridLayout)
    QVector<Q2DViewerWidget *> m_vectorViewers;
    /// Visors definits amb geometries lliures (distribuits fora del gridLayout)
    QList<Q2DViewerWidget *> m_freeLayoutViewersList;

    /// Llistat de geometries que cada viewer visible té assignada
    QStringList m_geometriesList;

    /// Indica si el layout s'ha definit de forma regular (setGrid(rows, columns)) o no 
    bool m_isRegular;
};

}

#endif
