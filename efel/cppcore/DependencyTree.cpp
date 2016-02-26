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

#include "DependencyTree.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <sstream>
#include <utility>


using std::map;
using std::string;

// LibVx:feature_name;wildcards_name
const char VERSION_END_DELIM = ':';
const char DEPENDENCY_START_DELIM = '#';
const char WILDCARD_START_DELIM = ';';

static inline string getVersion(const string str)
{
  size_t end = str.find(VERSION_END_DELIM);
  return string::npos == end ? string() : str.substr(0, end);
}

static inline string getFeature(const string str)
{
  size_t version_end = str.find(VERSION_END_DELIM);
  size_t wildcard_start = str.find(WILDCARD_START_DELIM);

  return str.substr(version_end + 1, wildcard_start - version_end - 1);
}

static inline string getWildcards(const string str)
{
  size_t wildcard_start = str.find(WILDCARD_START_DELIM);
  return string::npos == wildcard_start ? string() :
      str.substr(wildcard_start + 1);
}

DependencyMap parseStream(std::istream &cin)
{
  DependencyMap ret;

  string line;
  do {
    std::getline(cin, line);

    string feature;
    std::stringstream ss(line);
    ss >> feature;

    if(feature.empty())
      continue;

    ret[feature] = FeatureNames();
    string dep;
    while(!ss.eof()){
      ss >> dep;
      if(DEPENDENCY_START_DELIM != dep[0])
        continue;
      ret[feature].push_back(dep.substr(1));
    }
  } while (!cin.eof());
  return ret;
}

cTree::cTree(const char *strFileName) {
  std::ifstream cin(strFileName);
  if (cin.is_open()) {
    dependencies = parseStream(cin);
  } else {
    ErrorStr += "Could not open the file " + string(strFileName) + '\n';
  }
}

//  Fill deps with all the names of the dependencies
//  NOTE: the returned deps must be in the order of evaluation: A depends on B,
//  then B must be before A
static int getDependency(const string fullFeature,
                         const DependencyMap &dependencies,
                         FeatureNames &deps)
{
  string feature = fullFeature;
  size_t wcpos = feature.find(WILDCARD_START_DELIM);
  if (wcpos >= 0) {
    feature = feature.substr(0, wcpos);
  }

  DependencyMap::const_iterator it = dependencies.find(feature);
  if(it != dependencies.end()){
    for(FeatureNames::const_iterator dep = it->second.begin();
        dep != it->second.end(); ++dep){
      getDependency(*dep, dependencies, deps);
    }
  }

  if (std::find(deps.begin(), deps.end(), fullFeature) == deps.end()) {
    deps.push_back(fullFeature);
  }

  return 0;
}

string cTree::listDependencies()
{
  std::stringstream ss;
  std::ostream_iterator<string> outIt(ss, ", ");

  for(DependencyMap::const_iterator dep = dependencies.begin();
      dep != dependencies.end(); ++dep){
    ss << "Feature: " << dep->first << '\n';
    ss << "\tDependencies: " << dep->first;
    std::copy(dep->second.begin(), dep->second.end(), outIt);
    ss << '\n';
  }
  return ss.str();
}

/**
 * setFeaturePointers
 *
 * mapFptrLib :
 * FptrLookup | vector of pairs:
 *                  first | string: feature name
 *                  second | vector of featureStringPair
 *
 * Note: FptrLookup must have its vectorlist be ordered such that each
 *       of the dependent variables comes after its dependencies: in other
 *       words, the vector functions are executed in order, so earlier
 *       results must be available for later calculations
 */
int cTree::setFeaturePointers(const map<string, feature2function *> &mapFptrLib,
                              map<string, std::vector<featureStringPair > > &FptrLookup)
{
  if(dependencies.empty())
    return -1;

  for(DependencyMap::const_iterator it = dependencies.begin();
      it != dependencies.end(); ++it){
    const string &strLibFeature = it->first;

    FeatureNames deps;
    getDependency(strLibFeature, dependencies, deps);

    std::vector<featureStringPair> &vecfptr = FptrLookup[getFeature(strLibFeature)];

    for (FeatureNames::const_iterator depIt = deps.begin();
         depIt != deps.end(); ++depIt) {
      const string &dependency = *depIt;

      string version = getVersion(dependency);
      if (version.empty()) {
        ErrorStr += "Library version is missing in [" + dependency +
            "] expected format Lib:Feature\n";
        return -1;
      }

      // Find the feature function pointer map for the library
      map<string, feature2function *>::const_iterator mapLibItr
          = mapFptrLib.find(version);
      if (mapLibItr == mapFptrLib.end()) {
        ErrorStr += "Library [" + version + "] is missing\n";
        return -1;
      }

      // Find the function pointer of the feature in the library
      string dep_feature = getFeature(dependency);
      feature2function *fptrTbl = mapLibItr->second;
      feature2function::iterator mapFeatureItr = fptrTbl->find(dep_feature);
      if (mapFeatureItr == fptrTbl->end()) {
        ErrorStr += "Feature [" + dep_feature + "] is missing from Library [" + 
            version + "]\n";
        return -1;
      }

      string wildcards = getWildcards(dependency);
      // Add the feature function pointer and wildcards to the list of dependent
      // features
      vecfptr.push_back(featureStringPair(mapFeatureItr->second, wildcards));
    }
  }

  return 1;
}
