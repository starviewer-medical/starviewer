/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGQVOLUME3DVIEWFUSEDTESTINGEXTENSION_H
#define UDGQVOLUME3DVIEWFUSEDTESTINGEXTENSION_H

#include "ui_qvolume3dviewfusedtestingextensionbase.h"

#include <QDir>
#include <QMap>

class QAction;

namespace udg {

class Volume3DViewTesting;
class Volume;
class VtkVolume;
class VtkVolumeProperty;
//class Volume3DFusedVolume;
class Volume3DFusedViewer;
class ToolManager;
class QClutEditorDialog;

/**
 * ...
 *
 * @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QVolume3DViewFusedTestingExtension : public QWidget, private ::Ui::QVolume3DViewFusedTestingExtensionBase {
Q_OBJECT
public:

    QVolume3DViewFusedTestingExtension( QWidget * parent = 0 );
    virtual ~QVolume3DViewFusedTestingExtension();

    /// Li donem l'input a processar
    void setInput( Volume * input );

signals:
    /// Signal que s'emet quan s'escull obrir un Key Image Note. Es passa el path d'aquest
    void openKeyImageNote( const QString & path);

    /// Signal que s'emet quan s'escull obrir un Presentation State. Es passa el path d'aquest
    void openPresentationState( const QString & path);

    /// Senyal que s'emet quan s'han escollit un o més arxius que seran processats externament
    void selectedFiles( QStringList );


public slots:
    /// Actualitza el mètode de rendering que s'aplica al volum segons el que s'ha escollit al combo box
    void updateRenderingMethodFromCombo( int index );

    /// Aplica una funció de transferència predefinida a la visualització.
    void applyPresetClut( const QString & clutName );

    /// Mostra el diàleg per editar funcions de transferència.
    void showClutEditorDialog();

    /// Aplica la funció de transferència passada a la visualització.
    void applyClut( const TransferFunction & clut );

	/// Aplica una funció de transferència predefinida a la visualització.
    void applyPresetClut2( const QString & clutName );

    /// Mostra el diàleg per editar funcions de transferència.
    void showClutEditorDialog2();

    /// Aplica la funció de transferència passada a la visualització.
    void applyClut2( const TransferFunction & clut );

	 /// Aplica una funció de transferència predefinida a la visualització del model fusionat.
    void applyFusedPresetClut( const QString & clutName );

    /// Mostra el diàleg per editar funcions de transferència del model fusionat.
    void showFusedClutEditorDialog();

    /// Aplica la funció de transferència passada a la visualització del model fusionat.
    void applyFusedClut( const TransferFunction & clut );

    void manageClosedDialog();

	///Obre el segon model
	void openSecondary();
	///Activa els shaders
	void shadeOn();
	///Desactiva els shaders
	void shadeOff();
	
	///Fusiona models
	void fuse();
	///Genera la fusió sumant i div 2
	void fuseModels();
	///Genera la fusió agafant els valors alternadament
	void fuseModels2();
	///Genera la fusió agafant l'array de RGB del volum
	void fuseModels3();
	///Metode necessari per fer la fusio3
	//void renderRayCasting2(vtkVolume * volume);
	void changeFusionMethod(int method);



	///Gestió de l'obertura de fitxer
	void userCancellation();


private:
    /// posa a punt les tools que es poden fer servir en l'extensió
    void initializeTools();

    /// Obté la llista de funcions de transferència predefinides.
    void loadClutPresets();

    /// Estableix les connexions de signals i slots
    void createConnections();

    /// Lectura/escriptura de configuració
    void readSettings();
    void writeSettings();



private:
	 /// Directori de treball per fitxers ordinaris
    QString m_workingDirectory;

    /// Directori de treball per directoris dicom
    QString m_workingDicomDirectory;
	
    /// Última Extenció seleccionada per l'usuari
    QString m_lastExtension;
	
    ///per controlar si l'usuari cancel·la l'acció d'obrir un fitxer.
    bool m_userCancellation;
	
	///Paràmetre per activar 

    /// El volum d'input
    Volume *m_input;
	//Volume3DFusedVolume *m_inp;

	/// Volum secundari
	Volume *m_secondaryVolume;

	///Volum Fusionat
	Volume *m_fused;

	///Imatges
	vtkImageData *m_image;
	vtkImageData *m_image2;
	vtkImageData *m_imageCopy;

	///Array de valors
	unsigned char *m_data;
	unsigned char *m_data2;

	///Mida dels valors de propietat
    int m_dataSize;
	int m_dataSize2;
	///Metode de fusió
	int m_mode;

    /// Gestor de tools
    ToolManager *m_toolManager;

    /// Directori de funcions de transferència predefinides.
    QDir m_clutsDir;
    /// Mapa entre noms de funcions de transferència i el nom de fitxer corresponent.
    QMap<QString, QString> m_clutNameToFileName;
    /// Valor de propietat màxim del volum.
    unsigned short m_maximumValue;

    QClutEditorDialog * m_clutEditorDialog;
	QClutEditorDialog * m_clutEditorDialog2;
	QClutEditorDialog * m_FusedClutEditorDialog;



};

} // end namespace udg



#endif
