/* Copyright (c) 2015, EPFL/Blue Brain Project                                   
 *                                                                               
 * This file is part of eFEL <https://github.com/BlueBrain/eFEL>                 
 *                                                                               
 * This library is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU Lesser General Public License version 3.0 as published   
 * by the Free Software Foundation.                                              
 *                                                                               
 * This library is distributed in the hope that it will be useful, but WITHOUT   
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more 
 * details.                                                                      
 *                                                                               
 * You should have received a copy of the GNU Lesser General Public License      
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                   
 */      


#ifndef MAPOPERATIONS_H
#define MAPOPERATIONS_H

#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

/* reduce boiler plate for checking if there is cached data
 */
#define CHECK_DOUBLE_CACHE(name)               \
    do {                                       \
      int retVal, nSize;                       \
      retVal = CheckInDoublemap(IntDoubleData, \
                             StringData,       \
                             string(#name),    \
                             nSize);           \
      if (retVal){                             \
        return nSize;                          \
      }                                        \
    }while(0);

#define CHECK_INT_CACHE(name)                \
    do {                                     \
      int retVal, nSize;                     \
      retVal = CheckInIntmap(IntFeatureData, \
                             StringData,     \
                             string(#name),  \
                             nSize);         \
      if (retVal){                           \
        return nSize;                        \
      }                                      \
    }while(0);

/* Simplify boiler-plate of defining a double/int vector, and getting
 * the data vector associated with a feature name
 */
#define GETDOUBLE(name, dest)                 \
    vector<double> dest;                      \
    retVal = getDoubleVec(DoubleFeatureData,  \
                          StringData,         \
                          string(#name),      \
                          dest);

#define GETDOUBLE_RET(name, dest, minimum)          \
    GETDOUBLE(name, dest)                           \
    if ( retVal < minimum) {                        \
      GErrorStr += (string(__FUNCTION__) +          \
                    ": Can't find vector " # name); \
      return -1;                                    \
    }

/*
#define GETINT(name, dest)                    \
    vector<int> dest;                         \
    retVal = getIntVec(IntFeatureData,        \
                          StringData,         \
                          string(#name),      \
                          dest);

#define GETINT_RET(name, dest, minimum)                        \
    GETDOUBLE(name, dest)                                      \
    if ( retVal < minimum) {                                   \
      GErrorStr += __function__ ": Can't find vector " # name; \
      return -1;                                               \
    }
    */


typedef map<string, vector<int> > mapStr2intVec;
typedef map<string, vector<double> > mapStr2doubleVec;
typedef map<string, string> mapStr2Str;

extern string GErrorStr;

int getIntParam(mapStr2intVec& IntFeatureData, const string& param,
                vector<int>& vec);
int getDoubleParam(mapStr2doubleVec& DoubleFeatureData, const string& param,
                   vector<double>& vec);
int getStrParam(mapStr2Str& StringData, const string& param, string& value);

void setIntVec(mapStr2intVec& IntFeatureData, mapStr2Str& StringData,
               string key, const vector<int>& value);
void setDoubleVec(mapStr2doubleVec& DoubleFeatureData, mapStr2Str& StringData,
                  string key, const vector<double>& value);

int getDoubleVec(mapStr2doubleVec& DoubleFeatureData, mapStr2Str& StringData,
                 string strFeature, vector<double>& v);
int getIntVec(mapStr2intVec& IntFeatureData, mapStr2Str& StringData,
              string strFeature, vector<int>& v);
int CheckInDoublemap(mapStr2doubleVec& DoubleFeatureData,
                     mapStr2Str& StringData, string strFeature, int& nSize);
int CheckInIntmap(mapStr2intVec& IntFeatureData, mapStr2Str& StringData,
                  string strFeature, int& nSize);

// eCode feature convenience function
int mean_traces_double(mapStr2doubleVec& DoubleFeatureData,
                       const string& feature, const string& stimulus_name,
                       int i_elem, vector<double>& mean);
int std_traces_double(mapStr2doubleVec& DoubleFeatureData,
                      const string& feature, const string& stimulus_name,
                      double mean, int i_elem, vector<double>& std);
void getTraces(mapStr2doubleVec& DoubleFeatureData, const string& wildcard,
               vector<string>& traces);

#endif
