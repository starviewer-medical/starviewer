/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGHANGINGPROTOCOLMANAGER_H
#define UDGHANGINGPROTOCOLMANAGER_H

#include <QObject>
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
class PreviousStudiesManager;

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class HangingProtocolManager : public QObject {
Q_OBJECT
public:
    
    HangingProtocolManager( QObject *parent = 0 );
    ~HangingProtocolManager();

    /// Buscar els hanging protocols disponibles
    QList<HangingProtocol *> searchHangingProtocols(Patient *patient);
    QList<HangingProtocol *> searchHangingProtocols(Patient *patient, const QList<Study *> &previousStudies, const QHash<QString, QString> &originOfPreviousStudies);

    // Aplica un hanging protocol concret, ja sigui via identificador o per instància
    void applyHangingProtocol( int hangingProtocolNumber, ViewersLayout * layout, Patient * patient );
    void applyHangingProtocol( HangingProtocol *hangingProtocol, ViewersLayout * layout, Patient * patient );

    /// Aplica el millor hanging protocol de la llista donada
    void setBestHangingProtocol(Patient *patient, const QList<HangingProtocol*> &hangingProtocolList, ViewersLayout *layout);

    /// Si hi havia estudis en descàrrega, s'elimina de la llista
    void cancelHangingProtocolDowloading();

private slots:

    /// S'ha descarregat un estudi previ demanat
    void previousStudyDownloaded();

    /// Slot que comprova si l'error a la descarrega d'un estudi és un dels que s'estava esperan
    void errorDowlonadingPreviousStudies(QString studyUID);

private:
    /// Mira si el protocol es pot aplicar al pacient
    bool isModalityCompatible(HangingProtocol *protocol, Patient *patient);

    /// Mira si la modalitat és compatible amb el protocol
    bool isModalityCompatible(HangingProtocol *protocol, const QString &modality);

    // Busca la sèrie corresponent dins un grup de sèries. Si el booleà quitStudy és cert, a més, l'eliminarà del conjunt
    Series *searchSerie( QList<Series *> &seriesList, HangingProtocolImageSet *imageSet, bool quitStudy, HangingProtocol * hangingProtocol );

    /// Cert si la imatge compleix les restriccions
    bool isValidImage(Image *image, HangingProtocolImageSet *imageSet);

    /// Cert si la sèrie compleix les restriccions de l'imageSet, fals altrament
    bool isValidSerie(Series *serie, HangingProtocolImageSet *imageSet);

    /// Aplicar les transformacions (rotacions, flips..) per mostrar per pantalla
    /// TODO Els paràmetres serie i imageNumber no es fan servir per res. Estan posats per una implementació futura o és que realment són innecessaris?
    void applyDisplayTransformations( Series *serie,  int imageNumber, Q2DViewerWidget *viewer, HangingProtocolDisplaySet *displaySet );

    /// Ordena els estudis per data per tal que els hanging protocols els tingui ordenats.
    QList<Study*> sortStudiesByDate( const QList<Study*> & studies );

    /// Mètode encarregat d'assignar l'input al viewer a partir de les especificacions del displaySet+imageSet.
    void setInputToViewer(Q2DViewerWidget *viewerWidget, Series *series, HangingProtocolImageSet *imageSet, HangingProtocolDisplaySet *displaySet);

    /// Buscar els estudis previs
    Study * searchPreviousStudy( HangingProtocol * protocol , Study * referenceStudy, const QList<Study*> &previousStudies);

private:

    /// Estructura per guardar les dades que es necessiten quan es rep que s'ha fusionat un pacient amb un nou estudi
    /// Hem de guardar tota la informació perquè només sabem que és un previ i fins que s'hagi descarregat no podem saber quines series i imatges te
    struct StructPreviousStudyDownloading
    {
        Q2DViewerWidget *widgetToDisplay; /// Widget a on s'ha de mostrar la informacio
        HangingProtocolImageSet * imageSet; /// HangingProtocolImageSet que s'ha de satisfer, per escollir la serie descarregada que s'aplica
        HangingProtocol * hangingProtocol; /// HangingProtocol necessari per buscar la millor serie de les descarregades
        HangingProtocolDisplaySet * displaySet; /// Guardem el display set per poder escollir l'orientacio (útil en mamo) i si cal una eina també
    };

    QMultiHash<QString, StructPreviousStudyDownloading*> * m_studiesDownloading;

    Patient * m_patient;

    /// Objecte utilitzat per descarregar estudis previs. No es fa servir QueryScreen per problemes de dependències entre carpetes.
    PreviousStudiesManager *m_previousStudiesManager;
};

}

#endif
