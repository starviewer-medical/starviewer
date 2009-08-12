/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGISOIMAGEFILECREATOR_H
#define UDGISOIMAGEFILECREATOR_H

#include <QString>

namespace udg {

/** Crea un arxiu d'imatge ISO a partir d'un directori o fitxer.
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class IsoImageFileCreator {

public:

    enum ImageFileCreationError { InputPathNotExist, OutputIsoImageFilePathNotExist, OutputIsoImageDirPathNotPermissions, OutputIsoImageFilePathNotPermissions, InternError };

    IsoImageFileCreator();
    IsoImageFileCreator( QString inputPath, QString outputIsoImageFilePath );
    ~IsoImageFileCreator();

    /// Estableix el label del fitxer d'imatge ISO que es vol crear
    void setIsoImageLabel( QString isoImageLabel );

    /// Estableix el path del fitxer o directori a partir del qual es vol crear el fitxer d'imatge ISO
    void setInputPath( QString inputPath );

    /// Estableix el path del fitxer d'imatge ISO que es vol crear
    void setOutputIsoImageFilePath( QString outputIsoImageFilePath );

    /// Retorna el label del fitxer d'imatge ISO que es vol crear
    QString getIsoImageLabel();

    /// Retorna el path del fitxer o directori a partir del qual es vol crear el fitxer d'imatge ISO
    QString getInputPath();
    
    /// Retorna el path del fitxer d'imatge ISO que es vol crear
    QString getOutputIsoImageFilePath();

    /// Retorna la descripció de l'últim error que s'ha produit
    QString getLastErrorDescription();

    /// Retorna l'últim error que s'ha produit
    ImageFileCreationError getLastError();

    /** Crea el fitxer d'imatge ISO a partir d'un directori o fitxer
     * El path del fitxer d'imatge ISO que es crearà es troba a l'atribut m_inputPath
     * El path del directori o fitxer a partir del qual es crea el fitxer d'imatge ISO es troba a l'atribut m_outputIsoImageFilePath
     * @return true si la imatge s'ha creat correctament i false en cas contrari
     */
    bool createIsoImageFile();

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
};

}

#endif
