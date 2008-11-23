/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGHANGINGPROTOCOLIMAGESET_H
#define UDGHANGINGPROTOCOLIMAGESET_H

#include <QObject>

namespace udg {

class Series;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class HangingProtocolImageSet : public QObject
{
Q_OBJECT
public:

    HangingProtocolImageSet(QObject *parent = 0);

    ~HangingProtocolImageSet();

    enum SelectorUsageFlag { MATCH, NO_MATCH };

    enum SelectorCategory { RELATIVE_TIME, ABSTRACT_PRIOR };

    struct Restriction
    {
        SelectorUsageFlag usageFlag; // MATCH o NO_MATCH
        QString selectorAttribute; // TAG
        QString valueRepresentation; // valor del TAG
        int selectorValueNumber; // Només si el TAG és multivalor
    };

    /// Identificador de l'Image Set
    void setIdentifier( int identifier );

    /// Obtenir l'identificador de l'Image Set
    int getIdentifier();

    /// Afegir una restriccio
    void addRestriction( Restriction restriction );

    /// Obtenir les restriccions
    QList<Restriction> getRestrictions();

	/// Posar el tipus d'element
	void setTypeOfItem( QString );
		
	/// Obtenir el tipus d'element
	QString getTypeOfItem();

	/// Posar el número de la imatge a mostrar
	void setImageToDisplay( int imageNumber );

	/// Obtenir el numero de la imatge a mostrar
	int getImatgeToDisplay();

	/// Assignar la serie que es representa
	void setSeriesToDisplay( Series * series );
	
	/// Obtenir la serie que es representa
	Series * getSeriesToDisplay();

    /// Mètode per mostrar els valors
    void show();

private:

    /// Llista de restriccions que ha de complir l'Image Set
    QList<Restriction> m_listOfRestrictions;

    /// Identificador únic de l'Image Set
    int m_identifier;

    /// Categoria
    SelectorCategory m_category;

    /// Valor inferior rang
    int m_relativeTimeMinimum;

    /// Valor superior rang
    int m_relativeTimeMaximum;

    /// Unitats rang
    QString m_timeUnits;

    /// Descripcio
    QString m_description;

	/// Per saber si s'ha de tractar a nivell d'imatge o de sèrie. 
	QString m_typeOfItem;

	/// Serie que s'ajusta a les restriccions
	Series * m_serieToDisplay;

	/// Número d'imatge a mostrar
	int m_imageToDisplay;

};

}

#endif
