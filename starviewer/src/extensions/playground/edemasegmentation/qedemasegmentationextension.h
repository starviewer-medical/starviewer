/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQEDEMASEGMENTATION_H
#define UDGQEDEMASEGMENTATION_H

#include "ui_qedemasegmentationextensionbase.h"

// FWD declarations
class QAction;
class vtkImageMask;
class vtkImageThreshold;
class vtkActor;

namespace udg {

// FWD declarations
class Volume;
class StrokeSegmentationMethod;
class ToolManager;

/**
Extensió que segmenta l'hematoma i l'edema

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QEdemaSegmentationExtension : public QWidget , private ::Ui::QEdemaSegmentationExtensionBase {
Q_OBJECT
public:
    QEdemaSegmentationExtension( QWidget *parent = 0 );

    ~QEdemaSegmentationExtension();

public slots:
    /// Li assigna el volum principal
    /// És slot pq també ho fem servir en cas de canvi de sèrie
    void setInput( Volume *input );

    /// Executa l'algorisme de segmetnació
    void applyMethod();
    void applyCleanSkullMethod();
    void applyEdemaMethod();
    void applyVentriclesMethod();
    void applyFilterMainImage();
private:
    /// crea les accions \TODO 'pujar' al pare com a mètode virtual com a Extensions? [hauria de ser protected]
    void createActions();

    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Guarda i llegeix les característiques
    void readSettings();
    void writeSettings();

    void initializeTools();

private slots:
    /// visualitza la informació de la llavor del mètode de segmentació
    void setSeedPosition(double x, double y, double z);

    /// actualitza el valor llindar baix
    void setLowerValue( int x );

     /// actualitza el valor llindar alt
    void setUpperValue( int x );

    /// Canvia la opacitat de la màscara
    void setOpacity(int op);

    /// Calcula el volum de la màscara
    double calculateMaskVolume();

    /// Refresca el resultat del volum
    void updateVolume();

    /// Visualitza la màscara donats uns thresholds
    void viewThresholds();

    /// Visualitza els diferents overlays
    void viewLesionOverlay();
    void viewEdemaOverlay();
    void viewVentriclesOverlay();

    /// Desa la màscara que s'està visualitzant
    void saveActivedMaskVolume();

private:
    /// El volum principal
    Volume *m_mainVolume;

    /// El volum on hi guardem el resultat de la segmentació
    Volume *m_lesionMaskVolume;
    Volume *m_edemaMaskVolume;
    Volume *m_ventriclesMaskVolume;
    Volume *m_activedMaskVolume;
    vtkImageThreshold *m_imageThreshold;

    /// El volum on hi guardem la imatge principal filtrada
    Volume *m_filteredVolume;

    /// Mètode de la segmentació
    StrokeSegmentationMethod *m_segMethod;

    /// Membres de classe
    bool m_isSeed;
    bool m_isMask;
    
    int m_cont;
    int m_edemaCont;
    int m_ventriclesCont;
    int* m_activedCont;
    double m_volume;
    double m_edemaVolume;
    double m_ventriclesVolume;
    double* m_activedVolume;

    int m_minValue, m_maxValue;
    int m_insideValue, m_outsideValue;
    int m_lowerVentriclesValue, m_upperVentriclesValue;

    QAction *m_lesionViewAction;
    QAction *m_edemaViewAction;
    QAction *m_ventriclesViewAction;

    ///Directori on guardem les màscares
    QString m_savingMaskDirectory;

    /// Grup de botons en format exclusiu
    QActionGroup *m_viewOverlayActionGroup;

    /// ToolManager
    ToolManager *m_toolManager;

    /// La posició de la llavor
    double m_seedPosition[3];
};

} // end namespace udg

#endif
