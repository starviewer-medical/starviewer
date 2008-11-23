/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGHANGINGPROTOCOLLAYOUT_H
#define UDGHANGINGPROTOCOLLAYOUT_H

#include <QObject>

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class HangingProtocolLayout : public QObject
{
Q_OBJECT
public:
    HangingProtocolLayout( QObject *parent = 0 );

    ~HangingProtocolLayout();

    /// Posar el nombre de pantalles
    void setNumberOfScreens( int numberOfScreens );
    /// Posar la llista de píxels en vertical
    void setVerticalPixelsList( QList<int> verticalPixelsList );
    /// Posar la llista de píxels en horitzontal
    void setHorizontalPixelsList( QList<int> horizontalPixelsList );
    /// Posar la llista de posicions dels visualitzadors
    void setDisplayEnvironmentSpatialPositionList( QList<QString> displayEnvironmentSpatialPosition );
    /// Obtenir el nombre de pantalles
    int getNumberOfScreens();
    /// Obtenir la llista de píxels en vertical
    QList<int> getVerticalPixelsList();
    /// Obtenir la llista de píxels en horitzontal
    QList<int> getHorizontalPixelsList();
    /// Obtenir la llista de posicions dels visualitzadors
    QList<QString> getDisplayEnvironmentSpatialPositionList();

private:

    ///Hanging Protocol Environment Module Attributes
    /// Nombre de pantalles
    int m_numberOfScreens;
    /// Llista de píxels en vertical
    QList<int> m_verticalPixelsList;
    /// Llista de píxels en horitzontal
    QList<int> m_horizontalPixelsList;
    /// Llista de posicions dels visualitzadors
    QList<QString> m_displayEnvironmentSpatialPosition;

     ///Hanging Protocol Display Module


};

}

#endif
