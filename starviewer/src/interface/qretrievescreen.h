/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGQRETRIEVESCREEN_H
#define UDGQRETRIEVESCREEN_H

#include <QString>
#include <semaphore.h>

#include "study.h"
#include "image.h"
#include "starviewerprocessimage.h"
#include "ui_qretrievescreenbase.h"

/// Interfície que implementa la llista d'operacions realitzades cap a un PACS 
namespace udg {

class Status;

class QRetrieveScreen : public QDialog , private Ui::QRetrieveScreenBase{
Q_OBJECT
public:

	/** Constructor de la classe
	 * @param parent 
	 * @return 
	 */
    QRetrieveScreen( QWidget *parent = 0 );

	/** Insereix un nou estudi per descarregar
	 * @param study informació de l'estudi a descarregar
	 */
    void insertNewRetrieve( Study * );

	/// destructor de la classe
    ~QRetrieveScreen();
    
public slots :
    
	/** slot que s'invoca quant un StarviewerProcessImage emet un signal imageRetrieved
	 * @param uid de l'estudi descarregat de l'imatge
	 * @param número d'imatges descarregades
	 */
    void imageRetrieved( QString studyUID , int downloadedImages );

	/** Augmenta en un el nombre de series descarregades
	 * @param UID de l'estudi que s'ha descarregat una sèrie
	 */
    void setSeriesRetrieved( QString studyUID );

	/** S'invoca quant s'ha acabat de baixa un estudi. S'indica a la llista que la descarrega de l'estudi ha finalitzat
	 * @param  UID de l'estudi descarregat
	 */
    void setRetrievedFinished( QString studyUID );
    
	/** S'invoca quant es produeix algun error al descarregar un estudi
	 * @param studyUID UID de l'estudi descarregat
	 */
	void setErrorRetrieving( QString studyUID );
    
	/** S'invoca quan es comença a descarregar un estudi, per indicar-ho a la llista que aquell estudi ha començat la descarrega
	 * @param  UID de l'estudi que es comença a descarregar
	 */
	void setRetrieving( QString );
    
	/// Neteja la llista d'estudis excepte dels que s'estant descarregant en aquells moments 
    void clearList();

private:

	/// Crea les connexions pels signals i slots
    void createConnections();

	/** Esborra l'estudi enviat per parametre
	 * @param UID de l'estudi
 	 */
    void deleteStudy( QString studyUID );
};

};

#endif

