/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGISOIMAGEFILECREATOR_H
#define UDGISOIMAGEFILECREATOR_H

#include <QString>
#include <QObject>

class QProcess;

namespace udg {

/** Crea un arxiu d'imatge ISO a partir d'un directori o fitxer.
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class IsoImageFileCreator: public QObject {
Q_OBJECT
public:

    enum ImageFileCreationError { InputPathNotFound, OutputPathNotFound, OutputDirPathAccessDenied, OutputFilePathAccessDenied, InternalError };

    IsoImageFileCreator();
    IsoImageFileCreator( const QString &inputPath, const QString &outputIsoImageFilePath );
    ~IsoImageFileCreator();

    /// Estableix/Retorna el label del fitxer d'imatge ISO que es vol crear
    QString getIsoImageLabel() const;
    void setIsoImageLabel( const QString &isoImageLabel );

    /// Estableix/Retorna el path del fitxer o directori a partir del qual es vol crear el fitxer d'imatge ISO
    QString getInputPath() const;
    void setInputPath( const QString &inputPath );

    /// Estableix/Retorna el path del fitxer d'imatge ISO que es vol crear
    QString getOutputIsoImageFilePath() const;
    void setOutputIsoImageFilePath( const QString &outputIsoImageFilePath );
  
    /// Retorna la descripció de l'últim error que s'ha produit
    QString getLastErrorDescription() const;

    /// Retorna l'últim error que s'ha produit
    ImageFileCreationError getLastError() const;

    /** Inicia la creació del fitxer d'imatge ISO a partir d'un directori o fitxer
     * El path del fitxer d'imatge ISO que es crearà es troba a l'atribut m_inputPath
     * El path del directori o fitxer a partir del qual es crea el fitxer d'imatge ISO es troba a l'atribut m_outputIsoImageFilePath
     * És important tenir en compte que només inicia el procés no espera a la seva finalització. Per saber quan acaba es disposa del 
     + signal finishedCreateIsoImageFile que s'emet just al finalitzar el procés.
     */
    void startCreateIsoImageFile();

signals:

    /// Signal que s'emet al finalitzar el procés de generació del fitxer d'imatge ISO. Té un booleà per indicar si el procés ha finalitzat correcta o incorrectament
    void finishedCreateIsoImageFile(bool result);

private slots:

    /// Slot que contidrà els últims passos que cal realitzar abans d'emetre el signal finishedCreateIsoImageFile
    void finishCreationProcess(int exitCode);

private:

    /// Label de la imatge ISO que es vol crear
    QString m_isoImageLabel;

    /// Path del directori o fitxer a partir del qual volem generar el fitxer d'imatge ISO
    QString m_inputPath;

    /// Path del fitxer d'imatge ISO que es vol generar
    QString m_outputIsoImageFilePath;

    /// Descripció de l'últim error que s'ha produit
    QString m_lastErrorDescription;

    /// Últim error que s'ha produit
    ImageFileCreationError m_lastError;

    /// Process per la creació de la imatge ISO
    QProcess *m_process;
};

}

#endif
