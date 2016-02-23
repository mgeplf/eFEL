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

#include "types.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

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


class FeatureAccessor
{
public:
  FeatureAccessor(mapStr2intVec& IntFeatureData, 
                  mapStr2doubleVec& DoubleFeatureData,
                  mapStr2Str& StringData)
      : IntFeatureData(IntFeatureData) 
      , DoubleFeatureData(DoubleFeatureData)
      , StringData(StringData)
      {}

  inline bool getFeature(const string &name, vector<double> &dest)
  {
    return 0 <= getDoubleVec(DoubleFeatureData, StringData, name, dest);
  }

  inline bool getFeature(string name, vector<int> &dest)
  {
    return 0 <= getIntVec(IntFeatureData, StringData, name, dest);
  }

  template<typename T0, typename T1>
  inline bool getFeatures(string name0, vector<T0> &dest0, 
                         string name1, vector<T1> &dest1)
  {
    bool retVal;
    retVal = getFeature(name0, dest0);
    if (!retVal) return retVal;
    retVal = getFeature(name1, dest1);
    return retVal;
  }

  template<typename T0, typename T1, typename T2>
  inline int getFeatures(string name0, vector<T0> &dest0, 
                         string name1, vector<T1> &dest1,
                         string name2, vector<T2> &dest2)
  {
    bool retVal;
    retVal = getFeatures(name0, dest0,
                         name1, dest1);
    if (!retVal) return retVal;
    retVal = getFeature(name2, dest2);
    return retVal;
  }

  template<typename T0, typename T1, typename T2, typename T3>
  inline int getFeatures(string name0, vector<T0> &dest0, 
                         string name1, vector<T1> &dest1,
                         string name2, vector<T2> &dest2,
                         string name3, vector<T3> &dest3)
  {
    bool retVal;
    retVal = getFeatures(name0, dest0,
                         name1, dest1,
                         name2, dest2);
    if (!retVal) return retVal;
    retVal = getFeature(name3, dest3);
    return retVal;
  }

  template<typename T0, typename T1, typename T2, typename T3, typename T4>
  inline int getFeatures(string name0, vector<T0> &dest0, 
                         string name1, vector<T1> &dest1,
                         string name2, vector<T2> &dest2,
                         string name3, vector<T3> &dest3,
                         string name4, vector<T4> &dest4)
  {
    bool retVal;
    retVal = getFeatures(name0, dest0, 
                         name1, dest1, 
                         name2, dest2,
                         name3, dest3);
    if (!retVal) return retVal;
    retVal = getFeature(name4, dest4);
    return retVal;
  }

private:
  mapStr2intVec& IntFeatureData;
  mapStr2doubleVec& DoubleFeatureData;
  mapStr2Str& StringData;
};


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
