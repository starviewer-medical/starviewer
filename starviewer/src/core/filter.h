/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGFILTER_H
#define UDGFILTER_H

#include <QObject>

namespace udg {

class Volume;


/*
	Classe base per a tots els filtres

*/


class Filter 
{

public:
	Filter();
	
	~Filter();

	/* Tal i com fan els filtres de vtk i itk li assignem les dades que en aquest cas serà un volum
	Aquest mètode, segons quin tipus de filtre sigui extreurà les dades itk o vtk */
	void setInput(Volume *input);

	/* Mètode per realitzar els càlculs amb la consegüent transformació del volum */
	void update();

	/* Metode per assignar un identificador */
	void setFilterName(QString name);

	QString getFilterName();

    /* Metode que retorna el volum transformat */
	Volume * getOutput();


protected:

	


	/* Volum d'entrada */
	Volume * m_inputVolume;

	/* Volum de sortida */
	Volume * m_outputVolume;

	/* Nom del filtre */
	QString m_filterName;

};

}

#endif