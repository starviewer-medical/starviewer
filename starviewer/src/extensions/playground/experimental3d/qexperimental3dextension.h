#ifndef UDGQEXPERIMENTAL3DEXTENSION_H
#define UDGQEXPERIMENTAL3DEXTENSION_H


#include "ui_qexperimental3dextensionbase.h"


class QStringListModel;
class QTemporaryFile;


namespace udg {


class Experimental3DVolume;
class Obscurance;
class ObscuranceMainThread;
class Volume;


/**
 * Aquesta extensió serveix per fer tot tipus d'experiments relacionats amb la visualització 3D, fent un ray casting amb CPU.
 */
class QExperimental3DExtension : public QWidget, private ::Ui::QExperimental3DExtensionBase {

    Q_OBJECT

public:

    QExperimental3DExtension( QWidget *parent = 0 );
    ~QExperimental3DExtension();

    /// Assigna el volum d'entrada.
    void setInput( Volume *input );

public slots:

    /// Carrega la viewpoint entropy des d'un fitxer. Si no es dóna el nom de fitxer com a paràmetre el demana amb un diàleg.
    void loadViewpointEntropy( QString fileName = QString() );
    /// Desa la viewpoint entropy a un fitxer. Si no es dóna el nom de fitxer com a paràmetre el demana amb un diàleg.
    void saveViewpointEntropy( QString fileName = QString() );
    /// Carrega l'entropia des d'un fitxer. Si no es dóna el nom de fitxer com a paràmetre el demana amb un diàleg.
    void loadEntropy( QString fileName = QString() );
    /// Desa l'entropia a un fitxer. Si no es dóna el nom de fitxer com a paràmetre el demana amb un diàleg.
    void saveEntropy( QString fileName = QString() );
    /// Carrega la VMI des d'un fitxer. Si no es dóna el nom de fitxer com a paràmetre el demana amb un diàleg.
    void loadVmi( QString fileName = QString() );
    /// Desa la VMI a un fitxer. Si no es dóna el nom de fitxer com a paràmetre el demana amb un diàleg.
    void saveVmi( QString fileName = QString() );
    /// Carrega la informació mútua des d'un fitxer. Si no es dóna el nom de fitxer com a paràmetre el demana amb un diàleg.
    void loadMi( QString fileName = QString() );
    /// Desa la informació mútua a un fitxer. Si no es dóna el nom de fitxer com a paràmetre el demana amb un diàleg.
    void saveMi( QString fileName = QString() );
    /// Carrega la VoMI des d'un fitxer. Si no es dóna el nom de fitxer com a paràmetre el demana amb un diàleg.
    void loadVomi( QString fileName = QString() );
    /// Desa la VoMI a un fitxer. Si no es dóna el nom de fitxer com a paràmetre el demana amb un diàleg.
    void saveVomi( QString fileName = QString() );
    /// Carrega la viewpoint VoMI des d'un fitxer. Si no es dóna el nom de fitxer com a paràmetre el demana amb un diàleg.
    void loadViewpointVomi( QString fileName = QString() );
    /// Desa la viewpoint VoMI a un fitxer. Si no es dóna el nom de fitxer com a paràmetre el demana amb un diàleg.
    void saveViewpointVomi( QString fileName = QString() );
    /// Carrega la paleta per la color VoMI des d'un fitxer. Si no es dóna el nom de fitxer com a paràmetre el demana amb un diàleg.
    void loadColorVomiPalette( QString fileName = QString() );
    /// Carrega la color VoMI des d'un fitxer. Si no es dóna el nom de fitxer com a paràmetre el demana amb un diàleg.
    void loadColorVomi( QString fileName = QString() );
    /// Desa la color VoMI a un fitxer. Si no es dóna el nom de fitxer com a paràmetre el demana amb un diàleg.
    void saveColorVomi( QString fileName = QString() );
    /// Carrega les millors vistes des d'un fitxer. Si no es dóna el nom de fitxer com a paràmetre el demana amb un diàleg.
    void loadBestViews( QString fileName = QString() );
    /// Desa les millors vistes a un fitxer. Si no es dóna el nom de fitxer com a paràmetre el demana amb un diàleg.
    void saveBestViews( QString fileName = QString() );

private:

    /// Carrega dades de tipus float del fitxer al float. Retorna cert si tot va bé i fals si hi ha error.
    static bool loadFloatData( const QString &fileName, float &data );
    /// Carrega dades de tipus T del fitxer a la llista. Retorna cert si tot va bé i fals si hi ha error.
    template <class T> static bool loadData( const QString &fileName, QList<T> &list );
    /// Carrega dades de tipus T del fitxer al vector. Retorna cert si tot va bé i fals si hi ha error.
    template <class T> static bool loadData( const QString &fileName, QVector<T> &vector );
    /// Desa dades de tipus float del float al fitxer. Retorna cert si tot va bé i fals si hi ha error.
    static bool saveFloatData( float data, const QString &fileName );
    /// Desa dades de tipus T de la llista al fitxer. Retorna cert si tot va bé i fals si hi ha error.
    template <class T> static bool saveData( const QList<T> &list, const QString &fileName );
    /// Desa dades de tipus T del vector al fitxer. Retorna cert si tot va bé i fals si hi ha error.
    template <class T> static bool saveData( const QVector<T> &vector, const QString &fileName );
    /// Desa dades de tipus float del float al fitxer en forma de text amb un format. Retorna cert si tot va bé i fals si hi ha error.
    static bool saveFloatDataAsText( float data, const QString &fileName, const QString &format );
    /// Desa dades de tipus float del vector al fitxer en forma de text amb un format. Retorna cert si tot va bé i fals si hi ha error.
    static bool saveFloatDataAsText( const QVector<float> &vector, const QString &fileName, const QString &format, int base = 0 );
    /// Desa dades de tipus QPair<int, Vector3> de la llista al fitxer en forma de text amb un format. Retorna cert si tot va bé i fals si hi ha error.
    static bool saveDataAsText( const QList< QPair<int, Vector3> > &list, const QString &fileName, const QString &format, int base = 0 );

    /// Crea les connexions de signals i slots.
    void createConnections();

    /// Llança un diàleg per obtenir un nom de fitxer per llegir.
    QString getFileNameToLoad( const QString &settingsDirKey, const QString &caption, const QString &filter );
    /// Llança un diàleg per obtenir un nom de fitxer per escriure.
    QString getFileNameToSave( const QString &settingsDirKey, const QString &caption, const QString &filter, const QString &defaultSuffix );

    void loadTransferFunction( const QString &fileName );
    void saveTransferFunction( const QString &fileName );
    void loadCamera( const QString &fileName );

    /// Fa un recorregut pels viewpoints en ordre i amb suavitat.
    void tour( const QList<Vector3> &viewpoints, double speedFactor = 1.0 );

    /// Fa el ray casting pels mètodes de VMI. Rep la llista de punts de vista i els fitxers corresponents. Omple el volum vist per cada vista i retorna el volum total vist.
    float vmiRayCasting( const QVector<Vector3> &viewpoints, const QVector<QTemporaryFile*> &pOvFiles, QVector<float> &viewedVolumePerView );
    /// Normalitza les probabilitats de les vistes dividint-les per \a totalViewedVolume.
    void normalizeViewProbabilities( QVector<float> &viewProbabilities, float totalViewedVolume );
    /// Retorna les probabilitats p(O) dels objectes (vòxels) donats p(V) i p(O|V).
    QVector<float> getObjectProbabilities( const QVector<float> &viewProbabilities, const QVector<QTemporaryFile*> &pOvFiles );
    // Calcula les mesures relacionades amb la VMI: VMI, inestabilitats, EVMI.
    void computeVmiRelatedMeasures( const ViewpointGenerator &viewpointGenerator, const QVector<float> &viewProbabilities, const QVector<float> &objectProbabilities, const QVector<QTemporaryFile*> &pOvFiles,
                                    bool computeViewpointUnstabilities, bool computeEvmi );
    // Calcula el guided tour.
    void computeGuidedTour( const ViewpointGenerator &viewpointGenerator, const QVector<float> &viewProbabilities, const QVector<QTemporaryFile*> &pOvFiles );
    /// Escriu al log un error d'un programa.
    void logProgramError( int lineNumber, const QString &error, const QString &line ) const;
    /// Comprova que la línia tingi un nombre determinat de paraules.
    bool programCheckWordCount( int lineNumber, const QString &line, int wordCount ) const;
    bool programTab( int lineNumber, const QString &line, bool run );
    bool programRenderingInterpolation( int lineNumber, const QString &line, bool run );
    bool programRenderingGradientEstimator( int lineNumber, const QString &line, bool run );
    bool programRenderingBaseShading( int lineNumber, const QString &line, bool run );
    bool programRenderingCheckOrUncheck( int lineNumber, const QString &line, bool run );
    bool programVmiViewpoints( int lineNumber, const QString &line, bool run );
    bool programVmiCheckOrUncheck( int lineNumber, const QString &line, bool run );
    bool programVmiLoadOrSave( int lineNumber, const QString &line, bool run );

    void loadViewpointUnstabilities( const QString &fileName );
    void saveViewpointUnstabilities( const QString &fileName );
    void loadGuidedTour( const QString &fileName );
    void saveGuidedTour( const QString &fileName );
    void loadEvmi( const QString &fileName );
    void saveEvmi( const QString &fileName );

private slots:

    void setNewVolume( Volume *volume );

    /// Obre un diàleg per triar el color de fons.
    void chooseBackgroundColor();
    /// Habilita/inhabilita la il·luminació especular.
    void enableSpecularLighting( bool on );
    /// Obre un diàleg per carregar una funció de transferència.
    void loadTransferFunction();
    /// Obre un diàleg per desar una funció de transferència.
    void saveTransferFunction();
    void addRecentTransferFunction();
    void setRecentTransferFunction( const QModelIndex &index );
    void setTransferFunction( bool render = true );
    /// Renderitza amb les opcions seleccionades.
    void render();

    /// Posa els paràmetres de la càmera actuals als spinboxs corresponents.
    void getCamera();
    /// Assigna els paràmetres de la càmera d'acord amb els spinboxs corresponents.
    void setCamera();
    /// Carrega els paràmetres de la càmera des d'un fitxer.
    void loadCamera();
    /// Desa els paràmetres de la càmera a un fitxer.
    void saveCamera();
    /// Estableix el nombre de punts de vista.
    void setNumberOfViewpoints( int numberOfViewpoints );
    /// Situa la càmera al punt de vista seleccionat.
    void setViewpoint();
    /// Situa la càmera al punt de vista passat.
    void setViewpoint( const Vector3 &viewpoint );
    /// Fa un recorregut pels punts de vista triats per l'usuari.
    void tour();

    /// Comença el càlcul de les obscurances si no s'estan calculant i el cancel·la si s'estan calculant.
    void computeCancelObscurance();
    /// Fa el que cal quan s'acaba el càlcul de les obscurances correctament.
    void endComputeObscurance();
    /// Fa el que cal quan s'acaba el càlcul de les obscurances cancel·lat.
    void endCancelObscurance();
    /// Carrega les obscurances des d'un fitxer.
    void loadObscurance();
    /// Desa les obscurances a un fitxer.
    void saveObscurance();

    /// Estableix el nombre de punts de vista de l'SMI.
    void setNumberOfSmiViewpoints( int numberOfViewpoints );
    /// Calcula les SMI amb el/s punt/s de vista seleccionat/s.
    void computeSmi();
    /// Calcula les inestabilitats de les llesques.
    void computeSliceUnstabilities();
    /// Calcula les PMI amb el/s punt/s de vista seleccionat/s.
    void computePmi();
    /// Calcula les saliencies dels valors de propietat.
    void computePropertySaliencies();

    /// Calcula les mesures del grup VMI seleccionades.
    void computeSelectedVmi();
    void computeSelectedVmiOld();   // el deixem temporalment mentre acabem de passar el codi que queda a ViewpointInformationChannel
    void loadViewpointUnstabilities();
    void saveViewpointUnstabilities();
    void loadGuidedTour();
    void saveGuidedTour();
    void loadEvmi();
    void saveEvmi();

    void tourBestViews();
    void guidedTour();

    void computeVomiGradient();

    void loadAndRunProgram();

    void opacityVomiChecked( bool checked );
    void opacitySaliencyChecked( bool checked );

    void setVmiOneViewpointMaximum( int maximum );

private:

    Experimental3DVolume *m_volume;

    QList<TransferFunction> m_recentTransferFunctions;
    QStringListModel *m_recentTransferFunctionsModel;

    bool m_computingObscurance;
    ObscuranceMainThread *m_obscuranceMainThread;
    Obscurance *m_obscurance;

    QVector<float> m_viewpointEntropy;
    float m_entropy;
    QVector<float> m_vmi;
    float m_mi;
    QVector<float> m_vomi;
    float m_maximumVomi;
    QVector<float> m_viewpointVomi;
    QVector<Vector3Float> m_colorVomiPalette;
    QVector<Vector3Float> m_colorVomi;
    float m_maximumColorVomi;
    QList< QPair<int, Vector3> > m_bestViews;
    QVector<float> m_viewpointUnstabilities;
    QList< QPair<int, Vector3> > m_guidedTour;
    QVector<float> m_voxelSaliencies;
    float m_maximumSaliency;
    QVector<float> m_evmi;

    /// Cert quan estiguem executant el l'extensió interactivament.
    bool m_interactive;

};


} // namespace udg


#endif // UDGQEXPERIMENTAL3DEXTENSION_H
