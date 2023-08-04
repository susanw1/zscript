/*
 * ZscriptNotificationContext.hpp
 *
 *  Created on: 4 Aug 2023
 *      Author: alicia
 */

#ifndef SRC_MAIN_C___ZSCRIPT_EXECUTION_ZSCRIPTNOTIFICATIONCONTEXT_HPP_
#define SRC_MAIN_C___ZSCRIPT_EXECUTION_ZSCRIPTNOTIFICATIONCONTEXT_HPP_
#include "../ZscriptIncludes.hpp"
#include "../AbstractOutStream.hpp"

namespace Zscript {
namespace GenericCore {

template<class ZP>
class ZscriptNotificationSource;

template<class ZP>
class ZscriptNotificationContext {
    ZscriptNotificationSource<ZP> *source;
    Zscript<ZP> *zscript;

public:
    ZscriptNotificationContext(ZscriptNotificationSource<ZP> *source, Zscript<ZP> *zscript) :
            source(source), zscript(zscript) {
        source->setNotificationComplete(true);
    }

    void notificationComplete() {
        source->setNotificationComplete(true);
    }

    void notificationNotComplete() {
        source->setNotificationComplete(false);
    }

    bool isNotificationComplete() {
        return source->isNotificationComplete();
    }

    bool isAddressing() {
        return source->isAddressing();
    }

    int getID() {
        return source->getID();
    }

    Zscript<ZP>* getZscript() {
        return zscript;
    }

    AbstractOutStream<ZP>* getOutStream() {
        return zscript->getNotificationOutStream();
    }

    AsyncActionNotifier<ZP> getAsyncActionNotifier() {
        return source->getAsyncActionNotifier();
    }
};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_EXECUTION_ZSCRIPTNOTIFICATIONCONTEXT_HPP_ */
