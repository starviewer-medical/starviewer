/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQCONFIGURATIONSCREEN_H
#define UDGQCONFIGURATIONSCREEN_H

#include "ui_qconfigurationscreenbase.h"

namespace udg {

class Status;

/** Interfície que permet configurar els paràmetres del pacs i de la caché
@author marc
*/
class QConfigurationScreen : public QWidget, private ::Ui::QConfigurationScreenBase
{
Q_OBJECT

public:

    /// Constructor de la classe
    QConfigurationScreen( QWidget *parent = 0 );

    ///Destructor de classe
    ~QConfigurationScreen();

public slots:
    /// Aplica els canvis de la configuració
    bool applyChanges();

signals:
    /// Signal que s'emet cada vegada que hi ha un canvi a la configuració que pot afectar al queryscreen
    void configurationChanged(const QString& configuration);

protected:

    /// Event que s'activa al tancar al rebren un event de tancament
    void closeEvent( QCloseEvent* ce );

private slots:
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

    /// Slot que s'utilitza quant es fa algun canvi a la configuració, per activar els buttons apply
    void enableApplyButtons();

    /// Slot que s'utilitza quant es fa algun canvia el path de la base de dades, per activar els buttons apply
    void configurationChangedDatabaseRoot();

    /// Afegeix la '/' al final del path del directori si l'usuari no l'ha escrit
    void cacheImagePathEditingFinish();

    /// crear base de dades
    void createDatabase();

private:
    ///crea els connects dels signals i slots
    void createConnections();

    /// Crea els input validators necessaris pels diferents camps d'edició.
    void configureInputValidator();

    /// Carreguem la mida de les columnes del QTreeWidget de l'última vegada que es va tancar la pantalla.
    /// La mida de les columnes la tenim guardada al StarviewerSettings
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

    /// Tracta els errors que s'han produït a la base de dades en general
    void showDatabaseErrorMessage( const Status &state );

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

    /// Guarda la mida de les columnes del QTreeWidget de la pestanya de PACS Device al StarviewerSettings
    void saveColumnsWidth();

private:
    int m_selectedPacsID; /// Conté el D del pacs seleccionat en aquell moment
    bool m_configurationChanged; ///Indica si la configuració ha canviat
    bool m_createDatabase; /// Indica si s'ha comprovat demanat que es creï la base de dades indicada a m_textDatabaseRoot
};

};// end namespace udg

#endif
