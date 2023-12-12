/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_CORE_ECHOCOMMAND_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_CORE_ECHOCOMMAND_HPP_

#include "../../ZscriptIncludes.hpp"

#include <net/zscript/model/modules/base/CoreModule.hpp>
#include "../../execution/ZscriptCommandContext.hpp"
#include "../../LanguageVersion.hpp"

#define COMMAND_EXISTS_0001 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace GenericCore {

template<class ZP>
class EchoCommand: public core_module::Echo_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx) {
        GenericCore::TokenRingBuffer<ZP> *buffer = ctx.parseState->getReader().asBuffer();

        CommandOutStream<ZP> out = ctx.getOutStream();
        CommandTokenIterator<ZP> it = ctx.iteratorToMarker();
        for (GenericCore::OptionalRingBufferToken<ZP> opt = it.next(buffer); opt.isPresent; opt = it.next(buffer)) {
            RingBufferToken<ZP> token = opt.token;
            uint8_t key = token.getKey(buffer);
            if (token.isMarker(buffer)) {
                out.writeBytes(&key, 1);
            } else if (ZcharsUtils<ZP>::isNumericKey(key)) {
                if (key == Zchars::Z_STATUS) {
                    ctx.status(token.getData16(buffer));
                } else if (key != Zchars::Z_CMD) {
                    out.writeField(key, token.getData16(buffer));
                }
            } else if (key == Zchars::Z_BIGFIELD_QUOTED) {
                QuotedStringFieldWriter<ZP> str = out.beginQuotedString();
                for (RawTokenBlockIterator<ZP> tbi = token.rawBlockIterator(buffer); tbi.hasNext(buffer);) {
                    str.continueString(tbi.nextContiguous(buffer));
                }
                str.end();
            } else if (key == Zchars::Z_BIGFIELD_HEX) {
                BigHexFieldWriter<ZP> big = out.beginBigHex();
                for (RawTokenBlockIterator<ZP> tbi = token.rawBlockIterator(buffer); tbi.hasNext(buffer);) {
                    big.continueBigHex(tbi.nextContiguous(buffer));
                }
            }
        }
    }
};

}

}

#endif /* SRC_MAIN_C___ZSCRIPT_MODULES_CORE_ECHOCOMMAND_HPP_ */
