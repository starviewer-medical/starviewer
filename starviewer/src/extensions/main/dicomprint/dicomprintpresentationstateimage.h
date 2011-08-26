#ifndef DICOMPRINTPRESENTATIONSTATEIMAGE_H
#define DICOMPRINTPRESENTATIONSTATEIMAGE_H

namespace udg {

/**
    Aquesta classe defineix un Presentation State a aplicar a imatges que s'han d'enviar a imprimir.
  */

class DICOMPrintPresentationStateImage
{
public:
    DICOMPrintPresentationStateImage();

    /// Assigna/Obté el WindowLevel que s'aplicarà a les imatges. Sinó se n'assignat cap retorna 0. Consultar mètode applyDefaultWindowLevelToImage
    void setWindowLevel(double windowCenter, double windowWidth);
    double getWindowCenter() const;
    double getWindowWidth() const;

    /// Indica si s'ha d'aplicat el WindowLevel per defecte que té la Imatge, serà cert en el cas que no s'hagi assignat cap WindowLevel en aquest objecte
    bool applyDefaultWindowLevelToImage() const;

private:

    double m_windowWidth;
    double m_windowCenter;

    bool m_applyDefaultWindowLevelToImage;
};

}

#endif // DICOMPRINTPRESENTATIONSTATE_H
