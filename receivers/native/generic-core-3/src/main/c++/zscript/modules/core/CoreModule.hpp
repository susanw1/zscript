/*
 * CoreModule.hpp
 *
 *  Created on: 2 Aug 2023
 *      Author: alicia
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_CORE_COREMODULE_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_CORE_COREMODULE_HPP_

#include "../../ZscriptIncludes.hpp"
#include "../../execution/ZscriptCommandContext.hpp"
#include "../ZscriptModule.hpp"

#define MODULE_EXISTS_000 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_000 MODULE_SWITCH_UTIL(ZcodeCoreModule<ZP>::execute)

#include "CapabilitiesCommand.hpp"

namespace Zscript {
namespace GenericCore {
template<class ZP>
class ZcodeCoreModule: public ZcodeModule<ZP> {

public:

    static void execute(ZscriptCommandContext<ZP> ctx, uint8_t bottomBits) {

        switch (bottomBits) {
        case 0x0:
            CapabilitiesCommand<ZP>::execute(ctx);
            break;
        default:
            ctx.status(ResponseStatus::COMMAND_NOT_FOUND);
            break;
        }
    }

};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_MODULES_CORE_COREMODULE_HPP_ */
