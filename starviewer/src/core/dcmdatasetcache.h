/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDCMDATASETCACHE_H
#define UDGDCMDATASETCACHE_H

#include <QObject>
#include <QCache>
#include <QString>

class DcmDataset;

namespace udg {

/**
    Classe que fa de cache d'objectes DCMTK DcmDataSet. Serveix per millorar el rendiment de la classe
    DICOMTagReader i no s'hauria de fer servir fòra d'aquest àmbit.
    Té la funció d'autoclear. Aquesta serveix per esborrar tota la cache si no s'utilitza transcurregut un temps determinat.
    Si durant un temps determinat no es fa cap insert, remove o find, i s'ha activat l'autoclear, aquesta s'auto esborrarà.
    L'autoclear, per defecte, està desactivat.
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DcmDatasetCache : public QObject
{
Q_OBJECT
public:
    DcmDatasetCache();
    /// Constructor que permet activar l'autoclear passant els segons cada quant ha de buidar la cache.
    DcmDatasetCache(int seconds);

    ~DcmDatasetCache();

    /// Busca a la cache el DcmDataSet extret del fitxer filePath. Retorna NULL en el cas de no trobar-lo.
    /// L'element tornat passa a ser responsabilitat de qui el rep (la caché no l'alliberarà mai).
    DcmDataset* find(const QString &filePath);

    /// Afegeix/eliminia un element a la cache. Un cop afegit la cache serà la propietària de l'objecte i el podrà esborrar quan vulgui.
    /// Per tant, si es vol conservar l'element fora de la caché, s'haurà de mantenir una còpia.
    bool insert(const QString &filePath, DcmDataset *dataSet, int cost = 1);
    bool remove(const QString &filePath);

    /// Buida la cache
    void clear();

    /// Activa/desactiva l'autoclear. Es pot especificar el número de segons que han de transcòrrer sense cap activitat a la cache per
    /// tal de que s'autobuidi.
    void activateAutoclear(int seconds = 60);
    void deactivateAutoclear();

protected:
    /// Event que s'executarà al passar els segons especificats per setSecondsForAutoClear
    void timerEvent(QTimerEvent *);

private:
    /// Reinicialitza el compte enrera del timer d'autoclear o l'inicialitza si cal.
    void resetAutoclearTimer();
    /// Retorna si tenim l'autoclear timer actiu o no
    bool isAutoclearTimerActive();
    /// Activa l'autoclear timer als segons indicats. Si ja n'hi havia un altre d'actiu, el reseteja als segons indicats.
    void activateAutoclearTimer(int seconds);
    /// Si hi ha un autoclear timer actiu, el desactiva.
    void killAutoclearTimer();

private:
    QCache<QString, DcmDataset> m_cache;
    int m_autoclearTimer;
    int m_secondsForAutoclear;
    bool m_autoclearIsActive;
};

}

#endif
