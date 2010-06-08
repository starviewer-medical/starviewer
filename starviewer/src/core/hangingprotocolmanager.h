/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGHANGINGPROTOCOLMANAGER_H
#define UDGHANGINGPROTOCOLMANAGER_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QMultiHash>
#include <QProgressDialog>

namespace udg {

class ViewersLayout;
class HangingProtocol;
class HangingProtocolImageSet;
class HangingProtocolDisplaySet;
class Patient;
class Series;
class Study;
class Image;
class Q2DViewerWidget;
class Q2DViewer;

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class HangingProtocolManager : public QObject {
Q_OBJECT
public:
    
    HangingProtocolManager( QObject *parent = 0 );
    ~HangingProtocolManager();

    /// Buscar els hanging protocols disponibles, si applyBestHangingProtocol, a més, aplica el millor que ha trobat
    QList<HangingProtocol * > searchHangingProtocols( ViewersLayout *layout, Patient *patient, bool applyBestHangingProtocol, int *indexOfBestHangingProtocol = NULL);

    // Aplica un hanging protocol concret, ja sigui via identificador o per instància
    void applyHangingProtocol( int hangingProtocolNumber, ViewersLayout * layout, Patient * patient );
    void applyHangingProtocol( HangingProtocol *hangingProtocol, ViewersLayout * layout, Patient * patient );

    /// TODO Mètode públic temporal per poder aplicar les transformacions desitjades per un viewer
    /// es fa especialment per no haver de fer noves classes ni duplicar el mapa de transformacions
    /// i per poder aplicar-ho ràpidament sobre mamo. S'ha de plantejar bé on hauria d'anar tot això
    /// Donada la orientació actual i la desitjada, aplica sobre el viewer donat les transformacions pertinents
    /// El format de les orientacions és el mateix que el del DICOM, 2 strings separats per "\",
    /// el primer indica la direcció de les rows i el segon la direcció de les columnes
    void applyDesiredDisplayOrientation(const QString &currentOrientation, const QString &desiredOrientation, Q2DViewer *viewer);

    /// Buscar hanging protocols quan es sap que hi ha previes
    QList<HangingProtocol * > getHangingProtocolsWidthPreviousSeries( Patient * patient, QList<Study*> previousStudies, QHash<QString, QString> pacs );

    /// Buscar els estudis previs
    Study * searchPreviousStudy( HangingProtocol * protocol , Study * referenceStudy, QList<Study*> previousStudies);

    /// Si hi havia estudis en descàrrega, s'elimina de la llista
    void cancelHangingProtocolDowloading();

private slots:

    /// S'ha descarregat un estudi previ demanat
    void previousStudyDownloaded();

    /// Slot que comprova si l'error a la descarrega d'un estudi és un dels que s'estava esperan
    void errorDowlonadingPreviousStudies(QString studyUID);

private:
    /// Mira si el protocol es pot aplicar al pacient
    bool isValid( HangingProtocol *protocol, Patient *patient);

    // Busca la sèrie corresponent dins un grup de sèries. Si el booleà quitStudy és cert, a més, l'eliminarà del conjunt
    Series *searchSerie( QList<Series *> &seriesList, HangingProtocolImageSet *imageSet, bool quitStudy, HangingProtocol * hangingProtocol );

    /// Cert si la imatge compleix les restriccions
    bool isValidImage( Image *image, HangingProtocolImageSet *imageSet, HangingProtocol * hangingProtocol );

    /// Cert si la sèrie compleix les restriccions de l'imageSet, fals altrament
    bool isValidSerie( Series *serie, HangingProtocolImageSet *imageSet, HangingProtocol * hangingProtocol );

    /// Aplicar les transformacions (rotacions, flips..) per mostrar per pantalla
    /// TODO Els paràmetres serie i imageNumber no es fan servir per res. Estan posats per una implementació futura o és que realment són innecessaris?
    void applyDisplayTransformations( Series *serie,  int imageNumber, Q2DViewerWidget *viewer, HangingProtocolDisplaySet *displaySet );

    /// Ordena els estudis per data per tal que els hanging protocols els tingui ordenats.
    QList<Study*> sortStudiesByDate( QList<Study*> studies );

    /// Crea el widget que posara al visor on s'estan descarregant estudis
    QWidget * createDownloadingWidget( ViewersLayout *layout );

    /// Mètode encarregat d'assignar l'input al viewer a partir de les especificacions del displaySet+imageSet.
    void setInputToViewer(Q2DViewerWidget *viewerWidget, Series *series, HangingProtocolImageSet *imageSet, HangingProtocolDisplaySet *displaySet);

private:

    /// Estructura per guardar les dades que es necessiten quan es rep que s'ha fusionat un pacient amb un nou estudi
    /// Hem de guardar tota la informació perquè només sabem que és un previ i fins que s'hagi descarregat no podem saber quines series i imatges te
    struct StructPreviousStudyDownloading
    {
        Q2DViewerWidget *widgetToDisplay; /// Widget a on s'ha de mostrar la informacio
        QWidget *downloadingWidget; /// Widget que es crea quan s'esta esperant
        ViewersLayout * layout; /// Layout d'on hem de treure el downloadingWidget per posar-hi el widgetToDisplay
        HangingProtocolImageSet * imageSet; /// HangingProtocolImageSet que s'ha de satisfer, per escollir la serie descarregada que s'aplica
        HangingProtocol * hangingProtocol; /// HangingProtocol necessari per buscar la millor serie de les descarregades
        HangingProtocolDisplaySet * displaySet; /// Guardem el display set per poder escollir l'orientacio (útil en mamo) i si cal una eina també
    };

    /// Map per guardar les operacions de rotacio i flip per aplicar a les imatges. Valors: String posicions, nombre de rotacions, nombre flips
    QMap<QString, QString> m_operationsMap;

    QMultiHash<QString, StructPreviousStudyDownloading*> * m_studiesDownloading;

    Patient * m_patient;
};

}

#endif
