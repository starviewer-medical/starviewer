/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGTESTDICOMOBJECTS_H
#define UDGTESTDICOMOBJECTS_H


namespace udg {

class Image;
class Patient;
class Study;
class Series;

class TestDicomObjects 
{

public:

    static void printImage(Image *imateToPrint);

};

}

#endif
