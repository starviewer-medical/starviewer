#ifndef SERIES
#define SERIES

#include <list>
#include <string>
#include <iostream>

/** This class sets and gets series fields
 */

namespace udg{

class Series
{
    
 public :
    
    /// Initialize the fields of the series
    Series();
    
    /** Operador que ens ajudar a ordenar les series pel camp seriesNumber
     * @param Serie amb la que hem de comparar
     */
    bool operator<( Series );
    
    /** This action set the series Number
     * @param Series Number
     */
    void setSeriesNumber( std::string );
    
    /** This action set the series description 
     * @param Series description
     */
    void setSeriesDescription( std::string );
    
    /** This action set the series modality
     * @param series modality
     */
    void setSeriesModality( std::string );
    
    /** This action set the series date.  
     * @param Series date  
     */
    void setSeriesDate( std::string );
    
    /** This action set the series time.  
     * @param Series time  
     */
    void setSeriesTime( std::string );
    
    /** This action set the study UID          
     * @param  study UID
     */
    void setStudyUID( std::string );
    
    /** This action set the series UID        
     * @param   Series UID 
     */
    void setSeriesUID( std::string );
    
    /** This action set the operator's name 
     * @param  Series name 
     */
    void setOperatorName( std::string );
    
    /** This action set the series Protocol name       
     * @param   Series Protocol 
     */
    void setProtocolName( std::string );
    
    /** This action set the body part examinated  
     * @param Body part examinated in the series 
     */
    void setBodyPartExaminated( std::string );
    
    /** This action set series paths in de local data base 
     * @param  Series path
     */
    void setSeriesPath( std::string );

    /** Estableix el número d'imatges que conté la serie
     * @param Número d'imatges
     */    
    void setImageNumber( int );
    
    /** This function get the Series Number  
     * @return  Series Number  
     */
    std::string getSeriesNumber();
    
    /** This function get the Series Description  
     * @return  Series Description  
     */ 
    std::string getSeriesDescription();
    
    /** This function get the Series Modality
     * @return  Series Modality
     */
    std::string getSeriesModality();
    
    /** This function get the Series Time    
     * @return  Series Time   
     */
    std::string getSeriesTime();
    
    /** This function get the Series Date    
     * @return  Series Date    
     */
    std::string getSeriesDate();
    
    /** This function get the Study UID   
     * @return  Study UID         
     */
    std::string getStudyUID();

    /** This function get the Series UID  
     * @return  Series's UID  
     */
    std::string getSeriesUID();
    
    /** This function get the operator name  
     * @return  operator name   
     */
    std::string getOperatorName();
    
    /** This function get the Protocol Name  
     * @return  Protocol Name 
     */
    std::string getProtocolName();
    
    /** This function get the Body Part Examinated
     * @return  SBody Part Examinated 
     */
    std::string getBodyPartExaminated();
    
    /** This function get the series path in the local data base
     * @return  path 
     */
    std::string getSeriesPath();
    
    /** retorna el número d'imatges que conté la serie
     * @return Número d'imatges
     */
    int         getImageNumber();

 private :
    
    std::string m_seriesUID;
    std::string m_seriesDescription;
    std::string m_seriesDate;
    std::string m_seriesTime;
    std::string m_seriesModality;
    std::string m_seriesNumber;
    std::string m_studyUID;
    std::string m_protocolName;
    std::string m_operatorName;
    std::string m_bodyPartExaminated;
    std::string m_seriesPath;
    int         m_imageNumber;
};
};
#endif

