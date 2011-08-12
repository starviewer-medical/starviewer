#ifndef UDGQTHUMBNAILSPREVIEWWIDGET
#define UDGQTHUMBNAILSPREVIEWWIDGET

#include "ui_qthumbnailsspreviewwidgetbase.h"
#include <QWidget>
#include <QHash>
#include <QStringList>

class QPixmap;
class QListWidgetItem;

namespace udg {


/**
    Aquesta classe és un widget quen ens permet la previsualització de thumbnails. Hi podem afegir thumbnails, treure, saber quin és el seleccionat,...
  */
class QThumbnailsPreviewWidget : public QWidget, private Ui::QThumbnailsPreviewWidgetBase {
Q_OBJECT
public:
    /// Constructor de la classe
    QThumbnailsPreviewWidget(QWidget *parent = 0);

    /// Ens afegeix el thumbnail al final. El paràmetre ID és un identificador únic del thumbnail.
    void append(QString IDThumbnail, const QPixmap &thumbnail, QString thumbnailDescription);

    /// Ens insereix el thumbnail a la posició especificada. Si la posició és més gran que el número d'elements actuals, l'insereix al final
    void insert(int position, QString IDThumbnail, const QPixmap &thumbnail, QString thumbnailDescription);

    /// Treu el thumbnail de la previsualització.
    void remove(QString IDThumbnail);

    /// Selecciona el Thumbnail amb l'ID passat
    void setCurrentThumbnail(QString IDThumbnail);

    /// Retorna el ID del Thumbnail Seleccionat
    QString getSelectedThumbnailID();

public slots:
    /// Neteja el ListWidget de sèries
    void clear();

signals:
    /// Signal que s'emet quan han clickat un thumbnail
    void thumbnailClicked(const QString &IDThumbnail);
    /// Signal que s'emet quan han fet doble click sobre un thumbnail
    void thumbnailDoubleClicked(const QString IDThumbnail);

private:
    /// Ens crea un QListWidgetItem a partir de les dades passades
    QListWidgetItem* createQListWidgetItem(QString IDThumbnail, const QPixmap &thumbnail, QString thumbnailDescription);

    /// Retorna un getQListWidgetItem a partir del seu ID, si no el troba retorna null
    QListWidgetItem *getQListWidgetItem(QString IDThumbnail);

    /// Crea les connexions dels signals i slots
    void createConnections();

private slots:
    /// Slot que s'activa quant es selecciona una serie, emiteix signal a QStudyTreeWidget, perquè selecciona la mateixa serie que el QSeriesListWidget
    ///  @param serie Seleccionada
    void itemClicked(QListWidgetItem *item);

    /// Slot que s'activa quant es fa doblec
    ///  @param item de la serie Seleccionada
    void itemDoubleClicked(QListWidgetItem *item);

private:
    /// Mida de la imatge escalada a l'eix x
    static const int scaledSeriesSizeX;
    /// Mida de la imatge escalada a l'eix y
    static const int scaledSeriesSizeY;

    /// Guardem per cada sèrie a quin estudi pertany
    QHash<QString, QString> m_HashSeriesStudy;
    /// Modalitats de sèries que no són images, com (KO, PR, SR)
    QStringList m_nonDicomImageSeriesList;

    // Indica a quina ha estat la última fila que hem inseritat una sèrie que era una imatge
    int m_lastInsertedImageRow;
};

}

#endif
