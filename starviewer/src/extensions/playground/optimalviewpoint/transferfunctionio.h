/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/



#ifndef UDGTRANSFERFUNCTIONIO_H
#define UDGTRANSFERFUNCTIONIO_H



class QFile;
class QString;



namespace udg {



class TransferFunction;



/**
 * @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
 */
class TransferFunctionIO
{

private:

    TransferFunctionIO();
    ~TransferFunctionIO();

public:

    static TransferFunction * fromFile( QFile & file );
    static TransferFunction * fromFile( const QString & fileName );

    static void toFile( QFile & file, const TransferFunction & transferFunction );
    static void toFile( const QString & fileName, const TransferFunction & transferFunction );

};



}



#endif
