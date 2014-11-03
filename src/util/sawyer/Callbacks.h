#ifndef Sawyer_Callbacks_H
#define Sawyer_Callbacks_H

#include <sawyer/Sawyer.h>
#include <sawyer/SharedPointer.h>
#include <list>

namespace Sawyer {

// FIXME[Robb Matzke 2014-08-13]: documentation
template<class Callback>
class Callbacks {
private:
    typedef std::list<Callback> CbList;
    CbList callbacks_;

public:
    bool isEmpty() const {
        return callbacks_.empty();
    }

    Callbacks& append(const Callback &callback) {
        callbacks_.push_back(callback);
        return *this;
    }

    Callbacks& prepend(const Callback &callback) {
        callbacks_.push_front(callback);
        return *this;
    }

    Callbacks& eraseFirst(const Callback &callback) {
        for (typename CbList::iterator iter=callbacks_.begin(); iter!=callbacks_.end(); ++iter) {
            if (*iter == callback) {
                callbacks_.erase(iter);
                break;
            }
        }
        return *this;
    }

    Callbacks& eraseLast(const Callback &callback) {
        for (typename CbList::reverse_iterator iter=callbacks_.rbegin(); iter!=callbacks_.rend(); ++iter) {
            if (*iter == callback) {
                callbacks_.erase(iter);
                break;
            }
        }
        return *this;
    }

    Callbacks& eraseMatching(const Callback &callback) {
        typename CbList::iterator iter = callbacks_.begin();
        while (iter!=callbacks_.end()) {
            if (*iter == callback) {
                typename CbList::iterator toErase = iter++;
                callbacks_.erase(toErase);              // std::list iterators are stable over erasure
            } else {
                ++iter;
            }
        }
    }

    template<class CB, class Args>
    bool applyCallback(CB *callback, bool chained, const Args &args) const {
        return (*callback)(chained, args);
    }

    template<class CB, class Args>
    bool applyCallback(const SharedPointer<CB> &callback, bool chained, const Args &args) const {
        return (*callback)(chained, args);
    }

    template<class CB, class Args>
    bool applyCallback(CB &callback, bool chained, const Args &args) const {
        return callback(chained, args);
    }

    template<class Arguments>
    bool apply(bool chained, const Arguments &arguments) const {
        for (typename CbList::const_iterator iter=callbacks_.begin(); iter!=callbacks_.end(); ++iter)
            chained = applyCallback(*iter, chained, arguments);
        return chained;
    }
};

} // namespace

#endif
