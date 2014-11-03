#ifndef ROSE_Partitioner2_ModulesM68k_H
#define ROSE_Partitioner2_ModulesM68k_H

#include <Partitioner2/Function.h>
#include <Partitioner2/Partitioner.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesM68k {

/** Matches an M68k function prologue with LINK instruction.
 *
 *  Matches a "LINK.W A6, ###" instruction where "###" is zero or negative, and creates a function at that address. */
class MatchLink: public FunctionPrologueMatcher {
protected:
    Function::Ptr function_;
public:
    /** Allocating constructor. */
    static Ptr instance() { return Ptr(new MatchLink); }
    virtual Function::Ptr function() const ROSE_OVERRIDE { return function_; }
    virtual bool match(const Partitioner*, rose_addr_t anchor) ROSE_OVERRIDE;
};

/** Matches M68k function padding. */
class MatchFunctionPadding: public FunctionPaddingMatcher {
public:
    /** Allocating constructor. */
    static Ptr instance() { return Ptr(new MatchFunctionPadding); }
    virtual rose_addr_t match(const Partitioner*, rose_addr_t anchor) ROSE_OVERRIDE;
};

/** Adjusts basic block successors for M68k "switch" statements. */
class SwitchSuccessors: public BasicBlockCallback {
public:
    /** Allocating constructor. */
    static Ptr instance() { return Ptr(new SwitchSuccessors); }
    virtual bool operator()(bool chain, const Args&) ROSE_OVERRIDE;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
