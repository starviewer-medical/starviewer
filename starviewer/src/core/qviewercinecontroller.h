/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQVIEWERCINECONTROLLER_H
#define UDGQVIEWERCINECONTROLLER_H

#include <QObject>

class QAction;
class QBasicTimer;

namespace udg {

class QViewer;
class Q2DViewer;
class Volume;

/**
Controlador de seqüències de CINE acoplable a qualsevol QViewer

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QViewerCINEController: public QObject
{
Q_OBJECT
public:
    QViewerCINEController(QObject *parent = 0);

    ~QViewerCINEController();

    /**
     * Li assignem el viewer amb el que treballar
     * @param viewer
     */
    void setInputViewer( QViewer *viewer );

    enum CINEDimensionType{ SpatialDimension, TemporalDimension };
    void setCINEDimension( int dimension );

    QAction *getPlayAction() const;
    QAction *getRecordAction() const;
    QAction *getLoopAction() const;
    QAction *getBoomerangAction() const;

    /**
     * Li donem el path absolut de l'arxiu on es gravarà el vídeo
     * @param filename Path absolut, amb o sense extensió
     */
    void setRecordFilename( const QString filename );

signals:
    void playing();
    void paused();
    void recording( int progress );
    void velocityChanged( int velocity );

public slots:
    /// Engega la reproducció
    void play();

    /// Pausa/para la reproducció
    void pause();

    /// Grava en vídeo la seqüència corresponent
    void record();

    /// Li donem la velocitat de reproducció expresada en el nombre d'imatges que volem veure per segon
    void setVelocity( int imagesPerSecond );

    /// L'indiquem que volem engegar el mode loop
    /// En aquest mode, un cop arribem a la última imatge, tornem a començar de nou des de la inicial
    void loopMode( bool enable );

    /// L'indiquem que volem engegar el mode boomerang
    /// En aquest mode es recorren les imatges repetidament en l'ordre 1..n n..1 (endavant i endarrera)
    void boomerangMode( bool enable );

    /// Li indiquem l'interval de reproducció
    void setPlayInterval( int firstImage, int lastImage );

    /**
     * Reseteja la informació de CINE segons l'input donat
     * @param input
     */
    void resetCINEInformation(Volume *input);

protected:
    void timerEvent(QTimerEvent *event);

private:
    /**
     * Aquí ens ocupem de decidir cap on va el següent frame
     * durant la reproducció
     */
    void handleCINETimerEvent();

private:
    /// Variables de reproducció
    int m_firstSliceInterval;
    int m_lastSliceInterval;

    /// ens indica si la reproducció va cap endavant o cap endarera
    int m_nextStep;
    int m_velocity;
    QBasicTimer *m_timer;

    Q2DViewer *m_2DViewer;

    /// Indica si s'està reproduint o no
    bool m_playing;

    /// Dimensió sobre la qual es reproduirà
    int m_cineDimension;

    QAction *m_playAction;
    QAction *m_recordAction;
    QAction *m_loopAction;
    QAction *m_boomerangAction;

    QString m_recordFilename;

};

}

#endif
