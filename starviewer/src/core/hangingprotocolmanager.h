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

namespace udg {

class ViewersLayout;
class HangingProtocol;
class HangingProtocolImageSet;
class HangingProtocolDisplaySet;
class Patient;
class Series;
class Image;
class Q2DViewerWidget;
class Q2DViewer;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class HangingProtocolManager : public QObject
{
Q_OBJECT
public:

    HangingProtocolManager( QObject *parent = 0 );

    ~HangingProtocolManager();

	/// Buscar i aplicar el millor hanging protocol
    bool searchAndApplyBestHangingProtocol( ViewersLayout *layout, Patient *patient);

	// Aplicar un hanging protocol concret
	void applyHangingProtocol( int hangingProtocolNumber, ViewersLayout * layout, Patient * patient );

    /// TODO Mètode públic temporal per poder aplicar les transformacions desitjades per un viewer
    /// es fa especialment per no haver de fer noves classes ni duplicar el mapa de transdormacions
    /// i per poder aplicar-ho ràpidament sobre mamo. S'ha de plantejar bé on hauria d'anar tot això
    /// Donada la orientació actual i la desitjada, aplica sobre el viewer donat les transformacions pertinents
    /// El format de les orientacions és el mateix que el del DICOM, 2 strings separats per "\", 
    /// el primer indica la direcció de les rows i el segon la direcció de les columnes
    void applyDesiredDisplayOrientation(const QString &currentOrientation, const QString &desiredOrientation, Q2DViewer *viewer);

private:
    /// Mira si el protocol es pot aplicar al pacient
    bool isValid( HangingProtocol *protocol, Patient *patient);

    /// Buscar la sèrie corresponent a l'image set definit
    Series *searchSerie( Patient *patient, HangingProtocolImageSet *imageSet );

	/// Cert si la imatge compleix les restriccions
	bool isValidImage( Image *image, HangingProtocolImageSet *imageSet );

    /// Cert si la sèrie compleix les restriccions de l'imageSet, fals altrament
    bool isValidSerie( Patient *patient, Series *serie, HangingProtocolImageSet *imageSet );

	/// Aplicar les transformacions (rotacions, flips..) per mostrar per pantalla
	void applyDisplayTransformations( Patient *patient, Series *serie,  int imageNumber, Q2DViewerWidget *viewer, HangingProtocolDisplaySet *displaySet );

private:

	/// Map per guardar les operacions de rotacio i flip per aplicar a les imatges. Valors: String posicions, nombre de rotacions, nombre flips
	QMap<QString, QString> m_operationsMap;

};

}

#endif
