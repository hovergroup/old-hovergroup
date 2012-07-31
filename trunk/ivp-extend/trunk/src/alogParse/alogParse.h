/*
 * alogParse.h
 *
 *  Created on: Jul 31, 2012
 *      Author: josh
 */

#ifndef ALOGPARSE_H_
#define ALOGPARSE_H_


#include <string>

class ALogEntry
{
public:
  ALogEntry() {m_timestamp=0; m_dval=0; m_isnum=false;};
  ~ALogEntry() {};

  void set(double timestamp, const std::string& varname,
	   const std::string& source,
	   const std::string& srcaux,
	   const std::string& sval)
  {
    m_timestamp = timestamp;
    m_varname   = varname;
    m_source    = source;
    m_srcaux    = srcaux;
    m_sval      = sval;
    m_dval      = 0;
    m_isnum     = false;
  };

  void set(double timestamp, const std::string& varname,
	   const std::string& source,
	   const std::string& srcaux,
	   double dval)
  {
    m_timestamp = timestamp;
    m_varname   = varname;
    m_source    = source;
    m_srcaux    = srcaux;
    m_sval      = "";
    m_dval      = dval;
    m_isnum     = true;
  };

  void setStatus(const std::string& s) {m_status=s;};

  double      time() const         {return(m_timestamp);};
  double      getTimeStamp() const {return(m_timestamp);};
  std::string getVarName() const   {return(m_varname);};
  std::string getSource() const    {return(m_source);};
  std::string getSrcAux() const    {return(m_srcaux);};
  std::string getStringVal() const {return(m_sval);};
  double      getDoubleVal() const {return(m_dval);};
  bool        isNumerical() const  {return(m_isnum);};
  std::string getStatus() const    {return(m_status);};

  bool        isNull() const       {return(m_status=="null");};

  void        skewBackward(double v) {m_timestamp -= v;};
  void        skewForward(double v)  {m_timestamp += v;};

protected:
  double      m_timestamp;
  std::string m_varname;
  std::string m_source;
  std::string m_srcaux;
  std::string m_sval;
  double      m_dval;
  bool        m_isnum;

  // An optional status string. The empty string indicates the entry
  // is a normal entry. "invalid" means the entry is not normal. "eof"
  // could indicate that a the entry is the tail of normal entries.
  std::string  m_status;
};



#endif /* ALOGPARSE_H_ */
