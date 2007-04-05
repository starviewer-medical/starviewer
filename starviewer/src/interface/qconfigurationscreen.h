/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQCONFIGURATIONSCREEN_H
#define UDGQCONFIGURATIONSCREEN_H

#include "ui_qconfigurationscreenbase.h"

using namespace Ui; // \TODO això s'hauria d'evitar!

namespace udg {

class Status;
/** Interfície que permet configurar els paràmetres del pacs i de la caché
@author marc
*/
class QConfigurationScreen : public QDialog , private QConfigurationScreenBase
{
Q_OBJECT

public:

    /// Constructor de la classe
    QConfigurationScreen( QWidget *parent = 0 );

    ///Destructor de classe
    ~QConfigurationScreen();

public slots :

    /// Neteja els line edit de la pantalla
    void clear();

    /// Slot que dona d'alta el PACS a la la base de dades
    void addPacs();

    /// Slot que s'activa quant seleccionem un Pacs del PacsListView, emplena les caixes de texts amb les dades del Pacs
    void selectedPacs( QTreeWidgetItem * item , int );

    /// Slot que updata les dades d'un pacs
    void updatePacs();

    /// Slot que esborra el pacs seleccionat
    void deletePacs();

    /// Fa un echo a les dades del PACS que estan als textbox
    void test();

    /// Mostra un QDialog per especificar on es troba la base de dades de la caché
    void examinateDataBaseRoot();

    /// Mostra un QDialog per especificar on s'han de guardar les imatges descarregades
    void examinateCacheImagePath();

    /// Esborra tota la caché
    void deleteStudies();

    /// Compacta la base de dades de la cache
    void compactCache();

    /// Aplica els canvis de la configuració
    bool applyChanges();

    /// Guarda els canvis a la configuració dels paràmetres del PACS
    void acceptChanges();

    /// Tanca la pantalla de configuració, i desprecia els canvis
    void cancelChanges();

    /// Slot que s'utilitza quant es fa algun canvi a la configuració, per activar els buttons apply
    void configurationChanged( const QString& );

    /// Slot que s'utilitza quant es fa algun canvia el path de la base de dades, per activar els buttons apply
    void configurationChangedDatabaseRoot( const QString& );

    /// Afegeix la '/' al final del path del directori si l'usuari no l'ha escrit
    void cacheImagePathEditingFinish();

    /// crear base de dades
    void createDatabase();

signals :

    ///signal que s'emet quan hi ha algun canvi a la llista de PACS, per a que la QPacsList es pugui refrescar
    void pacsListChanged();

    ///signal que s'emet quan la cache ha estat netejada cap a QueryScreen, pq netegi el QStudyTreeView que mostra els estudis de la cache
    void cacheCleared();

protected :

    /** Event que s'activa al tancar al rebren un event de tancament
     * @param event de tancament
     */
    void closeEvent( QCloseEvent* ce );

private :

    int m_PacsID; /// Conté el D del pacs seleccionat en aquell moment
    bool m_configurationChanged; ///Indica si la configuració ha canviat
    bool m_createDatabase; /// Indica si s'ha comprovat demanat que es creï la base de dades indicada a m_textDatabaseRoot

    ///crea els connects dels signals i slots
    void createConnections();

    /** Carreguem la mida de les columnes del QTreeWidget de l'última vegada que es va tancar la pantalla. La mida de les columnes la tenim guardada al StarviewerSettings
     */
    void setWidthColumns();

    /** Comprovem que els paràmetres dels PACS siguin correctes.
     *  1r Que el AETitle no estigui en blanc,
     *  2n Que l'adreça del PACS no estigui en blanc,
     *  3r Que el Port del Pacs sigui entre 0 i 65535
     *  4t Que l'institució no estigui buida
     * @return bool, retorna cert si tots els parametres del pacs son correctes
     */
    bool validatePacsParameters();

    /** Valida que els canvis de la configuració siguin correctes
     *  Port local entre 0 i 65535
     *  Numero màxim de connexions 25
     *  Path de la base de dades i directori dicom's existeix
     *  @return indica si els canvis son correctes
     */
    bool validateChanges();

    /// Emplena el ListView amb les dades dels PACS que tenim guardades a la bd
    void fillPacsListView();

    /** Tracta els errors que s'han produït a la base de dades en general
     *           @param state  Estat del mètode
     */
    void databaseError(Status * state);

    /// Carrega les dades de configuració de la cache
    void loadCacheDefaults();

    /// Emplena els textboxs amb les dades del PACS
    void loadPacsDefaults();

    /// Carrega la informació de la institució
    void loadInstitutionInformation();

    /// calcula les dades del pool
    void loadCachePoolDefaults();

    /// Guarda els canvis a la configuració dels paràmetres del PACS
    void applyChangesPacs();

    ///  Aplica els canvis fets a la configuració de la cache
    void applyChangesCache();

    /// Aplica els canvis fets a la informació de la institució
    void applyChangesInstitution();

	/// col·loca les icones als buttons d'acceptar, cancel·lar i applicar de l'apartat de Pacs i la Cache
	void setIconButtons();

    /** Guarda la mida de les columnes del QTreeWidget de la pestanya de PACS Device al StarviewerSettings
     */
    void saveColumnsWidth();

};

};// end namespace udg

#endif
