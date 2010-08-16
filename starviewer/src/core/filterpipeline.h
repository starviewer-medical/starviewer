/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGFILTERPIPELINE_H
#define UDGFILTERPIPELINE_H


#include <QStack>


namespace udg {
class Filter;

/*
	Classe base per a tots els filtres

*/


class FilterPipeline 
{

public:
	FilterPipeline();
	~FilterPipeline();

	/* Mètode per afegir un filtre*/
	void push(Filter * newFilter);

	/* Metode per treure un filtre de la pila */
	Filter * pop();
	
	/* Metode que retorna l'últim filtre afegit */
	Filter * top();

	void update();

	int getNumberOfFilters();


protected:

	/* Contenidor de filtres */
	QStack<Filter*> m_filterStack;

	/* Nom del filtre */
	int m_numFilters;

};

}

#endif