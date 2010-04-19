/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQLOCALDATABASECONFIGURATIONSCREEN_H
#define UDGQLOCALDATABASECONFIGURATIONSCREEN_H

#include "ui_qlocaldatabaseconfigurationscreenbase.h"

namespace udg {

class Status;

/** Widget en el que es configuren els paràmetres de la Base de Dades local
*/
class QLocalDatabaseConfigurationScreen : public QWidget, private Ui::QLocalDatabaseConfigurationScreenBase
{
Q_OBJECT

public:
    /// Constructor de la classe
    QLocalDatabaseConfigurationScreen( QWidget *parent = 0 );

    ///Destructor de classe
    ~QLocalDatabaseConfigurationScreen();

public slots:
    /// Aplica els canvis de la configuració
    bool applyChanges();

signals:
    /// Signal que s'emet cada vegada que hi ha un canvi a la configuració que pot afectar al queryscreen
    void configurationChanged(const QString& configuration);

private slots:
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

    /// Tracta els errors que s'han produït a la base de dades en general
    void showDatabaseErrorMessage( const Status &state );

    /// Carrega les dades de configuració de la cache
    void loadCacheDefaults();

    ///  Aplica els canvis fets a la configuració de la cache
    void applyChangesCache();

    /** Valida que els canvis de la configuració siguin correctes
     *  Path de la base de dades i directori dicom's existeix
     *  @return indica si els canvis son correctes
     */
    bool validateChanges();

private:
    bool m_configurationChanged; ///Indica si la configuració ha canviat
    bool m_createDatabase; /// Indica si s'ha comprovat demanat que es creï la base de dades indicada a m_textDatabaseRoot
};

};// end namespace udg

#endif
