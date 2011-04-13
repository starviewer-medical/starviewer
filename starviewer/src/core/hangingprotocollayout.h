#ifndef UDGHANGINGPROTOCOLLAYOUT_H
#define UDGHANGINGPROTOCOLLAYOUT_H

#include <QObject>

namespace udg {

/**
    @author Grup de Gràfics de Girona  (GGG ) <vismed@ima.udg.es>
*/
class HangingProtocolLayout : public QObject
{
Q_OBJECT
public:
    HangingProtocolLayout(QObject *parent = 0);

    ~HangingProtocolLayout();

    /// Posar el nombre de pantalles
    void setNumberOfScreens(int numberOfScreens);
    /// Posar la llista de píxels en vertical
    void setVerticalPixelsList(const QList<int> &verticalPixelsList);
    /// Posar la llista de píxels en horitzontal
    void setHorizontalPixelsList(const QList<int> &horizontalPixelsList);
    /// Posar la llista de posicions dels visualitzadors
    void setDisplayEnvironmentSpatialPositionList(const QList<QString> &displayEnvironmentSpatialPosition);
    /// Obtenir el nombre de pantalles
    int getNumberOfScreens() const;
    /// Obtenir la llista de píxels en vertical
    QList<int> getVerticalPixelsList() const;
    /// Obtenir la llista de píxels en horitzontal
    QList<int> getHorizontalPixelsList() const;
    /// Obtenir la llista de posicions dels visualitzadors
    QList<QString> getDisplayEnvironmentSpatialPositionList() const;

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
};

}

#endif
