/* POET : Parameterized Optimizations for Empirical Tuning
   Copyright (c)  2008,  Qing Yi.  All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
 3. Neither the name of UTSA nor the names of its contributors may be used to
    endorse or promote products derived from this software without specific
    prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISEDOF THE POSSIBILITY OF SUCH DAMAGE.
*/

//********************************************************************
//  Routines implementing dynamic top-down parsing from syntax specifications
//********************************************************************

#include <fstream>
#include <iostream>
#include <list>
#include <poet_ASTvisitor.h>
#include <poet_ASTeval.h>
#include <timing.h>
#include <assert.h>

extern bool debug_time();
extern bool debug_parse();
extern "C" POETCode* make_sourceString( const char* text, int len);

class ParseError : public Error {
  POETCode* r1, *r2;
  int lineno;
 public:
  ParseError(POETCode* _r1, POETCode *_r2, int _lineno) : r1(_r1), r2(_r2), lineno(_lineno) {}
  std::string message() const 
   { std:: stringstream msg; msg << "Parsing type mismatch at line " << lineno << ": " << SHORT(r1->toString(),100) << "\n=> " << r2->toString() << "\n";  
     return msg.str(); }
};
#define PARSE_MISMATCH(r1,r2,ln)  { throw ParseError(r1,r2,ln); }


POETCode* InvokeExpMacro(POETCode* op, POETCode* args)
{
  XformVar* xvar = dynamic_cast<XformVar*>(op);
  if (xvar != 0) return xvar->eval(args);
  CodeVar* fvar = dynamic_cast<CodeVar*>(op);
  if (fvar != 0) return CODE_REF(fvar,args);
  INCORRECT_CVAR(op);
}  


/***************** computation of lookahead info **************************/

//#define DEBUG_LOOKAHEAD
/*QY: compute lookahead info; return the min len of tokens in lookahead */
unsigned POETParseList::
compute_lookaheadInfo(std::vector<POETCode*>& res, 
                            unsigned need) 
{
     if (!itemFilter.size()) { 
        lookahead = EvaluatePOET::compute_lookaheadInfo(get_arg(0), itemFilter, need);
     }
     for (unsigned i = 0; i < itemFilter.size(); ++i)
         res.push_back(itemFilter[i]);
     return lookahead;
}

bool POETParseList:: match_lookahead(POETCode* input)
{
  if (!itemFilter.size())  {
     lookahead = EvaluatePOET::compute_lookaheadInfo(get_arg(0), itemFilter, 1);
  }
  for (unsigned i = 0; i < itemFilter.size(); ++i)
     if (EvaluatePOET::match_lookahead(input, itemFilter[i]))
          return true;
  return false; 
}

unsigned POETTypeTor::compute_lookaheadInfo(unsigned index, unsigned need)
{
     for (unsigned i = parseInfo.size(); i <= index; ++i) 
         { parseInfo.push_back(ParseInfo()); }
     ParseInfo& cur = parseInfo[index];
     if (!cur.filter.size()) {
        cur.lookahead = EvaluatePOET::compute_lookaheadInfo(args[index], cur.filter, need);
#ifdef DEBUG_LOOKAHEAD
std::cerr << "lookahead for: " << toString() << ":" << index << ":";
        for (int i = 0; i < cur.filter.size(); ++i) 
std::cerr << cur.filter[i]->toString() << ";";
        std::cerr << "\n";
#endif
     }
     return cur.lookahead;
}

unsigned POETTypeTor::
compute_lookaheadInfo(std::vector<POETCode*>& res, unsigned need)
{
    int size = args.size();
    unsigned len = -1;
    for (int i = 0; i < size; ++i)
     {
        unsigned curlen = compute_lookaheadInfo(i,need);
        if (len > curlen) len = curlen;
        ParseInfo& cur = parseInfo[i];
        for (unsigned i = 0; i < cur.filter.size(); ++i) {
            res.push_back(cur.filter[i]);
        }
    }
    return len;
}

POETCode* POETTypeTor::get_parseInfo(POETCode* r1)
{ /*QY: compute and set parsing information*/
    unsigned size = args.size();
    if (!size) return 0;
    for (unsigned i = 0; i < size-1; ++i)
     {
        compute_lookaheadInfo(i, 1);
        ParseInfo& cur = parseInfo[i];
        if (cur.lookahead == 0) LOOKAHEAD_EMPTY(args[i]);  
        assert(cur.filter.size() > 0);
        for (unsigned j = 0; j < cur.filter.size(); ++j) {
            POETCode* cur_filter = cur.filter[j];
            if (EvaluatePOET::match_lookahead(r1, cur_filter))
               return args[i];
        }
     }
     return args[size-1];
}


unsigned EvaluatePOET:: compute_exp_lookahead(std::vector<POETCode*>& res)
{
   res.push_back(fac->new_string("("));
   if (exp_item->get_entry().get_code() == 0)
      SYM_UNDEFINED("EXP_BASE");
   compute_lookaheadInfo(exp_item->get_entry().get_code(), res);
   POETCode* uop = exp_uop->get_entry().get_code();
   while (uop != 0) {
       res.push_back(get_head(uop));
       uop = get_tail(uop);
   }
   return 1;
}

/*QY: return the min length of tokens placed in the filter res*/
unsigned EvaluatePOET::
compute_lookaheadInfo(POETCode* cur, std::vector<POETCode*>& res, unsigned need)
{
 try {
    assert(cur != 0);
    switch (cur->get_enum()) {
     case SRC_TYPE: 
         if (cur != ANY) { res.push_back(cur); return 1; }
         return 0;
     case SRC_STRING: 
         if (cur != EMPTY) { res.push_back(cur); return 1; }
         return 0;
     case SRC_ICONST:  
         res.push_back(cur); res.push_back(AST2String(cur)); 
         return 1;
     case SRC_CVAR:  {
         CodeVar* cvar = static_cast<CodeVar*>(cur);
         POETCode* parse=cvar->get_parseInfo();
         unsigned lookahead = cvar->get_entry().get_lookahead();
         if (need < lookahead) need = lookahead;
         if (parse != EMPTY) {
            unsigned len = compute_lookaheadInfo(parse, res, need);
            if (lookahead > 1 && len < lookahead) 
               { LOOKAHEAD_AMBIGUOUS(cvar); return 0; }
            return len;
         }
         res.push_back(cvar);  /*QY: the code var is a token */
         return 1;
        }
     case SRC_LVAR: {
          LocalVar* lvar = static_cast<LocalVar*>(cur);
          POETCode* restr = 0;
          switch (lvar->get_entry().get_entry_type())
          {
            case LVAR_CODEPAR: case LVAR_ATTR:
                 restr = lvar->get_entry().get_restr(); break;
            default: restr = lvar->get_entry().get_code(); 
                 break;
          }
          if (restr == 0) { LOOKAHEAD_AMBIGUOUS(cur); return 0; }
          else return compute_lookaheadInfo(restr,res,need); 
        }
     case SRC_LIST: {
         POETList* curlist = static_cast<POETList*>(cur);
         int curstart = res.size();
         unsigned len = compute_lookaheadInfo(curlist->get_first(),res,need);
         cur = curlist->get_rest();
         if (cur == 0) return len;
         int cursize = res.size();
         if (need == 1) {
            if (len == 0) return compute_lookaheadInfo(cur, res, need);
            return len;
         }
         if (len < need ) {
            std::vector<POETCode*> tailres;
            unsigned len2 = compute_lookaheadInfo(cur, tailres, need-len);
            ASTFactory* fac = ASTFactory::inst();
            for (int i = curstart; i < cursize; ++i) {
               POETCode* first = res[i]; 
               res[i] = fac->append_tuple(first,tailres[0]);
               for (unsigned j = 1; j < tailres.size(); ++j) 
                   res.push_back(fac->append_tuple(first,tailres[j]));
            }
            return len+len2;
         }
         return len;
        }
     case SRC_OP : {
        POETOperator* op = static_cast<POETOperator*>(cur);
        switch(op->get_op()) {
        case TYPE_TOR: {
            POETTypeTor* tor = static_cast<POETTypeTor*>(cur);
            return tor->compute_lookaheadInfo(res, need);
           }
        case POET_OP_LIST1: 
        case TYPE_LIST1: 
            return compute_lookaheadInfo(op->get_arg(0),res,need);
        case POET_OP_LIST: 
        case TYPE_LIST: 
            compute_lookaheadInfo(op->get_arg(0),res); return 0;
        case POET_OP_EXP:
            return compute_exp_lookahead(res);
        default:
           LOOKAHEAD_AMBIGUOUS(cur); return 0;
        }
       }
     default:
        LOOKAHEAD_AMBIGUOUS(cur); return 0;
     }
   }
   catch (Error err) { std::cerr << " From computing filter information for " << cur->toString() << "\n"; throw err; }
}

bool EvaluatePOET::match_lookahead(POETCode* r1, POETCode* cur_filter)
{
#ifdef DEBUG_LOOKAHEAD
std::cerr << "trying to match " << r1->toString() << " with lookahead:" << cur_filter->toString() << "\n";
#endif
    POETCode* r1_first = get_head(r1);
    switch (cur_filter->get_enum()) {
     case SRC_STRING: 
     case SRC_ICONST: return (cur_filter == r1_first);
     case SRC_CVAR: /* a code template token */
          return (r1_first->get_enum() == SRC_CVAR &&
              static_cast<CodeVar*>(r1_first)->get_entry() == 
              static_cast<CodeVar*>(cur_filter)->get_entry());
     case SRC_TYPE:
          return (match_Type(r1_first, static_cast<POETType*>(cur_filter), 
                               MATCH_AST_PATTERN)); 
     case SRC_TUPLE: /*QY: multiple lookahead tokens*/
        {
         POETTuple* tuple = static_cast<POETTuple*>(cur_filter);
         for (unsigned i = 0; i < tuple->size(); ++i) {
              if (!match_lookahead(r1_first, tuple->get_entry(i))) return false;
              r1_first = NextToken(r1);
         }
         return true; 
        }  
     case SRC_OP: {
        POETParseList* parse = dynamic_cast<POETParseList*>(cur_filter);
        if (parse != 0) return parse->match_lookahead(r1);
     }
     default: std::cerr << "Unexpected: " << cur_filter->toString() << "\n"; assert(0);
    }
    return false;
  }

class ParseExp : public EvaluatePOET 
{
   std::vector<POETCode*> itemFilter;
   bool backtrack; 

   static ParseExp* my;

   bool match_expItem(POETCode* input)
   { 
       for (unsigned i = 0; i < itemFilter.size(); ++i) {
           if (match_lookahead(input, itemFilter[i])) return true;
       }
       return false;
   }

   POETCode* MatchOp(POETCode* curop, POETCode* input)
     {
        for (; curop != 0; (curop=get_tail(curop),input=NextToken(input)))
               { 
                 if (get_head(curop) != get_head(input)) break; 
               }
        if (curop == 0) return (input==0)? EMPTY : input;
        return 0;
     }
   ParseExp() {
      backtrack=curfile->get_backtrack();
      compute_exp_lookahead(itemFilter);
      if (exp_bop->get_entry().get_code() == 0)
         SYM_UNDEFINED("EXP_BOP");
   }
 public:
   static void reset() { if (my != 0) delete my; 
                    my = 0; }
   static ParseExp* inst() { 
            if (my == 0) my = new ParseExp();
            return my; }
   typedef std::pair<POETCode*, POETCode*> Result;
   Result ParseItemType(POETCode* input, int *p_lineno = 0);
   Result ParseExpImpl(POETCode* input, POETCode* bop, POETCode* inherit, int *p_lineno=0);
   Result parse(POETCode* input, int *p_lineno = 0) 
    { 
     POETCode* bop = exp_bop->get_entry().get_code();
     return ParseExpImpl(SkipEmpty(input, p_lineno), bop, 0, p_lineno); 
    }

};

ParseExp* ParseExp::my = 0;

ParseExp::Result ParseExp::ParseItemType(POETCode* input, int *p_lineno)
   {
      POETCode* res = 0, *tail=0;
      if (match_expItem(input)) {
          try {
           res = EvaluatePOET::parse_AST(input, exp_item->get_entry().get_code(), &tail);
           assert(res != 0);
           input=SkipEmpty(tail, p_lineno);
          }
          catch (ParseError err) { 
               if (get_head(input) == lp) { /* QY: treat lp the default way */
                  Result resOfRest = ParseExpImpl(NextToken(input),exp_bop->get_entry().get_code(),0, p_lineno);
                  if (get_head(resOfRest.second) == rp) {
                        input = NextToken(resOfRest.second);
                        res = resOfRest.first;
                  }
                  else {
                      return Result(0,input);
                   }
               }
               else if (backtrack) return Result(0,input); 
               else throw err;
          }
      }
      else if ( get_head(input) == lp) {
           Result resOfRest = ParseExpImpl(NextToken(input),exp_bop->get_entry().get_code(),0, p_lineno);
           if (get_head(resOfRest.second) == rp) {
                 input = NextToken(resOfRest.second);
                 res = resOfRest.first;
           }
           else return Result(0,input);
      }
      else return Result(0,input);
     if (funcall->get_entry().get_code() != 0 && get_head(input) == lp)
     {
        Result resOfTail = ParseExpImpl(NextToken(input),exp_bop->get_entry().get_code(),0, p_lineno);
        std::vector<POETCode*> argVec; 
        while (resOfTail.first!=0 && (get_head(resOfTail.second) != rp)) {
              argVec.push_back(resOfTail.first);
              resOfTail=ParseExpImpl(NextToken(resOfTail.second),exp_bop->get_entry().get_code(),0, p_lineno);
        }
        argVec.push_back(resOfTail.first);
        POETCode* args = Vector2List(argVec);
        if (args == 0) args = EMPTY;
        return Result(InvokeExpMacro(funcall->get_entry().get_code(),
                                     PAIR(res,args)),
                          NextToken(resOfTail.second));
     }
     else if (arrref->get_entry().get_code() != 0 && get_head(input) == lb )
     {
        std::vector<POETCode*> argVec; 
        while (get_head(input) == lb) 
        {
          Result resOfTail = ParseExpImpl(NextToken(input),exp_bop->get_entry().get_code(),0, p_lineno);
          if (resOfTail.first==0 || get_head(resOfTail.second) != rb) 
              return Result(res,input);
          argVec.push_back(resOfTail.first);
          input = NextToken(resOfTail.second);
        }
        POETCode* args = Vector2List(argVec);
        if (args == 0) args = EMPTY;
        CodeVar* fvar = dynamic_cast<CodeVar*>(arrref->get_entry().get_code());
        return Result(InvokeExpMacro(arrref->get_entry().get_code(),
                                     PAIR(res,args)),input);
     }
     return Result(res, input);
  }

ParseExp::Result 
ParseExp::ParseExpImpl(POETCode* input, POETCode* bop, POETCode* inherit, int *p_lineno)
{
  if (inherit == 0) {
    POETCode* p_uop = exp_uop->get_entry().get_code();
    for (POETCode* cur = 0; ((cur=get_head(p_uop))!=0); p_uop = get_tail(p_uop)) 
    {
       POETCode* p_input = MatchOp(cur, input);
       if (p_input != 0) {
          Result resOfRest = ParseItemType(p_input, p_lineno);
          if (resOfRest.first != 0) {
             if (buildUop->get_entry().get_code()!=0) {
                XformVar* xvar = dynamic_cast<XformVar*>(buildUop->get_entry().get_code());
                if (xvar == 0) INCORRECT_XVAR(buildUop);
                inherit = xvar->eval(PAIR(cur,resOfRest.first),false);
             }
             else {
                CodeVar* cvar = dynamic_cast<CodeVar*>(parseUop->get_entry().get_code());
                if (cvar == 0) INCORRECT_CVAR(parseUop);
                inherit = CODE_REF(cvar, PAIR(cur,resOfRest.first));
             }
             input = resOfRest.second;
          }
          break;
       }
    }
  }
  if (inherit == 0)  {
     Result res = ParseItemType(input, p_lineno);
     inherit = res.first; input = res.second;
  }
  if (inherit == 0) { return Result(0,input); }
  if (get_tail(input) != 0) {
     POETCode* p_bop = bop;
     for (POETCode* cur_bop=0; (cur_bop = get_head(p_bop)) != 0; p_bop = get_tail(p_bop) )
     {
        for (POETCode* cur = 0; (cur = get_head(cur_bop)) != 0; cur_bop = get_tail(cur_bop)) 
        {
           POETCode* p_input = MatchOp(cur, input);
           if (p_input != 0) { 
              Result resOfTail = ParseExpImpl(p_input,get_tail(p_bop),0, p_lineno);
              if (resOfTail.first != 0) {
                 POETCode* first1 = 0;
                 if (buildBop->get_entry().get_code()!=0) {
                    XformVar* xvar = dynamic_cast<XformVar*>(buildBop->get_entry().get_code());
                    if (xvar == 0) INCORRECT_XVAR(buildBop);
                    first1 = xvar->eval(TUPLE3(cur,inherit,resOfTail.first),false);
                 } 
                 else {
                    CodeVar* cvar = dynamic_cast<CodeVar*>(parseBop->get_entry().get_code());
                    if (cvar == 0) INCORRECT_XVAR(parseBop);
                    first1=CODE_REF(cvar,TUPLE3(cur,inherit,resOfTail.first));
                 }
                 return ParseExpImpl(resOfTail.second,bop,first1, p_lineno);
              }
              return Result(inherit,input);
           }
        }
     }
   }
   return Result(inherit, input) ;
}

inline POETCode* SubList(POETCode* l1, POETCode* stop, POETCode*& rest) {
  if (l1 == stop) { rest = l1; return EMPTY; }
  POETList* ll1 = dynamic_cast<POETList*>(l1);
  if (ll1 != 0) {
     if (ll1->get_first() == stop) { rest = ll1; return EMPTY; } 
     POETCode* ll1Rest = ll1->get_rest();
     if (ll1Rest == stop || ll1Rest == 0) 
         { rest = ll1Rest; return ll1->get_first(); }
     POETCode* result_rest = SubList(ll1Rest, stop, rest);
     if (result_rest==EMPTY) result_rest = 0;
     if (result_rest == 0 || result_rest->get_enum() == SRC_LIST)
        return ASTFactory::inst()->new_list(ll1->get_first(), result_rest);
     return ASTFactory::inst()->new_list(ll1->get_first(), 
                ASTFactory::inst()->new_list(result_rest,0));
  }
  rest = EMPTY; return l1;
}

inline POETCode* MergeList(POETCode* l1, POETCode* l2) {
  POETList* ll2 = dynamic_cast<POETList*>(l2);
  if (ll2 == 0 && l2 != EMPTY) {
      ll2 = ASTFactory::inst()->new_list(l2, 0);
  }

  POETList* ll1 = dynamic_cast<POETList*>(l1);
  if (ll1 != 0) {
     POETCode* ll1First=ll1->get_first(), *ll1Rest = ll1->get_rest();
     if (ll1Rest == 0) 
         return ASTFactory::inst()->new_list(ll1First, ll2); 
     return ASTFactory::inst()->new_list(ll1First, MergeList(ll1Rest, ll2)); 
  }
  return ASTFactory::inst()->new_list(l1, ll2);
}

/* flatten lists for source matching */
class FlattenListVisitor : public ReplInfoVisitor
{
 public:
  virtual void visitList(POETList* s) {  
     POETCode* sFirst = s->get_first(), *sRest = s->get_rest();
     POETCode* s1 = apply(sFirst);
     POETCode* s2 = (sRest == 0)? EMPTY : apply(sRest);
     if (s1 != EMPTY && s2 != EMPTY) res = MergeList(s1, s2);
     else if (s1 == EMPTY) res = s2;
     else res = s1;
  }
};

/* Replace singleton lists with their single elements */
class ReplaceSingletonListVisitor : public ReplInfoVisitor
{
 public:
  virtual void visitList(POETList* s) {  
     if (s->get_rest() == 0) res = s->get_first();
  }
};

class ParseMatchVisitor  : public EvaluatePOET, public POETCodeVisitor
{
  POETCode* fullmatch;
  POETCode* res;
  POETCode* inherit; /*QY: the previous AST node */
  bool backtrack; 
  int lineno;

  POETCode* r1;
 
 public:
  POETCode* get_rest() { return r1; }
  int get_lineno() { return lineno; }
  ParseMatchVisitor(POETCode* _r1, bool _backtrack) 
    : fullmatch(0), res(0),inherit(0),backtrack(_backtrack), lineno(0)
   { 
     assert(_r1 != 0); 
     r1 = FlattenListVisitor().apply(_r1);
     r1 = SkipEmpty(r1, &lineno); 
   }
  POETCode* apply(POETCode* _r2, POETCode* _fullmatch, POETCode** ParseMatchLeftOver = 0)
  {
    if (r1 == 0)  r1 = EMPTY;
    POETCode* r1_head = get_head(r1);
    if (r1_head->get_enum() == SRC_OP && static_cast<POETUop*>(r1_head)->get_op() == POET_OP_ANNOT) {
       r1 = SkipEmpty(get_tail(r1), &lineno);
       POETCode* leftOver = 0;
       POETCode* r1_arg=static_cast<POETUop*>(r1_head)->get_arg();
       POETCode* nres = apply(r1_arg,EMPTY,&leftOver);
       if (leftOver == 0) r1 = nres;
       else r1 = fac->new_list(nres,leftOver); 
    }

    if (r1 == _r2)  { res = _r2;  r1 = 0; return res; }
    assert(r1 != 0 && _r2 != 0);

    POETCode* r1Save = r1;

    POETCode* matchsave = fullmatch;
    try { 
       fullmatch = _fullmatch;

       if (debug_parse() && _r2 != ANY) {
         std::cerr << "Trying to match " << _r2->get_className() << ":" << _r2->toString(DEBUG_VAR_RESTR) << ((fullmatch == 0)? " + NULL" : (fullmatch == EMPTY)? " + EMPTY" : (" + " + fullmatch->toString()) ) << " with " << r1->toString(DEBUG_OUTPUT_SHORT) << "\n";
       }

       res = 0; _r2->visit(this);
       if (res == 0) {
           std::cerr << "Failed to note failure of parsing " << _r2->toString() << "\n";
           assert(0);
       }
       if (r1 != 0)
          r1 = SkipEmpty(r1, &lineno);
       if (fullmatch != 0 && fullmatch != EMPTY) {
          POETCode* res_save = res;
          apply(fullmatch, EMPTY);
          res = res_save;
       }
       if (ParseMatchLeftOver != 0) {
           *ParseMatchLeftOver = (r1==EMPTY)? 0 : r1;
       }
    }
    catch (ParseError err) 
    {
       fullmatch = matchsave;
       throw err;
    }
    fullmatch = matchsave;
    if (debug_parse() && _r2 != ANY) {
        std::cerr << "Matching " << _r2->toString() << " with " << res->toString(DEBUG_OUTPUT_SHORT) << "\n";
    }
    if (r1Save != r1) 
       inherit=res;

    return res;
  }
 private:

  void parseTuple( POETCode* elemType) 
  {
     POETList* elemTypeList = dynamic_cast<POETList*>(elemType);
     if (elemTypeList==0) {
       std::cerr << "expecting a list but get : " << elemType->toString() << "\n";
        assert(0);
     }
     POETCode* start = eval_AST(elemTypeList->get_first());
     if (start != EMPTY)
        apply(start,EMPTY);

     POETCode* result = 0;
     POETList *p = dynamic_cast<POETList*>(elemTypeList->get_rest()); 
     while (p != 0 && p->get_rest() != 0)  
        {
           POETCode* cur = p->get_first(); 
           p = dynamic_cast<POETList*>(p->get_rest()); 
           assert(p != 0);
           POETCode* sep = eval_AST(p->get_first());
           apply(cur, sep);
           result = fac->append_tuple(result, res);
           p = dynamic_cast<POETList*>(p->get_rest()); 
        }
     if (p != 0) { 
        apply(p->get_first(), fullmatch);
        if (fullmatch != 0) fullmatch = EMPTY;
     }
     res = result;
  }
  void parseList( POETParseList* op, POETCode* elemType, POETCode* sep)
  {
     if (r1 == 0 || r1 == EMPTY) { res = EMPTY; return; }
     POETCode* r1_first = get_head(r1);
     if (r1_first->get_enum()==SRC_STRING && !backtrack && !op->match_lookahead(r1)) { res = EMPTY; return; }
     else if (r1_first->get_enum() == SRC_LVAR && !match_AST(r1_first, elemType, MATCH_AST_PATTERN)) { res = EMPTY; return; }
     POETCode* r1save = r1;
     apply(elemType, EMPTY);
     if (r1save == r1 && res == EMPTY) return;
     if (r1 == 0 || r1 == EMPTY) { 
           if (res->get_enum() == SRC_LIST)
              res = fac->new_list(res, 0);
          return; 
     }
     POETCode* firstResult = res;
     if (sep != EMPTY) {
        if (sep == get_head(r1)) { r1 = NextToken(r1); } 
        else { res = firstResult;  
           if (res->get_enum() == SRC_LIST)
              res = fac->new_list(res, 0);
           return; } 
     }
     POETCode* _r1 = r1;
     try { parseList(op, elemType, sep); }
     catch (ParseError err) { 
       if (!backtrack || sep!=EMPTY || fullmatch==0) throw err; 
       else { 
            r1 = _r1; res = firstResult; 
            if (res->get_enum() == SRC_LIST)
               res = fac->new_list(res, 0);
             return; 
        }
     }
     if (res != 0 && res!=EMPTY && res->get_enum() != SRC_LIST)
        res = fac->new_list(res, 0);
     if (firstResult != EMPTY) {
        if (res == EMPTY) res = 0;
        res = fac->new_list(firstResult, res);
     }
  }
  POETCode* match_eval(POETCode* v2, bool mod=true) {
     if (r1 == 0) { return res=match_AST(EMPTY, v2, MATCH_AST_PATTERN); }
     if (fullmatch == 0 || r1->get_enum() != SRC_LIST) {
        res = match_AST (r1, v2, MATCH_AST_PATTERN); 
        if (res != 0 && mod) { r1 = 0; }
     }
     else if (fullmatch == EMPTY || v2->get_enum() == SRC_CVAR) {
        assert ( r1->get_enum() == SRC_LIST); 
        POETList* r1_list = static_cast<POETList*>(r1);
        res = match_AST (r1_list->get_first(), v2, MATCH_AST_PATTERN);
        if (res != 0 && mod) { r1 = r1_list->get_rest() ; } 
     }
     return res;
  }

  virtual void visitString(POETString* v) {
     if (r1 == v) { res = r1; r1 = EMPTY;  return; }
     if (fullmatch != 0 && v == EMPTY) { res = v; return; }
     POETList* r1list = dynamic_cast<POETList*>(r1);
     if (r1list != 0 && r1list->get_first() == v) {
            res = v; r1 = r1list->get_rest(); return;
      }
    PARSE_MISMATCH(r1,v,lineno);
  }
  void defaultVisit(POETCode* v2) {
     if (v2 == EMPTY) { res = EMPTY; return; }
     if (match_eval(v2)) return;

     if ( r1->get_enum() != SRC_LIST) { PARSE_MISMATCH(r1,v2,lineno) }
     POETList* r1_list = static_cast<POETList*>(r1);

            POETCode* after_stop;
            POETCode* before_stop = SubList(r1_list, fullmatch, after_stop); 
            if (after_stop == 0 || after_stop == EMPTY) {
                 PARSE_MISMATCH(r1,v2,lineno); // not found fullmatch
              }
            POETCode* r1Save = r1;
            r1 = before_stop;
            try { res = apply(v2,0); }
            catch (ParseError err) { r1=r1Save; throw err; }
            r1=r1Save; 
            if (res != 0) { r1 = after_stop; return; }
      res = match_AST(EMPTY, v2, MATCH_AST_PATTERN); 
     if (res == 0) {
        PARSE_MISMATCH(EMPTY,v2,lineno)
     }
  }

  virtual void visitLocalVar(LocalVar* v)
  {
     LvarSymbolTable::Entry e =v->get_entry();
     POETCode* code = e.get_code();
     if (code != v && code != 0)  code->visit(this); 
     else {
        POETCode* restr =  e.get_restr();
        if (restr != 0) restr->visit(this);
        else SYM_UNDEFINED(v->toString());
        e.set_code(res);
     }
  }
  
  virtual void visitType(POETType *t)
  {
    if (t == ANY && fullmatch == 0) { /*QY: go over annotations */
       POETCode* r1_save = r1, *r1_tail=get_tail(r1);
       r1 = r1_tail;
       if (r1 != 0) {
          POETCode* input2 = apply(t, 0);
          if (input2 == r1_tail) res = r1_save;
          else res = fac->new_list(get_head(r1_save), input2);
       }
       else res = r1_save;
       return;
    }
    POETCode* input = (fullmatch==0)? r1 : get_head(r1);
    res = match_Type(input, t, true);
    if (res == 0) PARSE_MISMATCH(input,t,lineno);
    if (fullmatch != 0) r1 = get_tail(r1);
    else r1 = 0;
  }
  virtual void visitOperator(POETOperator *op)
  {
     switch (op->get_op()) {
     case POET_OP_TUPLE: {
       parseTuple(op->get_arg(0));
       break;
      }
     case TYPE_LIST1:
     case TYPE_LIST: {
         POETCode* arg = op->get_arg(0);
         POETCode* matchSave = fullmatch; fullmatch = EMPTY;
         std::vector<POETCode*> match_res;
         do {
            POETCode* r1_first=match_eval(arg);
            if (r1_first == 0)  { 
                fullmatch = matchSave;
                if (fullmatch != 0) {
                    res = Vector2List(match_res); 
                    if (res == EMPTY && op->get_op() == TYPE_LIST1)
                         PARSE_MISMATCH(r1,op,lineno);
                    return; 
                   }
                else PARSE_MISMATCH(r1,op,lineno);
            }
            match_res.push_back(r1_first);
         } while (r1 != 0);
         fullmatch = matchSave;
         if (match_res.size() == 0) PARSE_MISMATCH(r1,op,lineno);
         res = Vector2List(match_res);
         assert(res != EMPTY);
         return;
       }
     case TYPE_TOR: {
        POETCode* r1_first = get_head(r1);
        if (r1_first->get_enum() == SRC_LVAR // r1 is a trace handle
           || (r1_first->get_enum() == SRC_CVAR &&
             static_cast<CodeVar*>(r1_first)->get_parseInfo() != EMPTY)) 
            { //QY: r1_first is already parsed and is not a token  
              if (match_eval(op)) return;
            }
 
         if (!backtrack) {
            POETTypeTor* tor = static_cast<POETTypeTor*>(op);
            POETCode* arg = tor->get_parseInfo(r1);
            apply(arg, fullmatch); 
            if (fullmatch != 0) fullmatch=EMPTY; 
            return;
         }
         else  {
            POETCode* r1save = r1;
            size_t size = op->numOfArgs();
            for (unsigned i = 0; i < size; ++i) {
               POETCode* arg = op->get_arg(i);
               POETCode* filter = 0;
               switch (arg->get_enum()) {
               case SRC_STRING: if (arg != EMPTY) filter = arg; break;
               case SRC_CVAR:
                 filter=static_cast<CodeVar*>(arg)->get_parseInfo();
                 if (filter != 0 && filter->get_enum() == SRC_LIST)
                    filter = static_cast<POETList*>(filter)->get_first();
                 else filter = 0;
                 break;
               default: ; /* no filtering */
               }
               if (filter != 0 && filter->get_enum()==SRC_STRING && filter != r1_first) {
                 continue;
               }
               try { apply(arg, EMPTY);
                    if (res != 0) return; }
               catch (ParseError err) { r1 = r1save; }
            }
         }
         PARSE_MISMATCH(r1,op,lineno);
         return;
      }
     case POET_OP_EXP: {
       ParseExp::Result exp_res = ParseExp::inst()->parse(r1, &lineno);
       res = exp_res.first;
       if (res == 0) PARSE_MISMATCH(r1,op,lineno); 
       if (fullmatch != 0) r1 = exp_res.second;
       else if (exp_res.second != 0 && exp_res.second != EMPTY) 
             { PARSE_MISMATCH_LEFT(exp_res.second,lineno); }
       else r1 = 0;
       break;
       }
     case POET_OP_LIST1:
     case POET_OP_LIST: {
       POETParseList* oplist = dynamic_cast<POETParseList*>(op);
       assert(oplist != 0);
       std::string sepString;
       POETCode* sep = op->get_arg(1);
       if (sep != 0) { 
           sep  = eval_AST(sep);  
           POETString* sep1 = AST2String(sep);
           if (sep1 == 0) INCORRECT_STRING(sep);
           sepString = sep1->get_content();
       }
       else sep = EMPTY;
       if (IS_SPACE(sepString)) sep = EMPTY;
       try { parseList(oplist, op->get_arg(0), sep); }
       catch (ParseError err) { 
           if (op->get_op() == POET_OP_LIST && backtrack && fullmatch != 0) res = EMPTY; 
           else throw err; }
       break;
     }
     default:
        defaultVisit(eval_AST(op));
     }
  }
  bool ExamineParseFunctionResult()
    {
     if (res != 0 && res != EMPTY) {
        POETTuple* vec = dynamic_cast<POETTuple*>(res);
        if (vec == 0 || vec->size() != 2) 
           EXIT("ERROR: Parsing function must return a (code, rest-of-input) pair : " + res->toString() + "\n"); 
        POETCode*res1 = vec->get_entry(1);
        POETCode*res0 = vec->get_entry(0);
        if (res1 != 0 && res1 != EMPTY && fullmatch == 0) 
          { return false; }
        r1 = res1; res = res0;
        return true;
     }
     return false;
    }
  virtual void visitXformVar(XformVar* v)
  {
    POETCode* _r1 = r1;
    try {
     XformVarInvoke xform(v->get_entry(), v->get_config(), r1);
     res = eval_AST(&xform);
     if (!ExamineParseFunctionResult()) 
          { res = 0; 
          PARSE_MISMATCH(_r1,v,lineno); }
    }
    catch (ParseError err) {
        r1 = _r1; throw err;
    }
    catch (Error err) { std::cerr << "From invoking " << v->toString(DEBUG_NO_VAR) << "\n"; throw err; }
  }
  virtual void visitAssign(POETAssign* assign)  {
          check_localVar(assign->get_rhs());
          apply(assign->get_rhs(),fullmatch);
          res = assign_AST(res, assign->get_lhs());
          if (fullmatch != 0) fullmatch=EMPTY; 
    }

  virtual void visitCodeVar(CodeVar* v2)  
   {
     if (match_eval(v2)) { return; }

     POETCode* v2parse = v2->get_parseInfo();
     assert (v2parse != 0); 
     if (v2parse == EMPTY) 
         PARSE_MISMATCH(v2,r1,lineno); // v2 is built only through scanning
     POETCode* pars=0;
     switch (v2parse->get_enum()) {
       case SRC_OP:
       case SRC_XVAR: break; /*QY: parameters are not involved in parsing*/
       default: pars = v2->get_entry().get_param();
     }
     POETCode* r1Save = r1;
     try {
           if (pars != 0) 
               v2->get_entry().get_symTable()->push_table(false);
           LocalVar* inherit_var = v2->get_entry().get_inherit_var();
           if (inherit_var != 0) {
                inherit_var->get_entry().set_code( inherit);
                if (debug_parse())
                   std::cerr << inherit_var->toString(OUTPUT_VAR_VAL) << "\n";
           }
           apply(v2parse,fullmatch);
           if (fullmatch != 0) fullmatch=EMPTY; 
           if (pars != 0) {
               /*QY get the values stored in pars*/
               pars=ReplaceSingletonListVisitor().apply(pars); 
               POETCode* cond = v2->get_static_attr("cond");
               if (cond != 0 && !AST2Bool(eval_AST(cond))) 
                      PARSE_MISMATCH(v2,pars,lineno);
               res = v2->invoke_rebuild(pars);
               v2->get_entry().get_symTable()->pop_table();
            } 
            else if (v2->get_entry().get_param() != 0)
            {
               if (!v2->check_cond(res))
                      PARSE_MISMATCH(v2,res,lineno);
               POETCode* rres = v2->invoke_rebuild(res);
               res = rres;
            }
            else if (v2->get_entry().get_code() != 0) 
                 res = fac->build_codeRef(v2->get_entry(),0);
     }
     catch (ParseError err) {
             if (pars != 0) v2->get_entry().get_symTable()->pop_table();
             r1 = r1Save;
             throw err;
     }
     catch (Error err) { 
             std::cerr << " From parsing code template " << v2->toString() << "\n"; 
             throw err;
     }
  }
  void visitTuple( POETTuple* v2) 
  {
     POETCode* result = 0;
     int size = v2->size();
     for (int i = 0; i < size-1; ++i) {
           apply(v2->get_entry(i), EMPTY);
           result = fac->append_tuple(result, res);
     }
     apply(v2->get_entry(size-1), fullmatch);
     if (fullmatch != 0) fullmatch=EMPTY; 
     result = fac->append_tuple(result, res);
     res = result;
  }
  POETEnum get_matchEnum(POETCode* v2first)
   {
     POETEnum v2first_enum = v2first->get_enum();
     if (v2first_enum == SRC_ASSIGN) {
        POETCode* c  = static_cast<POETAssign*>(v2first)->get_rhs();
        if (c != 0) {
            v2first_enum = c->get_enum();
        }
     }
     return v2first_enum;
   }

  virtual void visitList(POETList* v2)  {
     POETCode* v2Tail = v2->get_rest();
     if (v2Tail == 0) {
         apply(v2->get_first(), fullmatch); 
         if (fullmatch != 0) fullmatch=EMPTY; 
         res = fac->new_list(res, 0);
         return;
     } 
     //if (r1->get_enum() != SRC_LIST)  PARSE_MISMATCH(r1,v2,lineno);
     assert (v2Tail->get_enum() == SRC_LIST); 

     POETCode* v2Head=v2->get_first(); 
     POETCode* v2Second = static_cast<POETList*>(v2Tail)->get_first();
     POETCode* v2SecondTail = static_cast<POETList*>(v2Tail)->get_rest();
     POETCode *sep = EMPTY, *after_first = v2Tail;
     if ( v2Head->get_enum() != SRC_STRING && get_matchEnum(v2Second) == SRC_STRING)  {
       sep = v2Second; after_first = v2SecondTail;    
     } 
     
     POETCode* _r1 = r1;
     POETCode* res1 = apply(v2->get_first(), sep);
     if (after_first == 0) {
           if ( r1 == 0 || r1 == EMPTY || fullmatch != 0)
             { res = fac->new_list(res1, 0); return; }
           r1 = _r1; PARSE_MISMATCH(_r1, v2,lineno);
      } 
     try {    
           POETList* l = dynamic_cast<POETList*>(after_first);
           if (l != 0 && l->get_rest() == 0) after_first = l->get_first();
           apply(after_first, fullmatch); 
           if (fullmatch != 0) fullmatch=EMPTY; 
         }
     catch (ParseError err) { 
           r1 = _r1; throw err; 
        }
     res = fac->new_list(res1, res);
   }
};

class ApplyTokenOperator : public ReplInfoVisitor
{
  POETCode* tokens;
 public: 
  ApplyTokenOperator(POETCode* _t) : tokens(_t) { assert(tokens!=0); }
  virtual void visitList(POETList* l)
   { 
     ReplInfoVisitor::visitList(l);
     if (res->get_enum() == SRC_LIST)
       res = apply_tokens(static_cast<POETList*>(res)); 
     if (res == 0) res = EMPTY;
   }
   virtual void visitCodeVar( CodeVar* v) { res = v; }
   virtual void visitOperator(POETOperator *op)
   {
    if (op->get_op() == POET_OP_TYPEMATCH || op->get_op() == POET_OP_TYPEMATCH_Q) 
    {
       POETBop* bop = dynamic_cast<POETBop*>(op);
       assert(bop != 0);
       POETCode* arg = bop->get_arg1();
       arg->visit(this); 
       if (arg != res) bop->set_arg1(res);
       res = op;
     }
   }
};

POETCode* EvaluatePOET::parse_AST(POETCode* input, POETCode* pattern, POETCode** leftOver)
{  
      ParseMatchVisitor matchop(input, curfile->get_backtrack());
      POETCode* res = matchop.apply(pattern, (leftOver==0)? 0 : EMPTY, leftOver);
      POETCode* left = matchop.get_rest();
      if (leftOver==0 && left != 0 && left != EMPTY)
          PARSE_MISMATCH_LEFT(left, matchop.get_lineno());
      return res;
}

POETCode* EvaluatePOET::
eval_TypeMatch(POETCode* input, POETCode* pattern, bool throwError)
{
  try { return parse_AST(input, pattern); }
  catch (ParseError err) {
       if (!throwError) return 0;
        else { EXIT(err.message()); }
  }
}

POETCode* EvaluatePOET::parse_input(POETCode* input, POETCode* pattern)
{
   static double token_time = 0, prep_time=0, parse_time=0;
   static bool  first=true;
   ParseExp::reset();

   bool dt = debug_time();
if (dt) {
 if (first) {
    register_timing(&token_time, "time spent in tokenizer:");
    register_timing(&prep_time, "time spent in parse preparation:");
    register_timing(&parse_time, "time spent in parsing and AST construction:");
 }
first=false;
}

   if (prep->get_entry().get_code() != 0) {
      double cur = (dt?GetWallTime():0);
      XformVar* prep_xform = dynamic_cast<XformVar*>(prep->get_entry().get_code());
      if (prep_xform == 0) INCORRECT_XVAR(prep);
      input = prep_xform->eval(input, false);
      if (dt) prep_time +=GetWallTime()-cur;
   }
   if (tokens->get_entry().get_code() != 0) {
      double cur = (dt?GetWallTime():0);
      ApplyTokenOperator op(tokens->get_entry().get_code());
      input = op.apply(input);
      if (dt) token_time +=GetWallTime()-cur;
   }
   input = eval_AST(input);
   if (pattern == 0) pattern = parseTarget->get_entry().get_code();
   if (pattern == 0) return input; 
   switch (pattern->get_enum())
    {
    case SRC_READ_INPUT: return input;
    default:
       try { 
          double cur = (dt?GetWallTime():0);
          POETCode* result = parse_AST(input, pattern);
          if (dt) parse_time +=GetWallTime()-cur;
            return result;
          }
       catch (ParseError err) { EXIT(err.message()); }
   }
}



