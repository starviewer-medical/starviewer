/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDATABASEINSTALLATION_H
#define UDGDATABASEINSTALLATION_H

#include <QObject>
#include <QProgressDialog>

namespace udg {

/** Aquesta classe comprova que els directoris i la base de dades de la cache estiguin correctament creats si no es aquest el cas, els crea, per a que l'aplicacio pugui funcionar correctament
 *	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
 */
class DatabaseInstallation : public QObject {
Q_OBJECT
public:

    DatabaseInstallation();
    ~DatabaseInstallation();

    /** Comprova que el directori on es guarda la base dades, les imatges i la base de dades existeixin sinó l'intenta crear.
     * També comprova que la base de dades estigui en la revisió que necessita la compilació actual de l'starviewer i sinó la
     * intenta actualitzar
     * @return indica si la base de dades existeix
     */
    bool checkStarviewerDatabase();

    /** Comprova si existeix el directori de la base de dades
     * @return indica si el directori existeix
     */
    bool existsDatabasePath();

    /** Comprova si existeix el directori on es guarden les imatges descarregades
     * @return indica si el directori existeix
     */
    bool existsLocalImagePath();

    /** Comprova si existeix el fitxer de la base de dades
     * @return indica si el directori existeix
     */
    bool existsDatabaseFile();

    ///Esborra la base de dades actual i torna a crear-ne una de nova
    bool reinstallDatabase();

    ///reinstal·lar la base de dades i esborra les imatges descarregades o importades a la base de dades local. Mostra un QProgressDialog mentre es neteja la cache.
    bool removeCacheAndReinstallDatabase();

    ///Aplica els canvis a fets a la última revisió de la base de dades a la base de dades locals
    bool updateDatabaseRevision();

	/// Retorna els errors que s'han trobat
	QString getErrorMessage();

private slots:
    ///Fa avançar la barra de progrés
    void setValueProgressBar();

private:
   /** Comprova que el directori on es guarden les imatges descarregades existeixi si no l'intenta crear
     * @return indicat si el directori existeix o no
     */
    bool checkLocalImagePath();

    ///Comprova que existeix el path de la base de dades i sinó existeix el crea
    bool checkDatabasePath();

    ///Comprova si la revisió de la base de dades és la necessària per l'actual compilació de l'starviewer i sinó l'intenta actualitzar
    bool checkDatabaseRevision();

    ///Intenta reparar la base dedades, sinó ho aconseguei la reinstal·la
    bool repairDatabase();

    /// Retorna cert si tenim permisos d'escriptura a la base de dades, fals altrament
    bool isDatabaseFileWritable();

    /** Crea el directori per guardar les imatges de la cache
     * @return indica si s'ha pogut crear el directori
     */
    bool createLocalImageDir();

    /** Crea el directori per guardar la base de dades
     * @return indica si s'ha pogut crear el directori
     */
    bool createDatabaseDirectory();

    /** Crea la base de dades
     * @return indica si s'ha pogut crear la base de dades
     */
    bool createDatabaseFile();

private:
    /// Diàleg de progrés per les operacions costoses
    QProgressDialog *m_qprogressDialog;

	/// Missatges d'errors que s'han anat produint
	QString m_errorMessage;
};

}//end namespace udg

#endif
