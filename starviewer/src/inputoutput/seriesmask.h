/***************************************************************************
 *   Copyright (C) 2005 by marc                                            *
 *   marc@localhost.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef SERIESMASK
#define SERIESMASK
#define HAVE_CONFIG_H 1

//necessitem tipus d'aquests classes que són structs,per tant no se'ns permet fer "forward declaration"
#include <dimse.h> // provide the structure DcmDataSet
#include <dcdeftag.h> //provide the information for the tags
#include <ofcond.h> //provide the OFcondition structure and his members
#include "status.h"

#ifndef CONST
#include "const.h"     
#endif


/** This class construct a mask to search series in the pacs 
 */
namespace udg{
class SeriesMask{

public:

    SeriesMask();

    Status setSeriesNumber(std:: string );
    Status setSeriesDate(std::string date);
    Status setSeriesDate(std::string dateMin,std::string dateMax);
    Status setSeriesTime(std::string);
    Status setSeriesTime(std::string timeMin,std::string timeMax);
    Status setSeriesDescription(std::string);  
    Status setSeriesModality(std::string);  
    Status setSeriesOperator(std::string);
    Status setSeriesBodyPartExaminated(std::string);
    Status setSeriesProtocolName(std::string);
    
    Status setStudyUID(std::string);
    Status setSeriesUID(std::string date);
    
    /** Retorna el series Number
    *            @return   series Number 
    */
    std::string getSeriesNumber();
    
    /** Retorna la data de la sèrie
    *            @return   data de la sèrie
    */
    std::string getSeriesDate();
    
    /** Retorna l'hora de la sèrie
    *            @return   hora de la sèrie
    */    
    std::string getSeriesTime(); 
    
    /** Retorna l'hora de la sèrie
    *            @return   hora de la sèrie
    */    
    std::string getSeriesTime( std::string minTime , std::string maxTime );       
        
    /** Retorna la descripcio de la sèrie
      *         @return descripcio de la serie
      */
    std::string getSeriesDescription();  
      
    /** Retorna la modalitat de la sèrie
      *         @return modalitat de la sèrie
      */
    std::string getSeriesModality();  
      
      
    /** Retorna l'operador que captat la serie
      *         @return operdor
      */  
    std::string getSeriesOperator();
    
    /** Retorna la part del cos examinada en la serie
      *         @return part del cos examinada
      */
    std::string getSeriesBodyPartExaminated();
    
    /** Retorna el nom del protocol utiltizat la serie
      *         @return nom del protocol utilitzat a la seire
      */
    std::string getSeriesProtocolName();
      
    /** Retorna l'uid de la serie
      *         @return SeriesUID
      */  
    std::string getSeriesUID();
    
    /** Retorna l'UID de l'estudi 
      *         @return StudyUID
      */
    std::string getStudyUID();
      
    DcmDataset* getSeriesMask();
   
private:

   DcmDataset *m_seriesMask;
   
   void retrieveLevel();

};
};
#endif
