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

    /// Fa el ray casting pels mètodes de VMI. Rep la llista de punts de vista i els fitxers corresponents. Omple el volum vist per cada vista i retorna el volum total vist.
    float vmiRayCasting( const QVector<Vector3> &viewpoints, const QVector<QTemporaryFile*> &pOvFiles, QVector<float> &viewedVolumePerView );
    /// Normalitza les probabilitats de les vistes dividint-les per \a totalViewedVolume.
    void normalizeViewProbabilities( QVector<float> &viewProbabilities, float totalViewedVolume );
    /// Retorna les probabilitats p(O) dels objectes (vòxels) donats p(V) i p(O|V).
    QVector<float> getObjectProbabilities( const QVector<float> &viewProbabilities, const QVector<QTemporaryFile*> &pOvFiles );

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
    void loadVomi();
    void saveVomi();
    void vomiChecked( bool checked );
    void loadVoxelSaliencies();
    void saveVoxelSaliencies();
    void loadViewpointVomi();
    void saveViewpointVomi();
    void voxelSalienciesChecked( bool checked );

    void loadAndRunProgram();

private:

    Experimental3DVolume *m_volume; // el destruirà el visor

    bool m_computingObscurance;
    ObscuranceMainThread *m_obscuranceMainThread;
    Obscurance *m_obscurance;

    QVector<float> m_vmi;
    QVector<float> m_viewpointUnstabilities;
    QList< QPair<int, Vector3> > m_bestViews;
    QVector<float> m_vomi;
    float m_maximumVomi;
    QVector<float> m_voxelSaliencies;
    float m_maximumSaliency;
    QVector<float> m_viewpointVomi;

};


}


#endif
