#ifndef ROSE_Partitioner2_Engine_H
#define ROSE_Partitioner2_Engine_H

#include <BinaryLoader.h>
#include <Disassembler.h>
#include <Partitioner2/Function.h>
#include <Partitioner2/Partitioner.h>
#include <Partitioner2/Utility.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Base class for engines driving the partitioner.
 *
 *  An engine serves two purposes:
 *
 *  @li It provides a number of utility methods for preparing specimens to be partitioned, namely steps that parse the binary
 *      container (ELF, PE, etc), map sections into virtual memory, perform optional dynamic linking and relocation fixups,
 *      apply map resource strings ("map:" files), and choose disassembler based on architecture.
 *
 *  @li It provides a number of wrapper methods and algorithms for using a partitioner to organize instructions into basic
 *      blocks and functions.
 *
 *  Users need not use an engine at all if they don't want to -- everything the engine does can (and was previously) done at
 *  the user level with calls to other APIs.  However, using an engine for as many steps as possible will mean that user code
 *  will automatically benefit from improvements such as new algorithms.
 *
 *  Besides reimplementing engine algorithms in user code, a user can also modify behavior in these other ways:
 *
 *  @li The engine is intended to be a base class for user subclasses so the user can override individual algorithms and still
 *      use the other algorithms.
 *
 *  @li The engine API is written in terms of individual steps where the user has a chance to change things between steps.
 *      Each step will automatically execute any previous steps that are necessary. The main steps are: @ref parse, @ref load,
 *      and @ref partition.
 *
 *  @li The engine's partitioning API always takes a partitioner as an argument. The user can modify the partitioner's behavior
 *      by subclassing, registering partitioner callbacks, or modifying the partitioner state between calls to the engine. */
class Engine {
    SgAsmInterpretation *interp_;                       // interpretation set by loadSpecimen
    BinaryLoader *loader_;                              // how to remap, link, and fixup
    Disassembler *disassembler_;                        // not ref-counted yet, but don't destroy it since user owns it
    MemoryMap map_;                                     // memory map initialized by load()
public:
    Engine(): interp_(NULL), loader_(NULL), disassembler_() {}

    virtual ~Engine() {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Basic steps
    //
    // Call these in order specified here, but each one will also automatically run the previous steps if necessary.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Parse specimen binary containers.
     *
     *  Parses the specimen binary containers (ELF, PE, etc) but does not load segments into memory.  Any specified names that
     *  are map resources (begin with "map:") are discarded and the remaining names are passed to ROSE's @c frontend
     *  function. The best SgAsmInterpretation is then chosen and returned.  If the list of names has nothing suitable for
     *  @c frontend then the null pointer is returned.
     *
     * @{ */
    virtual SgAsmInterpretation* parse(const std::vector<std::string> &fileNames);
    SgAsmInterpretation* parse(const std::string &fileName) {
        return parse(std::vector<std::string>(1, fileName));
    }
    /** @} */

    /** Load and/or link interpretation.
     *
     *  Loads and/or links the engine's interpretation according to the engine's binary loader, first parsing the container if
     *  necessary. The following steps are performed:
     *
     *  @li If the engine has no interpretation then @ref parse is called first in order to parse the ELF, PE, etc. binary
     *      container. Only those specimens whose names are not memory map resources are used for this step.
     *
     *  @li If the engine has an interpretation (either originally or as a result of the previous step), and that
     *      interpretation lacks a memory map or has an empty memory map, then the @ref BinaryLoader::load method is invoked on
     *      the interpretation using the loader returned by @ref obtainLoader.  The loader configuration controls whether
     *      dynamic linking is performed.
     *
     *  @li If there is no interpretation or no memory map then an empty memory map is created, otherwise the interpretation's
     *      memory map is used for the following steps and the eventual return value.
     *
     *  @li If any specified specimen names are map resources (begin with "map:") then they are applied to the memory map.
     *
     * @{ */
    virtual MemoryMap load(const std::vector<std::string> &fileNames = std::vector<std::string>());
    MemoryMap load(const std::string &fileName) { return load(std::vector<std::string>(1, fileName)); }
    /** @} */


    /** Partition instructions into basic blocks and functions.
     *
     *  Parses and loads the specimen if necessary, then disassembles and organizes instructions into basic blocks and
     *  functions. Returns the partitioner that was used and which contains the results.
     *
     * @{ */
    Partitioner partition(const std::vector<std::string> &fileNames = std::vector<std::string>());
    Partitioner partition(const std::string &fileName) { return partition(std::vector<std::string>(1, fileName)); }
    virtual Partitioner partition(SgAsmInterpretation*);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Some utilities
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Documentation for specimen names. */
    static std::string specimenNameDocumentation();

    /** Obtain a binary loader.
     *
     *  This is usually called before the engine creates a partitioner. It looks for a suitable loader type and allocates an
     *  instance of the loader, saving a pointer.  If the caller provides a loader then that loader is used instead of creating
     *  a new one. Otherwise, if this engine already has a loader it does nothing.  Finally, failing those two situations, the
     *  engine looks at its interpretation, which must exist, to choose a loader.
     *
     *  If a new loader is created it is configured to map the specimen's sections but not perform linking or relocation
     *  fixups. */
    virtual BinaryLoader *obtainLoader(BinaryLoader *hint=NULL);

    /** Obtain a disassembler.
     *
     *  This is usually called before the engine creates a partitioner. It looks for a suitable disassembler type and allocates
     *  an instance of the disassembler, saving a pointer.  If the caller provides a disassembler then that disassembler is
     *  used instead of allocating a new one.  If the caller provides a non-empty architecture name, then that name is used to
     *  lookup up a disassembler.  Otherwise, if this engine already has a disassembler it does nothing.  Finally, failing
     *  those three situations, the engine looks at its interpretation, which must exist, to choose a disassembler.
     *
     *  @{ */
    virtual Disassembler* obtainDisassembler(Disassembler *hint=NULL);
    virtual Disassembler* obtainDisassembler(const std::string &isaName);
    /** @} */

    /** Create a bare partitioner.
     *
     *  A bare partitioner, as far as the engine is concerned, is one that has characteristics that are common across all
     *  architectures but which is missing all architecture-specific functionality.  Using the partitioner's own constructor
     *  is not quite the same--that would produce an even more bare partitioner!  The partitioner must have disassembler and
     *  memory map properties assigned already. They can be assigned explicitly with @ref disassembler and @ref memoryMap
     *  methods, and/or they can be allocated implicitly by calling steps up through @ref load. */
    virtual Partitioner createBarePartitioner();

    /** Create a generic partitioner.
     *
     *  A generic partitioner should work for any architecture but is not fine-tuned for any particular architecture. The
     *  partitioner must have disassembler and memory map properties assigned already. They can be assigned explicitly with
     *  @ref disassembler and @ref memoryMap methods, and/or they can be allocated implicitly by calling steps up through @ref
     *  load. */
    virtual Partitioner createGenericPartitioner();

    /** Create a tuned partitioner.
     *
     *  Returns a partitioner that is tuned to operate on the architecture described by the specified disassembler. The
     *  partitioner must have disassembler and memory map properties assigned already. They can be assigned explicitly with
     *  @ref disassembler and @ref memoryMap methods, and/or they can be allocated implicitly by calling steps up through @ref
     *  load. */
    virtual Partitioner createTunedPartitioner();

private:
    void checkCreatePartitionerPrerequisites() const;
    

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Property: interpretation.
     *
     *  Returns the interpretation, if any, that was created by the @ref parse step.  If the interpretation is changed then
     *  other properties such as @ref loader, @ref memoryMap, and @ref disassembler might also need to be changed in order to
     *  remain consistent with the interpretation.
     *
     * @{ */
    SgAsmInterpretation* interpretation() const { return interp_; }
    Engine& interpretation(SgAsmInterpretation *i) { interp_ = i; return *this; }
    /** @} */

    /** Property: loader
     *
     *  Returns or modifies the binary loader used by this engine during the @ref load step.  The loader is used to map
     *  interpretation sections into memory, link dynamic libraries, and apply relocation fixups.  Any of these steps can be
     *  enabled or disabled by adjusting properties of the loader.
     *
     * @sa obtainLoader
     *
     * @{ */
    BinaryLoader* loader() const { return loader_; }
    Engine& loader(BinaryLoader *l) { loader_ = l; return *this; }
    /** @} */

    /** Property: memory map
     *
     *  Returns the memory map resulting from the @ref load step.  This is a combination of the memory map created by the
     *  BinaryLoader and stored in the interpretation, and the application of any memory map resources ("map:" files).  During
     *  partitioning operations the memory map comes from the partitioner itself.
     *
     *  If a memory map is created and initialized to non-empty before the @ref load step then the load step does not run the
     *  BinaryLoader.
     *
     *  The return value is a non-const reference so that the map can be manipulated directly if desired.
     *
     * @{ */
    MemoryMap& memoryMap() { return map_; }
    const MemoryMap& memoryMap() const { return map_; }
    Engine& memoryMap(const MemoryMap &m) { map_ = m; return *this; }
    /** @} */

    /** Property: disassembler.
     *
     *  Returns or modifies the disassembler used by this engine.  The disassembler is used when the engine creates a
     *  partitioner during the @ref partition step.
     *
     * @sa obtainDisassembler
     *
     * @{ */
    Disassembler *disassembler() const { return disassembler_; }
    Engine& disassembler(Disassembler *d) { disassembler_ = d; return *this; }
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  High-level methods that mostly call low-level stuff
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Partitions instructions into basic blocks and functions.
     *
     *  This method is a wrapper around a number of lower-level partitioning steps that uses the specified interpretation to
     *  instantiate functions and then uses the specified partitioner to discover basic blocks and use the CFG to assign basic
     *  blocks to functions.  It is often overridden by subclasses. */
    virtual Engine& runPartitioner(Partitioner&, SgAsmInterpretation*);

    /** Discover as many basic blocks as possible.
     *
     *  Processes the "undiscovered" work list until the list becomes empty.  This list is the list of basic block placeholders
     *  for which no attempt has been made to discover instructions.  This method implements a recursive descent disassembler,
     *  although it does not process the control flow edges in any particular order. Subclasses are expected to override this
     *  to implement a more directed approach to discovering basic blocks. */
    virtual void discoverBasicBlocks(Partitioner&);

    /** Discover as many functions as possible.
     *
     *  Discover as many functions as possible by discovering as many basic blocks as possible (@ref discoverBasicBlocks) Each
     *  time we run out of basic blocks to try, we look for another function prologue pattern at the lowest possible address
     *  and then recursively discover more basic blocks.  When this procedure is exhausted a call to @ref
     *  attachBlocksToFunctions tries to attach each basic block to a function.
     *
     *  Returns a list of functions that need more attention.  These are functions for which the CFG is not well behaved--such
     *  as inter-function edges that are not function call edges. */
    virtual std::vector<Function::Ptr> discoverFunctions(Partitioner&);

    /** Runs post-partitioning fixups.
     *
     *  This method is normally run after the CFG/AUM is built. It does things like give names to some functions. The binary
     *  interpretation argument is optional, although some functionality is reduced when it is null. */
    virtual void postPartitionFixups(Partitioner&, SgAsmInterpretation*);
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Methods to make basic blocks
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Discover a basic block.
     *
     *  Obtains an arbitrary basic block address from the list of undiscovered basic blocks (i.e., placeholders that have no
     *  basic block assigned to them yet) and attempts to discover the instructions that belong to that block.  The basic block
     *  is then added to the specified partitioner's CFG/AUM.
     *
     *  Returns the basic block that was discovered, or the null pointer if there are no pending undiscovered blocks. */
    virtual BasicBlock::Ptr makeNextBasicBlock(Partitioner&);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Methods to make functions.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Make functions based on specimen container.
     *
     *  Traverses the specified interpretation parsed from, for example, related ELF or PE containers, and make functions at
     *  certain addresses that correspond to specimen entry points, imports and exports, symbol tables, etc.  This method only
     *  calls many of the other "make*Functions" methods and accumulates their results.
     *
     *  Returns a list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<Function::Ptr> makeContainerFunctions(Partitioner&, SgAsmInterpretation*);

    /** Make functions at specimen entry addresses.
     *
     *  A function is created at each specimen entry address for all headers in the specified interpretation and adds them to
     *  the specified partitioner's CFG/AUM.
     *
     *  Returns a list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<Function::Ptr> makeEntryFunctions(Partitioner&, SgAsmInterpretation*);

    /** Make functions at error handling addresses.
     *
     *  Makes a function at each error handling address in the specified interpratation and inserts the function into the
     *  specified partitioner's CFG/AUM.
     *
     *  Returns the list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<Function::Ptr> makeErrorHandlingFunctions(Partitioner&, SgAsmInterpretation*);

    /** Make functions at import trampolines.
     *
     *  Makes a function at each import trampoline and inserts them into the specified partitioner's CFG/AUM. An import
     *  trampoline is a thunk that branches to a dynamically loaded/linked function. Since ROSE does not necessarily load/link
     *  dynamic functions, they often don't appear in the executable.  Therefore, this function can be called to create
     *  functions from the trampolines and give them the same name as the function they would have called had the link step
     *  been performed.
     *
     *  Returns a list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<Function::Ptr> makeImportFunctions(Partitioner&, SgAsmInterpretation*);

    /** Make functions at export addresses.
     *
     *  Makes a function at each address that is indicated as being an exported function, and inserts them into the specified
     *  partitioner's CFG/AUM.
     *
     *  Returns a list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<Function::Ptr> makeExportFunctions(Partitioner&, SgAsmInterpretation*);

    /** Make functions for symbols.
     *
     *  Makes a function for each function symbol in the various symbol tables under the specified interpretation and inserts
     *  them into the specified partitioner's CFG/AUM.
     *
     *  Returns a list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<Function::Ptr> makeSymbolFunctions(Partitioner&, SgAsmInterpretation*);

    /** Make functions for function call edges.
     *
     *  Scans the partitioner's CFG to find edges that are marked as function calls and makes a function at each target address
     *  that is concrete.  The function is added to the specified partitioner's CFG/AUM.
     *
     *  Returns a list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<Function::Ptr> makeCalledFunctions(Partitioner&);

    /** Make function at prologue pattern.
     *
     *  Scans executable memory starting at the specified address and which is not represented in the CFG/AUM and looks for
     *  byte patterns and/or instruction patterns that indicate the start of a function.  When a pattern is found a function is
     *  created and inserted into the specified partitioner's CFG/AUM.
     *
     *  Patterns are found by calling the @ref Partitioner::nextFunctionPrologue method, which most likely invokes a variety of
     *  predefined and user-defined callbacks to search for the next pattern.
     *
     *  Returns a pointer to the newly inserted function if one was found, otherwise returns the null pointer. */
    virtual Function::Ptr makeNextPrologueFunction(Partitioner&, rose_addr_t startVa);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Methods that adjust existing functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Attach basic blocks to functions.
     *
     *  Calls @ref Partitioner::discoverFunctionBasicBlocks once for each known function the partitioner's CFG/AUM in a
     *  sophomoric attempt to assign existing basic blocks to functions.  Returns the list of functions that resulted in
     *  errors.  If @p reportProblems is set then emit messages to mlog[WARN] about problems with the CFG (that stream must
     *  also be enabled if you want to actually see the warnings). */
    virtual std::vector<Function::Ptr> attachBlocksToFunctions(Partitioner&, bool emitWarnings=false);

    /** Attach dead code to function.
     *
     *  Examines the ghost edges for the basic blocks that belong to the specified function in order to discover basic blocks
     *  that are not reachable according the CFG, adds placeholders for those basic blocks, and causes the function to own
     *  those blocks.
     *
     *  If @p maxIterations is larger than one then multiple iterations are performed.  Between each iteration @ref
     *  makeNextBasicBlock is called repeatedly to recursively discover instructions for all pending basic blocks, and then the
     *  CFG is traversed to add function-reachable basic blocks to the function.  The loop terminates when the maximum number
     *  of iterations is reached, or when no more dead code can be found within this function, or when the CFG reaches a state
     *  that has non-call inter-function edges.  In the last case, @ref Partitioner::discoverFunctionBasicBlocks can be called
     *  to by the user to determine what's wrong with the CFG.
     *
     *  Returns the set of newly discovered addresses for unreachable code.  These are the ghost edge target addresses
     *  discovered at each iteration of the loop and do not include addresses of basic blocks that are reachable from the ghost
     *  target blocks. */
    virtual std::set<rose_addr_t> attachDeadCodeToFunction(Partitioner&, const Function::Ptr&, size_t maxIterations=size_t(-1));

    /** Attach dead code to functions.
     *
     *  Calls @ref attachDeadCodeToFunction once for each function that exists in the specified partitioner's CFG/AUM, passing
     *  along @p maxIterations each time.
     *
     *  Returns the union of the dead code addresses discovered for each function. */
    virtual std::set<rose_addr_t> attachDeadCodeToFunctions(Partitioner&, size_t maxIterations=size_t(-1));

    /** Attach function padding to function.
     *
     *  Examines the memory immediately prior to the specified function's entry address to determine if it is alignment
     *  padding.  If so, it creates a data block for the padding and adds it to the function.
     *
     *  Returns the padding data block, which might have existed prior to this call.  Returns null if the function apparently
     *  has no padding. */
    virtual DataBlock::Ptr attachPaddingToFunction(Partitioner&, const Function::Ptr&);

    /** Attach padding to all functions.
     *
     *  Invokes @ref attachPaddingToFunction for each known function and returns the set of data blocks that were returned by
     *  the individual calls. */
    virtual std::vector<DataBlock::Ptr> attachPaddingToFunctions(Partitioner&);

    /** Attach intra-function data to functions.
     *
     *  Looks for addresses that are not part of the partitioner's CFG/AUM and which are surrounded immediately below and above
     *  by the same function and add that address interval as a data block to the surrounding function.  Returns the list of
     *  such data blocks added.
     *
     *  @todo In @ref attachSurroundedDataToFunctions: We can add a single-function version of this if necessary. It was done
     *  this way because it is more efficient to iterate over all unused addresses and find the surrounding functions than it
     *  is to iterate over one function's unused addresses at a time. [Robb P. Matzke 2014-09-08] */
    virtual std::vector<DataBlock::Ptr> attachSurroundedDataToFunctions(Partitioner&);
};

} // namespace
} // namespace
} // namespace

#endif
