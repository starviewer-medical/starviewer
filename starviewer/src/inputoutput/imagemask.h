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

#ifndef IMAGEMASK
#define IMAGEMASK
#define HAVE_CONFIG_H 1

//necessitem tipus d'aquests classes que són structs,per tant no se'ns permet fer "forward declaration"
#include <dimse.h> // provide the structure DcmDataSet
#include <dcdeftag.h> //provide the information for the tags
#include <ofcond.h> //provide the OFcondition structure and his members
#include "status.h"
#include <string>
#ifndef CONST
#include "const.h"     
#endif

/** Aquesta classe permet construir una màscara per a cercar imatges 
 */
namespace udg{
class ImageMask{

public:

    ///Constructor de la classe
    ImageMask();
        
    /** Set ImageNumber.
     * @param image Number
     * @return The status of the action
     */
    Status setStudyUID( std::string );
    
    /** Set SeriesUID.
     * @param Series UID
     * @return The status of the action
     */
    Status setSeriesUID( std::string );
    
    /** set the StudyId of the images
     * @param   Study instance UID the study to search. If this parameter is null it's supose that any mask is applied at this field
     * @return The state of the action
     */
    Status setImageNumber( std::string );
    
    /** Retorna el uid de l'estudi
      * @return StudyUID
      */
    std::string getStudyUID();
    
    /** Retorna el UID de la serie
      * @return SeriesUID
      */
    std::string getSeriesUID();
    
    /** Retorna el número d'imatge
      * @return número d'imatge
      */
    std::string getImageNumber();
    
    /** Return the generated image mask
     * @return returns the image mask
     */
    DcmDataset* getImageMask();
    
private:

   DcmDataset *m_imageMask;
   
   /// This action especified that the query search, will use the retrieve level I. For any doubts about this retrieve level and the query/retrieve fields consult DICOMS's documentation in Chapter 4, C.6.2.1
   void retrieveLevel();

};
}; //end namespace udg
#endif
