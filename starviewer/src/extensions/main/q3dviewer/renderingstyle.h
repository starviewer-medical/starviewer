#ifndef UDGRENDERINGSTYLE_H
#define UDGRENDERINGSTYLE_H

#include "transferfunction.h"

namespace udg {

/**
    Classe que encapsula tots els paràmetres de rendering.
  */
class RenderingStyle {

public:

    /// Mètode de rendering.
    enum Method { RayCasting, Texture3D, Texture2D, MIP, IsoSurface, Contouring };
    /// Qualitat de les obscurances.
    enum ObscuranceQuality { Low, Medium, High };

    /// Assignar/obtenir el mètode de rendering.
    Method getMethod() const;
    void setMethod(Method method);
    /// Assignar/obtenir si s'aplica il·luminació o no.
    bool getShading() const;
    void setShading(bool shading);
    /// Assignar/obtenir els paràmetres d'il·luminació.
    double getAmbientCoefficient() const;
    void setAmbientCoefficient(double ambientCoefficient);
    double getDiffuseCoefficient() const;
    void setDiffuseCoefficient(double diffuseCoefficient);
    double getSpecularCoefficient() const;
    void setSpecularCoefficient(double specularCoefficient);
    double getSpecularPower() const;
    void setSpecularPower(double specularPower);
    /// Assignar/obtenir la funció de transferència.
    const TransferFunction& getTransferFunction() const;
    void setTransferFunction(const TransferFunction &transferFunction);
    /// Assignar/obtenir si s'apliquen contorns o no.
    bool getContour() const;
    void setContour(bool contour);
    /// Assignar/obtenir el llindar dels contorns.
    double getContourThreshold() const;
    void setContourThreshold(double contourThreshold);
    /// Assignar/obtenir si s'apliquen obscurances o no.
    bool getObscurance() const;
    void setObscurance(bool obscurance);
    /// Assignar/obtenir la qualitat de les obscurances.
    ObscuranceQuality getObscuranceQuality() const;
    void setObscuranceQuality(ObscuranceQuality obscuranceQuality);
    /// Assignar/obtenir el factor multiplicatiu de les obscurances.
    double getObscuranceFactor() const;
    void setObscuranceFactor(double obscuranceFactor);
    /// Assignar/obtenir l'iso-valor per les iso-superfícies.
    double getIsoValue() const;
    void setIsoValue(double isoValue);

    /// Mètodes per convertir el RenderingStyle en QVariant i viceversa.
    QVariant toVariant() const;
    static RenderingStyle fromVariant(const QVariant &variant);

private:

    /// Mètode de rendering.
    Method m_method;
    /// Il·luminació o no.
    bool m_shading;
    /// Paràmetres d'il·luminació.
    double m_ambientCoefficient;
    double m_diffuseCoefficient;
    double m_specularCoefficient;
    double m_specularPower;
    /// Funció de transferència.
    TransferFunction m_transferFunction;
    /// Contorns o no.
    bool m_contour;
    /// Llindar per aplicar contorns.
    double m_contourThreshold;
    /// Obscurances o no.
    bool m_obscurance;
    /// Qualitat de les obscurances.
    ObscuranceQuality m_obscuranceQuality;
    /// Factor multiplicatiu de les obscurances.
    double m_obscuranceFactor;
    /// Iso-valor per les iso-superfícies.
    double m_isoValue;

};

}

#endif
