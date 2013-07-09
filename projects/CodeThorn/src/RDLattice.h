#include "Labeler.h"
#include "VariableIdMapping.h"
#include "HSet.h"

#ifndef RD_LATTICE_H
#define RD_LATTICE_H

using CodeThorn::Label;
using CodeThorn::VariableId;
using CodeThorn::VariableIdMapping;

class RDLattice {
 public:
  typedef set<pair<Label,VariableId> >::iterator iterator;
  RDLattice();
  iterator begin();
  iterator end();
  size_t size();
  void insertPair(Label,VariableId);
  void erasePair(Label,VariableId);
  void eraseAllPairsWithVariableId(VariableId var);
  bool isBot();
  void setBot();
  void toStream(ostream& os, VariableIdMapping* vim=0);
  bool exists(pair<Label,VariableId>);
  void setEmptySet() {_bot=false;rdSet.clear();}
#if 1
  void combine(RDLattice& b);
  bool approximatedBy(RDLattice& b);
#endif
 private:
  set<pair<Label,VariableId> > rdSet;
  bool _bot;
};

#endif

