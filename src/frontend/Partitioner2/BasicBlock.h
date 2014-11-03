#ifndef ROSE_Partitioner2_BasicBlock_H
#define ROSE_Partitioner2_BasicBlock_H

#include <Partitioner2/Attribute.h>
#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/DataBlock.h>
#include <Partitioner2/Semantics.h>

#include <sawyer/Cached.h>
#include <sawyer/Optional.h>
#include <sawyer/SharedPointer.h>

#include <set>
#include <string>
#include <vector>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

namespace BaseSemantics = rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;

/** Basic block information.
 *
 *  A basic block is a sequence of distinct instructions with linear control flow from the first instruction to the last.  No
 *  edges are permitted to enter or leave the basic block except to the first instruction and from the last instruction,
 *  respectively.  The instructions of a basic block are not required to be contiguous or non-overlapping.
 *
 *  A basic block is a read-only object once it reaches the BB_COMPLETE state, and can thus be shared between partitioners and
 *  threads.  The memory for these objects is shared and managed by a shared pointer implementation. */
class BasicBlock: public Sawyer::SharedObject, public Attribute::StoredValues {
public:
    /** Shared pointer to a basic block. */
    typedef Sawyer::SharedPointer<BasicBlock> Ptr;

    /** Basic block successor. */
    class Successor {
    private:
        Semantics::SValuePtr expr_;
        EdgeType type_;
    public:
        explicit Successor(const Semantics::SValuePtr &expr, EdgeType type=E_NORMAL)
            : expr_(expr), type_(type) {}
        /** Symbolic expression for the successor address. */
        const Semantics::SValuePtr& expr() const { return expr_; }

        /** Type of successor. */
        EdgeType type() const { return type_; }
    };

    /** All successors in no particular order. */
    typedef std::vector<Successor> Successors;

private:
    bool isFrozen_;                                     // True when the object becomes read-only
    rose_addr_t startVa_;                               // Starting address, perhaps redundant with insns_[0]->p_address
    std::vector<SgAsmInstruction*> insns_;              // Instructions in the order they're executed
    BaseSemantics::DispatcherPtr dispatcher_;           // How instructions are dispatched (null if no instructions)
    BaseSemantics::RiscOperatorsPtr operators_;         // Risc operators even if we're not using a dispatcher
    BaseSemantics::StatePtr initialState_;              // Initial state for semantics (null if no instructions)
    bool usingDispatcher_;                              // True if dispatcher's state is up-to-date for the final instruction
    Sawyer::Optional<BaseSemantics::StatePtr> optionalPenultimateState_; // One level of undo information
    std::vector<DataBlock::Ptr> dblocks_;               // Data blocks owned by this basic block, sorted

    // The following members are caches either because their value is seldom needed and expensive to compute, or because
    // the value is best computed at a higher layer than a single basic block (e.g., in the partitioner) yet it makes the
    // most sense to store it here. Make sure clearCache() resets these to initial values.
    // FIXME[Robb P. Matzke 2014-08-13]: eventually we may want to support user-defined cache entries
    Sawyer::Cached<Successors> successors_;             // control flow successors out of final instruction
    Sawyer::Cached<std::set<rose_addr_t> > ghostSuccessors_;// non-followed successors from opaque predicates, all insns
    Sawyer::Cached<bool> isFunctionCall_;               // is this block semantically a function call?
    Sawyer::Cached<bool> isFunctionReturn_;             // is this block semantically a return from the function?
    Sawyer::Cached<BaseSemantics::SValuePtr> stackDelta_;// change in stack pointer from beginning to end of block

    void clearCache() const {
        successors_.clear();
        ghostSuccessors_.clear();
        isFunctionCall_.clear();
        isFunctionReturn_.clear();
        stackDelta_.clear();
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    // use instance() instead
    BasicBlock(rose_addr_t startVa, const Partitioner *partitioner)
        : isFrozen_(false), startVa_(startVa), usingDispatcher_(true) { init(partitioner); }

public:
    /** Static allocating constructor.
     *
     *  The @p startVa is the starting address for this basic block.  The @p partitioner is the partitioner on whose behalf
     *  this basic block is created.  The partitioner is not stored in the basic block, but is only used to initialize
     *  certain data members of the block (such as its instruction dispatcher). */
    static Ptr instance(rose_addr_t startVa, const Partitioner *partitioner) {
        return Ptr(new BasicBlock(startVa, partitioner));
    }

    /** Virtual constructor.
     *
     *  The @p startVa is the starting address for this basic block.  The @p partitioner is the partitioner on whose behalf
     *  this basic block is created.  The partitioner is not stored in the basic block, but is only used to initialize
     *  certain data members of the block (such as its instruction dispatcher). */
    virtual Ptr create(rose_addr_t startVa, const Partitioner *partitioner) const {
        return instance(startVa, partitioner);
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Status
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Determine if basic block is read-only.
     *
     *  Returns true if read-only, false otherwise. */
    bool isFrozen() const { return isFrozen_; }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Instructions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Get the address for a basic block.
     *
     *  A basic block's address is also the starting address of its initial instruction.  The initial instruction need not be
     *  the instruction with the lowest address, but rather the instruction which is always executed first by the basic
     *  block. */
    rose_addr_t address() const { return startVa_; }

    /** Get the address after the end of the final instruction.
     *
     *  This is the address that immediately follows the final byte of the instruction that is executed last by the basic
     *  block.  The final executed instruction need not be the instruction with the highest address. */
    rose_addr_t fallthroughVa() const;

    /** Get the number of instructions in this block. */
    size_t nInstructions() const { return insns_.size(); }

    /** Return true if this basic block has no instructions.
     *
     *  A basic block is always expected to have at least one instruction whose address is the same as the basic block's
     *  address, and this method returns true if that instruction has not yet been discovered and appended to this basic
     *  block. A basic block may also own data blocks, but they are not counted by this method. */
    bool isEmpty() const { return insns_.empty(); }

    /** Determine if this basic block contains an instruction at a specific address.
     *
     *  Returns a non-null instruction pointer if this basic block contains an instruction that starts at the specified
     *  address, returns null otherwise. */
    SgAsmInstruction* instructionExists(rose_addr_t startVa) const;

    /** Determines if this basic block contains the specified instruction.
     *
     *  If the basic block contains the instruction then this method returns the index of this instruction within the
     *  block, otherwise it returns nothing. */
    Sawyer::Optional<size_t> instructionExists(SgAsmInstruction*) const;

    /** Get the instructions for this block.
     *
     *  Instructions are returned in the order they would be executed (i.e., the order they were added to the block).
     *  Blocks in the undiscovered and not-existing states never have instructions (they return an empty vector); blocks in
     *  the incomplete and complete states always return at least one instruction. */
    const std::vector<SgAsmInstruction*>& instructions() const { return insns_; }

    /** Append an instruction to a basic block.
     *
     *  If this is the first instruction then the instruction address must match the block's starting address, otherwise
     *  the new instruction must not already be a member of this basic block.  No other attempt is made to verify the
     *  integrety of the intra-block control flow (i.e., we do not check that the previous instruction had a single
     *  successor which is the newly appended instruction).  It is an error to attempt to append to a frozen block.
     *
     *  When adding multiple instructions:
     *
     * @code
     *  BasicBlock::Ptr bb = protoBlock->create(startingVa)
     *      ->append(insn1)->append(insn2)->append(insn3)
     *      ->freeze();
     * @endcode */
    void append(SgAsmInstruction*);

    /** Undo the latest append.
     *
     *  An append can be undone so that instructions can be appended, the result checked, and then undone.  Only one level
     *  of undo is available. */
    void pop();


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Static data blocks
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Get the number of data blocks owned. */
    size_t nDataBlocks() const { return dblocks_.size(); }

    /** Determine if this basic block contains the specified data block.
     *
     *  If the basic block owns the specified data block then this method returns the specified pointer, otherwise it returns
     *  the null pointer. */
    DataBlock::Ptr dataBlockExists(const DataBlock::Ptr&) const;

    /** Make this basic block own the specified data block.
     *
     *  If the specified data block is not yet owned by this basic block, then this method adds the data block as a member of
     *  this basic block and returns true, otherwise nothing is inserted and returns false.  A data block cannot be inserted
     *  when this basic block is frozen. */
    bool insertDataBlock(const DataBlock::Ptr&);

    /** Data blocks owned.
     *
     *  Returned vector is sorted according to data block starting address. */
    const std::vector<DataBlock::Ptr> dataBlocks() const { return dblocks_; }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Semantics
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Return the initial semantic state.
     *
     *  A null pointer is returned if this basic block has no instructions. */
    const BaseSemantics::StatePtr& initialState() const { return initialState_; }

    /** Return the final semantic state.
     *
     *  The returned state is equivalent to starting with the initial state and processing each instruction.  If a semantic
     *  error occurs during processing then the null pointer is returned.  The null pointer is also returned if this basic
     *  block is empty. */
    BaseSemantics::StatePtr finalState();

    /** Return the dispatcher that was used for the semantics.
     *
     *  Dispatchers are specific to the instruction architecture, and also contain a pointer to the register dictionary
     *  that was used.  The register dictionary can be employed to obtain names for the registers in the semantic
     *  states. A null dispatcher is returned if this basic block is empty. */
    const BaseSemantics::DispatcherPtr& dispatcher() const { return dispatcher_; }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Control flow
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Control flow successors.
     *
     *  The control flow successors indicate how control leaves the end of a basic block. These successors should be the
     *  most basic level of information; e.g., a basic block that results in an unconditional function call should not have
     *  an edge representing the return from that call. The successors are typically computed in the partitioner and cached
     *  in the basic block. */
    const Sawyer::Cached<Successors>& successors() const { return successors_; }

    /** Ghost successors.
     *
     *  A ghost successor is a control flow successor that is present in an individual instruction, but not present in the
     *  broader scope of a basic block.  Ghost successors typically occur when a conditional branch instruction in the
     *  middle of a basic block has an opaque predicate, causing it to become an unconditional branch.  The return value is
     *  the union of the ghost successors for each instruction in the basic block, and is updated whenever the set of
     *  instructions in the basic block changes.  The ghost successors are typically computed in the partitioner and cached
     *  in the basic block. */
    const Sawyer::Cached<std::set<rose_addr_t> >& ghostSuccessors() const { return ghostSuccessors_; }

    /** Insert a new successor.
     *
     *  Inserts a new successor into the cached successor list.  If the successor is already present then it is not added
     *  again (the comparison uses structural equivalance).
     *
     *  @{ */
    void insertSuccessor(const BaseSemantics::SValuePtr&, EdgeType type=E_NORMAL);
    void insertSuccessor(rose_addr_t va, size_t nBits, EdgeType type=E_NORMAL);
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Cached properties computed elsewhere
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Is a function call?
     *
     *  If the basic block appears to be a function call then this property is set to true.  A block is a function call if
     *  it appears to store a return value on the stack and then unconditionally branch to a function.  It need not end
     *  with a specific CALL instruction, nor are all CALL instructions actually function calls.  This property is
     *  typically computed in the partitioner and cached in the basic block. */
    const Sawyer::Cached<bool>& isFunctionCall() const { return isFunctionCall_; }

    /** Is a function return?
     *
     *  This property indicates whether the basic block appears to be a return from a function call.  A block is a return from
     *  a function call if, after the block is executed, the instruction pointer contains the value stored in memory one past
     *  the top of the stack. */
    const Sawyer::Cached<bool>& isFunctionReturn() const { return isFunctionReturn_; }

    /** Stack delta.
     *
     *  The stack delta is a symbolic expression created by subtracting the initial stack pointer register from the final
     *  stack pointer register.  This value is typically computed in the partitioner and cached in the basic block. */
    const Sawyer::Cached<BaseSemantics::SValuePtr>& stackDelta() const { return stackDelta_; }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Output
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** A printable name for this basic block.  Returns a string like 'basic block 0x10001234'. */
    std::string printableName() const;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Private members for the partitioner
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    friend class Partitioner;
    void init(const Partitioner*);
    void freeze() { isFrozen_ = true; optionalPenultimateState_ = Sawyer::Nothing(); }
    void thaw() { isFrozen_ = false; }
};

} // namespace
} // namespace
} // namespace

#endif
