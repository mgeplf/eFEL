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

#ifndef __DEPENDENCYTREE_H
#define __DEPENDENCYTREE_H

#include "types.h"

#include <map>
#include <string>
#include <vector>

//
typedef std::vector<std::string> FeatureNames;

//map from Feature (ex: LibV1:foo) to its dependencies: (ex: #LibV1:bar #LibV1:baz)
typedef std::map<std::string, FeatureNames > DependencyMap;

class cTree {
  DependencyMap dependencies;

 public:
  std::string ErrorStr;

  cTree(const char *strFileName);
  std::string listDependencies();
  int setFeaturePointers(const std::map<std::string, feature2function *> &mapFptrLib,
                         std::map<std::string, std::vector<featureStringPair> > &FptrLookup);
};

#endif
