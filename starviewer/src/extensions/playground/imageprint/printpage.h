#ifndef UDGPRINTPAGE_H
#define UDGPRINTPAGE_H

#include <QString>
#include <QList>

/**
* Implementació per la Interfície PrintPage, que defineix una pàgina d'impressió amb imatges DICOM.
*/

namespace udg
{

class Image;

class PrintPage 
{
public:
    
    ///Especifica/retorna el número de pàgina
    void setPageNumber(int pageNumber);
    int getPageNumber();

    ///Especifica/retorna les imatges que s'han d'imprimir en aquesta pàgina. Les imatges s'imprimiren segons l'ordre d'inserció a la llista
    void setImagesToPrint(QList<Image*> imagesToPrint);
    QList<Image*> getImagesToPrint();

private:

    int m_pageNumber;
    QList<Image*> m_imagesToPrint;
    
};
};
#endif