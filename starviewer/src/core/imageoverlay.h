#ifndef UDGIMAGEOVERLAY_H
#define UDGIMAGEOVERLAY_H

#include <QString>
#include <QSharedPointer>

class QRect;

namespace gdcm {
class Overlay;
}

namespace udg {

class DrawerBitmap;

/**
    Classe que encapsula l'objecte Overlay del DICOM.
    Per més informació, consultar apartat C.9 OVERLAYS, PS 3.3.
 */
class ImageOverlay {
public:
    ImageOverlay();
    ~ImageOverlay();

    /// Assigna/obté les files i columnes de l'overlay
    void setRows(int rows);
    void setColumns(int columns);
    int getRows() const;
    int getColumns() const;

    /// Assigna/obté l'origen de l'Overlay
    void setOrigin(int x, int y);
    int getXOrigin() const;
    int getYOrigin() const;

    /// Assigna/retorna les dades de l'overlay
    void setData(unsigned char *data);
    unsigned char* getData() const;

    /// Retorna cert sii l'overlay és vàlid (si el nombre de files i el nombre de columnes són positius i té dades).
    bool isValid() const;

    /// Crea un overlay més petit dins de la regió donada.
    /// Si l'overlay no és vàlid o la regió no està completament continguda dins de l'overlay, retorna un overlay invàlid.
    ImageOverlay createSubOverlay(const QRect &region) const;

    /// Separa l'overlay en overlays més petits que tinguin menys espai desaprofitat per estalviar memòria.
    /// Si l'overlay no és vàlid o és buit retorna una llista buida.
    QList<ImageOverlay> split() const;

    /// Compara aquest overlay amb un altre i diu si són iguals.
    bool operator ==(const ImageOverlay &overlay) const;

    /// Construeix un ImageOverlay a partir d'un gdcm::Overlay
    static ImageOverlay fromGDCMOverlay(const gdcm::Overlay &gdcmOverlay);

    /// Fusiona una llista d'overlays en un únic overlay
    /// Només fusionarà aquells overlays que reuneixin les condicions necessàries per considerar-se vàlids, és a dir, 
    /// que tingui un nombre de files i columnes > 0 i que tingui dades. El paràmetre ok, servirà per indicar els casos 
    /// en que no s'ha pogut realitzar fusió per algun error. Si el llistat d'Overlays vàlids és buit, retornarà un
    /// overlay buit, si només hi ha un de sol, retornarà aquest mateix overlay, la fusió dels n overlays altrament. Per aquests casos 
    /// el valor d'ok serà true. En cas que no hi hagi prou memòria per allotjar un nou buffer per l'overlay fusionat, retornarà un overlay buit i ok false.
    static ImageOverlay mergeOverlays(const QList<ImageOverlay> &overlaysList, bool &ok);

    /// Ens retorna l'overlay en format DrawerBitmap per sobreposar sobre una imatge amb l'origin i spacing donats
    DrawerBitmap* getAsDrawerBitmap(double origin[3], double spacing[3]) const;

private:
    /// Ús intern per QSharedPointer. Aquest serà el mètode que es cridarà per eliminar 
    /// TODO Seria convenient tenir definit un deleter genèric definit en una altra classe amb un template
    /// template <typename T> void arrayDeleter(T array[])
    /// en cas que tinguem més shared pointers amb punters a arrays
    static void deleteDataArray(unsigned char dataArray[]);

    /// Retorna una còpia de les dades de l'overlay a la regió donada.
    unsigned char* copyDataForSubOverlay(const QRect &region) const;

private:
    /// Files i columnes de l'overlay
    int m_rows, m_columns;

    /// Localització del primer punt de l'overlay respecte als pixels a la imatge, donat com fila\columna.
    /// El pixel superior esquerre té la coordenada 1\1
    /// Valors de columna majors que 1 indiquen que l'origen del pla d'overlay és a la dreta de l'origen de la imatge. 
    /// Valors de fila majors que 1 indiquen que l'origen del pla d'overlay està per sota de l'origen de la imatge.
    /// Valors per sota de 1 indiquen que l'origen del pla d'overlay està per sobre o a l'esquerra de l'origen de la imatge.
    int m_origin[2];

    /// Dades de l'overlay
    QSharedPointer<unsigned char> m_data;
};

}

#endif