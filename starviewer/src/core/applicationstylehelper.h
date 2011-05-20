#ifndef UDGAPPLICATIONSTYLEHELPER_H
#define UDGAPPLICATIONSTYLEHELPER_H

class QWidget;
class QString;
class QMovie;

namespace udg {

/**
    Classe que s'encarrega d'indicar i calcular diferents estils i configuracions de la interficie de l'aplicació, com per exemple
    tamanys de lletra, colors, etc.

    Per poder cridar els seu mètodes que ens retornin els diferents valors cal cridar, prèviament, a recomputeStyleToScreenOfWidget(widget)
    passant-li un widget que es trobi en la pantalla on volem que l'style s'apliqui.

    D'aquesta manera els diferents tamanys i estils s'adaptaran a la resolució i característiques d'aquella pantalla. Si no es fa, per defecte,
    retornarà valors vàlids per una pantalla "normal" (on normal = de resolució no més gran d'1 Mpx)
  */

class ApplicationStyleHelper {
public:
    ApplicationStyleHelper();

    /// Mètode que recalcula l'estil per aplicar en la pantalla on està situat el widget que se li passa.
    /// Útil per quan, per exemple, el tamany de lletra ha de ser diferent per tenir en compte pantalles amb resolucions molt altes
    void recomputeStyleToScreenOfWidget(QWidget *widget);

    /// Ens retorna el tamany de font que han de tenir les tools
    int getToolsFontSize() const;

    /// Escala el tamany d'una QMovie al adient a la pantalla on s'ha cridat recomputeStyleToScreenOfWidget
    void setScaledSizeTo(QMovie *movie) const;

    /// Escala el tamany de lletra d'un QWidget al adient a la pantalla on s'ha cridat recomputeStyleToScreenOfWidget
    void setScaledFontSizeTo(QWidget *widget) const;

private:
    /// A partir d'un tamany per defecte i una clau de setting que indica si el tamany està canviat per config.
    /// retorna el tamany de lletra adaptat a la pantalla
    int getScaledFontSize(double defaultFontSize, const QString &settingsBackdoorKey) const;

private:
    static double m_scaleFactor;
};

} // End namespace udg

#endif // UDGAPPLICATIONSTYLEHELPER_H
