/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGHANGINGPROTOCOLMANAGER_H
#define UDGHANGINGPROTOCOLMANAGER_H

#include <QObject>
#include <QMultiMap>

namespace udg {

class ViewersLayout;
class HangingProtocol;
class HangingProtocolImageSet;
class HangingProtocolDisplaySet;
class Patient;
class Series;
class Image;
class Q2DViewerWidget;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class HangingProtocolManager : public QObject
{
Q_OBJECT
public:

    HangingProtocolManager( QObject *parent = 0 );

    ~HangingProtocolManager();

    void searchAndApplyBestHangingProtocol( ViewersLayout * layout, Patient * patient);

private:

    /// Mira si el protocol es pot aplicar al pacient
    bool isValid( HangingProtocol * protocol, Patient * patient);

    /// Buscar la sèrie corresponent a l'image set definit
    Series * searchSerie( Patient * patient, HangingProtocolImageSet * imageSet );

	/// Cert si la imatge compleix les restriccions
	bool isValidImage( Image * image, HangingProtocolImageSet * imageSet );

    /// Cert si la sèrie compleix les restriccions de l'imageSet, fals altrament
    bool isValidSerie( Series * serie, HangingProtocolImageSet * imageSet );

	/// Aplicar les transformacions (rotacions, flips..) per mostrar per pantalla
	void applyDisplayTransformations( Patient * patient, Series * serie, Q2DViewerWidget * viewer, HangingProtocolDisplaySet * displaySet );

private:

	/// Map per guardar les operacions de rotacio i flip per aplicar a les imatges. Valors: String posicions, nombre de rotacions, nombre flips
	//QMultiMap<QString, int, int> m_operationsMap;

};

}

#endif
