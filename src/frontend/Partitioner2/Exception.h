#ifndef ROSE_Partitioner2_Exception_H
#define ROSE_Partitioner2_Exception_H

#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/DataBlock.h>
#include <Partitioner2/Function.h>

#include <stdexcept>
#include <string>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

class Exception: public std::runtime_error {
public:
    Exception(const std::string &mesg): std::runtime_error(mesg) {}
    ~Exception() throw() {}

};

class PlaceholderError: public Exception {
    rose_addr_t startVa_;
public:
    PlaceholderError(rose_addr_t startVa, const std::string &mesg)
        : Exception(mesg), startVa_(startVa) {}
    ~PlaceholderError() throw() {}
    rose_addr_t startVa() const { return startVa_; }
};

class BasicBlockError: public Exception {
    BasicBlock::Ptr bblock_;
public:
    BasicBlockError(const BasicBlock::Ptr &bblock, const std::string &mesg)
        : Exception(mesg), bblock_(bblock) {}
    ~BasicBlockError() throw() {}
    BasicBlock::Ptr bblock() const { return bblock_; }
};

class DataBlockError: public Exception {
    DataBlock::Ptr dblock_;
public:
    DataBlockError(const DataBlock::Ptr &dblock, const std::string &mesg)
        : Exception(mesg), dblock_(dblock) {}
    ~DataBlockError() throw() {}
    DataBlock::Ptr dblock() const { return dblock_; }
};

class FunctionError: public Exception {
    Function::Ptr function_;
public:
    FunctionError(const Function::Ptr &function, const std::string &mesg)
        : Exception(mesg), function_(function) {}
    ~FunctionError() throw() {}
    Function::Ptr function() const { return function_; }
};

} // namespace
} // namespace
} // namespace

#endif
