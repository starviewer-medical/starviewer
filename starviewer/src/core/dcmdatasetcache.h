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
#include <QReadWriteLock>

class DcmDataset;

namespace udg {

/**
    Classe que fa de cache d'objectes DCMTK DcmDataSet. Serveix per millorar el rendiment de la classe
    DICOMTagReader i no s'hauria de fer servir fòra d'aquest àmbit.
    Té la funció d'autoclear. Aquesta serveix per esborrar tota la cache si no s'utilitza transcurregut un temps determinat.
    Si durant un temps determinat no es fa cap insert, remove o find, i s'ha activat l'autoclear, aquesta s'auto esborrarà.
    L'autoclear, per defecte, està desactivat.
    Aquesta classe s'ha fet thread-safe a l'hora d'accedir a la cache propiament dita. En canvi, accedir a les funcions d'autoclear
    no és thread-safe. Per tant, abans de fer servir la classe a diferents threads (si cal) s'ha de configurar l'autoclear fora d'aquests.
    També permet definir el tamany de la cache a partir d'un paràmetre de configuració: DICOMDatasetCacheMaxSize; del lloc on es
    guardi la configuració amb QSettings.
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DcmDatasetCache : public QObject
{
Q_OBJECT
public:
    /// Els constructors no són thread-safe.
    DcmDatasetCache();
    /// Constructor que permet activar l'autoclear passant els segons cada quant ha de buidar la cache.
    DcmDatasetCache(int seconds);

    ~DcmDatasetCache();

    /// Busca a la cache el DcmDataSet extret del fitxer filePath. Retorna NULL en el cas de no trobar-lo.
    /// L'element tornat passa a ser responsabilitat de qui el rep (la caché no l'alliberarà mai). Mètode thread-safe.
    DcmDataset* find(const QString &filePath);

    /// Afegeix/eliminia un element a la cache. Un cop afegit la cache serà la propietària de l'objecte i el podrà esborrar quan vulgui.
    /// Per tant, si es vol conservar l'element fora de la caché, s'haurà de mantenir una còpia. Mètodes thread-safe
    bool insert(const QString &filePath, DcmDataset *dataSet, int cost = 1);
    bool remove(const QString &filePath);

    /// Buida la cache. Mètode thread-safe.
    void clear();

    /// Activa/desactiva l'autoclear. Es pot especificar el número de segons que han de transcòrrer sense cap activitat a la cache per
    /// tal de que s'autobuidi. Aquests mètodes no són thread-safe.
    void startAutoclear(int seconds = 60);
    void stopAutoclear();
signals:
    /// S'implementa el resetejar el timer amb un signal i slot ja que un timer no es pot resetejar des d'un thread que no sigui
    /// el que l'ha creat, impossibilitant el multithreading.
    void resetTimer();

protected:
    /// Event que s'executarà al passar els segons especificats per setSecondsForAutoClear
    void timerEvent(QTimerEvent *);

private slots:
    /// Reinicialitza el compte enrera del timer d'autoclear o l'inicialitza si cal.
    void resetAutoclearTimer();

private:
    /// Inicialitza la cache
    void InitializeCache();
    /// Retorna si tenim l'autoclear timer actiu o no
    bool isAutoclearTimerActive();
    /// Activa l'autoclear timer als segons indicats. Si ja n'hi havia un altre d'actiu, el reseteja als segons indicats.
    void startAutoclearTimer(int seconds);
    /// Si hi ha un autoclear timer actiu, el desactiva.
    void stopAutoclearTimer();

private:
    QCache<QString, DcmDataset> *m_cache;
    int m_autoclearTimer;
    int m_secondsForAutoclear;
    bool m_autoclearHasToBeActive;
    QReadWriteLock m_cacheLock;
    QReadWriteLock m_timerLock;
};

}

#endif
