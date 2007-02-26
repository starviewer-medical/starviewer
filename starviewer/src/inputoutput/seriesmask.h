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

/** Aquesta classe construiex la màscara de cerca per a les sèries
   *Cada camp que volem que la cerca ens retorni li hem de fer el set, sinó retornarà valor null per aquell camp. Per tots els camps podem passar, cadena buida que significa que buscarà tots els valors d'aquell camp, o passar-li un valor en concret, on només retornarà els compleixin aquell criteri.
 */
namespace udg{

class Status;

class SeriesMask
{

public:

    /** Constructor de la màscara de series, per defecte se li ha de passar com a mínim l'UID de l'estudi pel qual volem buscar les sèries
     *
     */
    SeriesMask();

    /** This action especified in the search which series number we want to match
     * @param seriesNumber' Number of the series to search. If this parameter is null it's supose that any mask is applied at this field
     * @return estat del mètode
     */
    Status setSeriesNumber(std:: string );

    /** Aquest mètode especifica per quina data s'ha de buscar la serie. El format es YYYYMMDD
     * Si passem una data sola, per exemple 20040505 només buscara sèries d'aquell dia
     * Si passem una data amb un guio a davant, per exemple -20040505 , buscarà sèries fetes aquell dia o abans
     * Si passem una data amb un guio a darrera, per exemple 20040505- , buscarà series fetes aquell dia, o dies posteriors
     * Si passem dos dates separades per un guio, per exemple 20030505-20040505 , buscarà sèries fetes entre aquelles dos dates
     * @param series data a cercar la sèrie
     * @return estat del mètode
     */
    Status setSeriesDate(std::string date);

     /** Especifica l'hora de la serie a buscar, les hores es passen en format HHMM
      *         Si es buit busca per qualsevol hora.
      *         Si se li passa una hora com 1753, buscarà series d'aquella hora
      *         Si se li passa una hora amb guió a davant, com -1753 , buscarà sèries d'aquella hora o fetes abans
      *         Si se li passa una hora amb guió a darrera, com 1753- , buscarà series fetes a partir d'aquella hora
      *         Si se li passa dos hores separades per un guió, com 1223-1753 , buscarà series fetes entre aquelles hores
      * @param Series Hora de la serie
      * @retun estat del mètode
      */
    Status setSeriesTime(std::string);

    /** This action especified that in the search we want the seriess description
     * @param Series description of the study to search. If this parameter is null it's supose that any mask is applied at this field.
     * @return estat del mètode
     */
    Status setSeriesDescription(std::string);

    /** This action especified that in the search we want the series modality
     * @param series modality the study to search. If this parameter is null it's supose that any mask is applied at this field
     * @return estat del mètode
     */
    Status setSeriesModality(std::string);

    /** This action especified that in the search we want to query the operator's name
     * @param Operator's name. If this parameter is null it's supose that any mask is applied at this field
     * @return estat del mètode
     */
    Status setSeriesOperator(std::string);

    /** This action especified that in the search we want to query the body part examinated
     * @param Body Part. If this parameter is null it's supose that any mask is applied at this field
     * @return estat del mètode
     */
    Status setSeriesBodyPartExaminated(std::string);

    /** This action especified that in the search we want to query the Protocol Name
     * @param Protocol Name. If this parameter is null it's supose that any mask is applied at this field
     * @return estat del mètode
     */
    Status setSeriesProtocolName(std::string);

    /** This action especified that in the search we want to query the series that have this Study UID
     * @param Study instance UID the study to search. If this parameter is null it's supose that any mask is applied at this field
     * @return estat del mètode
     */
    Status setStudyUID(std::string);

     /** This action especified that in the search we want the seriess description
     * @param Series description of the study to search. If this parameter is null it's supose that any mask is applied at this field.
     * @return estat del mètode
     */
    Status setSeriesUID(std::string date);

    /** Retorna el series Number
     * @return   series Number
     */
    std::string getSeriesNumber();

    /** Retorna la data de la sèrie
     * @return   data de la sèrie
     */
    std::string getSeriesDate();

    /** Retorna l'hora de la sèrie
     * @return   hora de la sèrie
     */
    std::string getSeriesTime();

    /** Retorna la descripcio de la sèrie
     * @return descripcio de la serie
     */
    std::string getSeriesDescription();

    /** Retorna la modalitat de la sèrie
     * @return modalitat de la sèrie
     */
    std::string getSeriesModality();

    /** Retorna l'operador que captat la serie
     * @return operdor
     */
    std::string getSeriesOperator();

    /** Retorna la part del cos examinada en la serie
     * @return part del cos examinada
     */
    std::string getSeriesBodyPartExaminated();

    /** Retorna el nom del protocol utiltizat la serie
     * @return nom del protocol utilitzat a la seire
     */
    std::string getSeriesProtocolName();

    /** Retorna l'uid de la serie
     * @return SeriesUID
     */
    std::string getSeriesUID();

    /** Retorna l'UID de l'estudi
     * @return StudyUID
     */
    std::string getStudyUID();

    /** Return the generated search mask
     * @return returns de search mask
     */
    DcmDataset* getSeriesMask();

private:

   DcmDataset *m_seriesMask;

   /** This action especified that the query search, will use the retrieve level Study. For any doubts about this retrieve level and the query/retrieve fields, consult DICOMS's documentation in Chapter 4, C.6.2.1
    */
   void retrieveLevel();

};
}; //end namespace udg
#endif
