#ifndef SERIES
#define SERIES

#include <list>
#include <string>
#include <iostream>

/** This class sets and gets series fields
 */

namespace udg{
class Series{
    
 public :
    
    Series();
    bool operator<(Series);
    
    void setSeriesNumber(std::string );
    void setSeriesDescription(std::string);
    void setSeriesModality(std::string);
    void setSeriesDate(std::string );
    void setSeriesTime(std::string);
    void setStudyUID(std::string);
    void setSeriesUID(std::string);
    void setOperatorName(std::string);
    void setProtocolName(std::string);
    void setBodyPartExaminated(std::string);
    void setSeriesPath(std::string);
    void setImageNumber(int);
     
    std::string getSeriesNumber( );
    std::string getSeriesDescription( );
    std::string getSeriesModality( );
    std::string getSeriesTime(  );
    std::string getSeriesDate(  );
    std::string getStudyUID(  );
    std::string getSeriesUID( );
    std::string getOperatorName( );
    std::string getProtocolName( );
    std::string getBodyPartExaminated( );
    std::string getSeriesPath( );
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

