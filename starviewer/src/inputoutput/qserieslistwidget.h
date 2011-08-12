#ifndef UDGSERIESLISTWIDGET_H
#define UDGSERIESLISTWIDGET_H

#include "ui_qserieslistwidgetbase.h"
namespace udg {

class Series;

/**
    Aquesta classe és un widget millorat del ListWidget de QT, fet  modificat per poder mostrar les sèries d'un estudi.
  */
class QSeriesListWidget : public QWidget, private Ui::QSeriesListWidgetBase {
Q_OBJECT
public:
    /// Constructor de la classe
    QSeriesListWidget(QWidget *parent = 0);

    /// Insereix l'informació d'una sèrie
    void insertSeries(QString studyInstanceUID, Series *series);

    QString getCurrentSeriesUID();

    /// Retorna l'estudi uid de la sèrie seeleccionada
    QString getCurrentStudyUID();

    /// Esborra de la llista la serie amb el UID passat per paràmetre
    void removeSeries(const QString &seriesInstanceUID);

public slots:
    /// Estableix quina és la série seleccionada
    void setCurrentSeries(const QString &seriesUID);

    void clear();

signals:
    /// Quan seleccionem una sèrie emeiteix una signal per a que el QStudyTreeWidget, tingui seleccionada la mateixa sèrie
    void selectedSeriesIcon(const QString studyInstanceUID, const QString seriesInstanceUID);

    /// Quan es fa doble click emiteix un signal, perquè la sèrie sigui visualitzada
    void viewSeriesIcon();

private:
    /// Crea les connexions
    void createConnections();

private slots:
    /// Slot que s'activa quan s'ha fet click sobre un thumbnail
    void seriesClicked(QString IDThumbnail);

    /// Slot que s'activa quan s'ha fet doble click sobre un thumbnail
    void seriesDoubleClicked(QString IDThumbnail);

private:

    /// Guardem per cada sèrie a quin estudi pertany
    QHash<QString, QString> m_HashSeriesStudy;
    /// Modalitats de sèries que no són images, com (KO, PR, SR)
    QStringList m_nonDicomImageSeriesList;
    // Indica a quina ha estat la última fila que hem inseritat una sèrie que era una imatge
    int m_lastInsertedImageRow;

};

};

#endif
