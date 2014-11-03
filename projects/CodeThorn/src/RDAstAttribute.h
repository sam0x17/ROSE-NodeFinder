// Author: Markus Schordan, 2013.

#ifndef RDANALYSISASTATTRIBUTE_H
#define RDANALYSISASTATTRIBUTE_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "RDAstAttributeInterface.h"
#include "RDLattice.h"
#include <string>

using std::string;

class RDAstAttribute : public RDAstAttributeInterface {
 public:
  virtual bool isBottomElement();
  virtual VariableIdSet allVariableIds();
  virtual LabelSet allLabels();
  virtual LabelSet definitionsOfVariableId(VariableId varId);
  virtual VariableIdSet variableIdsOfDefinition(Label def);
  virtual iterator begin();
  virtual iterator end();
  virtual ~RDAstAttribute();
 public:
  RDAstAttribute(RDLattice* elem);
  void toStream(ostream& os, VariableIdMapping* vim);
  string toString();
 private:
  RDLattice* _elem;
};

#endif
