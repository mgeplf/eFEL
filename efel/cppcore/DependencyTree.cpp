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
#include <cstdio>
#include <istream>
#include <fstream>
#include <list>
#include <utility>

using std::list;

inline string deblank(const string &str) {
  size_t startpos = str.find_first_not_of(" \t");
  size_t endpos = str.find_last_not_of(" \t");
  if (string::npos == startpos || string::npos == endpos)
    return string();
  return str.substr(startpos, endpos - startpos + 1);
}

static inline string getVersion(const string str)
{
  size_t version_end = str.find(":");
  return string::npos == version_end ? string() : 
      str.substr(0, version_end);
}

static inline string getFeature(const string str)
{
  size_t version_end = str.find(":");
  size_t wildcard_start = str.find(";");

  return str.substr(version_end + 1, wildcard_start - version_end - 1);
}

static inline string getWildcards(const string str)
{
  size_t wildcard_start = str.find(";");
  return string::npos == wildcard_start ? string() : 
      str.substr(wildcard_start + 1);
}

// return all the features in the first column of the dependency file
vector<string> getAllParents(const vector<string> &strDependencyFile) {
  vector<string> lstFeature;
  for (size_t i = 0; i < strDependencyFile.size(); i++) {
    string strLine = deblank(strDependencyFile[i]);
    size_t nPos = strLine.find_first_of('#');
    string FeatureName = deblank(strLine.substr(0, nPos - 1));
    if (!FeatureName.empty()) lstFeature.push_back(FeatureName);
  }
  return lstFeature;
}

list<string> getChilds(string feature, const vector<string> &strDependencyFile)
{
  list<string> children;
  for (size_t i = 0; i < strDependencyFile.size(); i++) {
    string strLine = deblank(strDependencyFile[i]);
    size_t nPos = strLine.find_first_of('#');
    string FeatureName = deblank(strLine.substr(0, nPos - 1));

    if (FeatureName != feature)
      continue;

    while (nPos != string::npos) {
      strLine = strLine.substr(nPos + 1);
      nPos = strLine.find_first_of('#');
      string child = deblank(nPos > 0 ? strLine.substr(0, nPos - 1) : strLine);
      children.push_back(child);
    }
  }
  return children;
}

vector<string> parseStream(std::istream &cin)
{
  vector<string> strDependencyFile;
  string line;
  do {
    std::getline(cin, line);
    strDependencyFile.push_back(line);
  } while (!cin.eof());
  return strDependencyFile;
}


cTree::cTree(const char *strFileName) {
  std::ifstream cin(strFileName);
  if (cin.is_open()) {
    strDependencyFile = parseStream(cin);
  } else {
    ErrorStr += "\nCould not open the file " + string(strFileName);
  }
}

//  Fill FinalList with a list of all the feature matching the wildcards
int getDependency(string strLine, 
                  string wildcards,
                  const vector<string> &strDependencyFile,
                  list<string> &FinalList
                 ) {
  // parse wildcards out of "LibVx:feature_name;wildcards_name"
  int wcpos = strLine.find(";");
  if (wcpos >= 0) {
    wildcards = strLine.substr(wcpos);
    strLine = deblank(strLine.substr(0, wcpos));
  }

  list<string> tmpChild = getChilds(strLine, strDependencyFile);

  while(!tmpChild.empty()){
    string str = tmpChild.front();
    tmpChild.pop_front();
    getDependency(str, wildcards, strDependencyFile, FinalList);
  }

  string fullFeature = strLine + wildcards;
  if (std::find(FinalList.begin(), FinalList.end(), fullFeature) == FinalList.end()) {
    FinalList.push_back(fullFeature);
  }

  return 0;
}

int cTree::getDependencyList(string) {
  for (unsigned i = 0; i < strDependencyFile.size(); i++) {
  }
  return 1;
}

int cTree::printTree() {
  printf("\n This is inside printTree ");
  return 1;
}

/**
 * setFeaturePointers
 *
 * mapFptrLib :
 * FptrLookup | vector of pairs:
 *                  first | string: feature name
 *                  second | vector of featureStringPair
 */
int cTree::setFeaturePointers(map<string, feature2function *> &mapFptrLib,
                              map<string, vector<featureStringPair > > *FptrLookup) 
{
  vector<string> vecFeature = getAllParents(strDependencyFile);
  if (vecFeature.size() == 0) return -1;

  string feature; 
  for (size_t i = 0; i < vecFeature.size(); i++) {
    string strLibFeature = vecFeature[i];

    list<string> FinalList;
    // fill FinalList with all the dependencies of feature vecFeature[i]
    getDependency(strLibFeature, "", strDependencyFile, FinalList);

    vector<featureStringPair> vecfptr;

    list<string>::iterator lstItr;
    for (lstItr = FinalList.begin(); lstItr != FinalList.end(); lstItr++) {
      // Here strLibFeature is the feature name of the dependent feature
      string &dependency = *lstItr;
      feature = getFeature(dependency);
      string version = getVersion(*lstItr);
      string wildcards = getWildcards(*lstItr);

      if (version.empty()) {
        ErrorStr += "\nLibrary version is missing in [" + *lstItr +
            "] expected format Lib:Feature";
        return -1;
      }

      // Find the feature function pointer map for the library
      map<string, feature2function *>::iterator mapLibItr
          = mapFptrLib.find(version);
      if (mapLibItr == mapFptrLib.end()) {
        ErrorStr += "\nLibrary [" + version + "] is missing\n";
        return -1;
      }

      // Find the function pointer of the feature in the library
      feature2function *fptrTbl = mapLibItr->second;
      feature2function::iterator mapFeatureItr = fptrTbl->find(feature);
      if (mapFeatureItr == fptrTbl->end()) {
        ErrorStr += ("\nFeature [" + feature + "] is missing from Library [" + 
                     version + "]");
        return -1;
      }

      // Add the feature function pointer and wildcards to the list of dependent
      // features
      vecfptr.push_back(featureStringPair(mapFeatureItr->second, wildcards));
    }
    // Add the vecfptr from above to a map with as key the base featurei
    FptrLookup->insert(
        std::pair<string, vector<featureStringPair> >(feature, vecfptr));
  }

  return 1;
}
