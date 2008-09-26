/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGHANGINGPROTOCOLDISPLAYSET_H
#define UDGHANGINGPROTOCOLDISPLAYSET_H

#include <QObject>

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class HangingProtocolDisplaySet : public QObject
{
Q_OBJECT
public:
    HangingProtocolDisplaySet(QObject *parent = 0);

    ~HangingProtocolDisplaySet();

    /// Posar l'identificador
    void setIdentifier( int identifier );

    /// Posar la descripcio
    void setDescription( QString description );

    /// Posar l'image set a que referencia
    void setImageSetNumber( int number );

    /// Posar la posició del visualitzador
    void setPosition( QString position );

	/// Posar la posició del pacient
	void setPatientOrientation( QString string );

	/// Obtenir l'identificador
    int getIdentifier();

    /// Obtenir la descripcio
    QString getDescription();

    /// Obtenir l'image set a que referencia
    int getImageSetNumber();

    /// Obtenir la posició del visualitzador
    QString getPosition();

	/// Obtenir la posició del pacient
	QString getPatientOrientation();

    /// Mètode per mostrar els valors
    void show();

private:

    /// Identificador de la seqüència
    int m_identifier;

    /// Descripció
    QString m_description;

    /// Image set que referencia
    int m_imageSetNumber;

    /// Posició a on es troba
    QString m_position;

	/// Orientacio del pacient
	QString m_patientOrientation;
};

}

#endif
