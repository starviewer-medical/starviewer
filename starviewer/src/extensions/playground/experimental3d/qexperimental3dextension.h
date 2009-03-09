#ifndef UDGQEXPERIMENTAL3DEXTENSION_H
#define UDGQEXPERIMENTAL3DEXTENSION_H


#include "ui_qexperimental3dextensionbase.h"


class QTemporaryFile;


namespace udg {


class Experimental3DVolume;
class Obscurance;
class ObscuranceMainThread;
class Volume;


/**
 * Aquesta extensió pretén ser com OptimalViewpoint però ben feta.
 */
class QExperimental3DExtension : public QWidget, private ::Ui::QExperimental3DExtensionBase {

    Q_OBJECT

public:

    QExperimental3DExtension( QWidget *parent = 0 );
    ~QExperimental3DExtension();

    /// Assigna el volum d'entrada.
    void setInput( Volume *input );

private:

    /// Crea les connexions de signals i slots.
    void createConnections();

    void loadTransferFunction( const QString &fileName );

    /// Fa un recorregut pels viewpoints en ordre i amb suavitat.
    void tour( const QList<Vector3> &viewpoints, double speedFactor = 1.0 );

    /// Crea els fitxers temporals per guardar la matriu P(O|V).
    QVector<QTemporaryFile*> createObjectProbabilitiesPerViewFiles( int nViewpoints );
    /// Destrueix els fitxer temporals per guardar la matriu P(O|V).
    void deleteObjectProbabilitiesPerViewFiles( QVector<QTemporaryFile*> &files );
    /// Fa el ray casting pels mètodes de VMI. Rep la llista de punts de vista i els fitxers corresponents. Omple el volum vist per cada vista i retorna el volum total vist.
    float vmiRayCasting( const QVector<Vector3> &viewpoints, const QVector<QTemporaryFile*> &pOvFiles, QVector<float> &viewedVolumePerView );
    /// Normalitza les probabilitats de les vistes dividint-les per \a totalViewedVolume.
    void normalizeViewProbabilities( QVector<float> &viewProbabilities, float totalViewedVolume );
    /// Retorna les probabilitats p(O) dels objectes (vòxels) donats p(V) i p(O|V).
    QVector<float> getObjectProbabilities( const QVector<float> &viewProbabilities, const QVector<QTemporaryFile*> &pOvFiles );
    // Calcula les mesures relacionades amb la VoMI: VoMI, saliency, VoMI de cada vista, VoMI amb colors.
    void computeVomiRelatedMeasures( const ViewpointGenerator &viewpointGenerator, const QVector<float> &viewProbabilities, const QVector<float> &objectProbabilities, const QVector<QTemporaryFile*> &pOvFiles,
                                     bool computeVomi, bool computeVoxelSaliencies, bool computeViewpointVomi, bool computeColorVomi );
    // Calcula les mesures relacionades amb la VMI: VMI, inestabilitats, EVMI.
    void computeVmiRelatedMeasures( const ViewpointGenerator &viewpointGenerator, const QVector<float> &viewProbabilities, const QVector<float> &objectProbabilities, const QVector<QTemporaryFile*> &pOvFiles,
                                    bool computeVmi, bool computeViewpointUnstabilities, bool computeEvmi );
    // Calcula les millors vistes.
    void computeBestViews( const QVector<Vector3> &viewpoints, const QVector<float> &viewProbabilities, const QVector<float> &objectProbabilities, const QVector<QTemporaryFile*> &pOvFiles );
    // Calcula el guided tour.
    void computeGuidedTour( const ViewpointGenerator &viewpointGenerator, const QVector<float> &viewProbabilities, const QVector<QTemporaryFile*> &pOvFiles );
    /// Escriu al log un error d'un programa.
    void logProgramError( int lineNumber, const QString &error, const QString &extra ) const;

    void loadColorVomiPalette( const QString &fileName );

    QString getFileNameToLoad( const QString &settingsDirKey, const QString &caption, const QString &filter );
    QString getFileNameToSave( const QString &settingsDirKey, const QString &caption, const QString &filter, const QString &defaultSuffix );

private slots:

    /// Obre un diàleg per triar el color de fons.
    void chooseBackgroundColor();
    /// Habilita/inhabilita la il·luminació especular.
    void enableSpecularLighting( bool on );
    /// Obre un diàleg per carregar una funció de transferència.
    void loadTransferFunction();
    /// Obre un diàleg per desar una funció de transferència.
    void saveTransferFunction();
    /// Fa la visualització amb les opcions seleccionades.
    void doVisualization();

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
    void loadVmi();
    void saveVmi();
    void loadViewpointUnstabilities();
    void saveViewpointUnstabilities();
    void loadBestViews();
    void saveBestViews();
    void loadGuidedTour();
    void saveGuidedTour();
    void loadVomi();
    void saveVomi();
    void vomiChecked( bool checked );
    void loadVoxelSaliencies();
    void saveVoxelSaliencies();
    void voxelSalienciesChecked( bool checked );
    void loadViewpointVomi();
    void saveViewpointVomi();
    void loadEvmi();
    void saveEvmi();
    void loadColorVomiPalette();
    void loadColorVomi();
    void saveColorVomi();
    void colorVomiChecked( bool checked );

    void tourBestViews();
    void guidedTour();

    void computeVomiGradient();

    void loadAndRunProgram();

private:

    Experimental3DVolume *m_volume; // el destruirà el visor

    bool m_computingObscurance;
    ObscuranceMainThread *m_obscuranceMainThread;
    Obscurance *m_obscurance;

    QVector<float> m_vmi;
    QVector<float> m_viewpointUnstabilities;
    QList< QPair<int, Vector3> > m_bestViews;
    QList< QPair<int, Vector3> > m_guidedTour;
    QVector<float> m_vomi;
    float m_maximumVomi;
    QVector<float> m_voxelSaliencies;
    float m_maximumSaliency;
    QVector<float> m_viewpointVomi;
    QVector<float> m_evmi;
    QVector<Vector3Float> m_colorVomiPalette;
    QVector<Vector3Float> m_colorVomi;
    float m_maximumColorVomi;

};


}


#endif
