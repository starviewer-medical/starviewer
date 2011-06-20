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

    /// Ens diu quin és el pla de projecció d'una imatge segons les etiquetes d'orientació (R/L,A/P,F/H)
    /// El format serà "direccióFiles\\direccióColumnes"
    /// Valors: AXIAL, SAGITAL, CORONAL, OBLIQUE o N/A
    static const QString getProjectionLabelFromPlaneOrientation(const QString &orientation);
};

} // End namespace udg

#endif
