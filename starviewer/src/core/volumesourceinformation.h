/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGVOLUMESOURCEINFORMATION_H
#define UDGVOLUMESOURCEINFORMATION_H

#include <QString>
#include <QStringList>
#include <vector>

namespace udg {

class DICOMTagReader;

/**
Conté informació relacionada amb el volum, com per exemple dades del pacient.

@author Grup de Gràfics de Girona  ( GGG )
*/
class VolumeSourceInformation
{
public:
    VolumeSourceInformation();

    ~VolumeSourceInformation();

    /// Assigna/ retorna el window/level que ens proporciona el DICOM
    void setWindowLevel( double window , double level )
    {
        m_windowLevel[0] = window;
        m_windowLevel[1] = level;
    }
    void setWindowLevel( double windowLevel[2] )
    {
        this->setWindowLevel( windowLevel[0] , windowLevel[1] );
    }
    void setWindow( double window )
    {
        m_windowLevel[0] = window;
    }
    void setLevel( double level )
    {
        m_windowLevel[1] = level;
    }
    /// Retorna els valors de window level. Per defecte en retorna el primer, però en poden haver-hi més. Si poition està fora de rang retornarà el primer.
    double getWindow( int position = 0 );
    double getLevel( int position = 0 );
    double *getWindowLevel( int position = 0 );
    void getWindowLevel( double &window , double &level, int position = 0 );
    void getWindowLevel( double windowLevel[2], int position = 0 );

    /// Retorna les descripcions associades als window levels
    QString getWindowLevelDescription( int position = 0 );

    /// Ens diu si té un window level vàlid o no
    bool hasWindowLevel();

    /// Ens retorna en nombre de windows levels que conté la imatge
    int getNumberOfWindowLevels();

    /// Indiquem la llista de fitxers que formen el volum i es carrega el dicomdataset (privat)
    void setFilenames( QStringList filenames );
    void setFilenames( QString filename ); // ho sobrecarreguem per quan només es tracti d'un sol arxiu

    /// Obtenim la llista d'arxius que conformen el volum
    QStringList getFilenames() const { return m_filenamesList; }

    /// Diu si existeixen valors de rescale
    bool hasModalityRescale();

    /// Recupera els valors de rescale slope/intercept
    double getRescaleSlope();
    double getRescaleIntercept();

    // Mètodes per tractar imatges de tipus multi-frame
    /// Diu si el volum és de tipus multiframe \TODO encara no estem segurs de que es faci aquesta consulta de forma correcta
    bool isMultiFrame();

    /// Ens diu el nombre de frames del que està composat el volum
    int getNumberOfFrames();

private:
    /// Carrega les dades dicom d'un arxiu
    bool loadDicomData( QString filename );

private:
    double m_windowLevel[2];

    /// Llista dels windows levels de la imatge
    std::vector< double* > m_windowLevelList;

    /// Llista de les descripcions dels windows levels ( si n'hi ha )
    QStringList m_windowLevelDescriptions;

    /// Vector amb els noms dels fitxers que conformen el volum
    QStringList m_filenamesList;

    /// Lector d'informació DICOM
    DICOMTagReader *m_dicomTagReader;
};

};  //  end  namespace udg

#endif
