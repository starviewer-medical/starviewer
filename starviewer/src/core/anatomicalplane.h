#ifndef UDGANATOMICALPLANE_H
#define UDGANATOMICALPLANE_H

class QString;

namespace udg {

/**
    Classe que encapsularà les orientacions anatòmiques del pacient
  */
class AnatomicalPlane {
public:
    /// Tipus d'orientacions anatòmiques
    enum AnatomicalPlaneType { Axial, Sagittal, Coronal, Oblique, NotAvailable };

    /// Donada una orientació anatòmica, ens retorna la corresponent etiqueta per mostrar a l'interfície
    static const QString getLabel(AnatomicalPlaneType orientation);
};

} // End namespace udg

#endif
