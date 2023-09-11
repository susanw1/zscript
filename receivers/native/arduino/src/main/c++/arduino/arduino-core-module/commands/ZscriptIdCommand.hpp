//
// Created by alicia on 11/09/23.
//

#ifndef ZSCRIPT_ZSCRIPTIDCOMMAND_H
#define ZSCRIPT_ZSCRIPTIDCOMMAND_H

#include <zscript/modules/ZscriptCommand.hpp>
#include "../../arduino-core-module/persistence/PersistenceSystem.hpp"

#define COMMAND_EXISTS_0004 EXISTENCE_MARKER_UTIL
#define COMMAND_EXISTS_0005 EXISTENCE_MARKER_UTIL
#define COMMAND_EXISTS_0014 EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class ZscriptIdCommand {
    static uint8_t tempIdArr[2];
    static uint8_t guidLoc;
public:
    enum IdType : uint8_t {
        TEMPORARY_ID, GUID, HARDWARE_ID
    };
    static constexpr char ParamIdType__I = 'I';
    static constexpr char ParamFailOnMismatchedId__F = 'F';

    static void setup() {
        guidLoc = PersistenceSystem<ZP>::reserveSection(16);
    }

    static void fetchId(ZscriptCommandContext<ZP> ctx) {
        CommandOutStream<ZP> out = ctx.getOutStream();
        uint16_t idType;
        if (!ctx.getField(ParamIdType__I, &idType)) {
            ctx.status(MISSING_KEY);
            return;
        }
        bool matches = false;
        uint8_t i = 0;
        uint8_t bigLength = ctx.getBigFieldSize();
        switch (idType) {
            case TEMPORARY_ID:
                out.writeBigHex(tempIdArr, 2);
                if (bigLength != 0) {
                    if (bigLength != 2) {
                        ctx.status(VALUE_OUT_OF_RANGE);
                        return;
                    }
                    matches = true;
                    for (BigFieldBlockIterator<ZP> iter = ctx.getBigField(); iter.hasNext();) {
                        if (tempIdArr[i++] != iter.next()) {
                            matches = false;
                            break;
                        }
                    }
                }
                break;
            case GUID:
                uint8_t guidValue[16];
                PersistenceSystem<ZP>::readSection(guidLoc, 16, guidValue);
                out.writeBigHex(guidValue, 16);
                if (bigLength != 0) {
                    if (bigLength != 16) {
                        ctx.status(VALUE_OUT_OF_RANGE);
                        return;
                    }
                    matches = true;
                    for (BigFieldBlockIterator<ZP> iter = ctx.getBigField(); iter.hasNext();) {
                        if (guidValue[i++] != iter.next()) {
                            matches = false;
                            break;
                        }
                    }
                }
                break;
            default:
                ctx.status(VALUE_OUT_OF_RANGE);
                return;
        }
        if (!matches && ctx.hasField(ParamFailOnMismatchedId__F)) {
            ctx.status(COMMAND_FAIL_CONTROL);
        }
    }

    static void makeRandomCode(ZscriptCommandContext<ZP> ctx) {
        uint16_t random = ZP::generateRandom16();
        tempIdArr[0] = random & 0xFF;
        tempIdArr[1] = (random >> 8) & 0xFF;
    }

    static void saveId(ZscriptCommandContext<ZP> ctx) {
        uint8_t guidToSave[16];
        if (ctx.getBigFieldSize() != 16) {
            ctx.status(VALUE_OUT_OF_RANGE);
            return;
        }
        uint8_t i = 0;
        for (BigFieldBlockIterator<ZP> iter = ctx.getBigField(); iter.hasNext();) {
            guidToSave[i++] = iter.next();
        }
        PersistenceSystem<ZP>::readSection(guidLoc, 16, guidToSave);
    }

};

template<class ZP>
uint8_t ZscriptIdCommand<ZP>::guidLoc = 0;
template<class ZP>
uint8_t ZscriptIdCommand<ZP>::tempIdArr[2] = {0, 0};
}

#define ZSCRIPT_ID_FETCH_COMMAND
#define ZSCRIPT_ID_SAVE_COMMAND
#define ZSCRIPT_ID_RANDOM_COMMAND

#endif //ZSCRIPT_ZSCRIPTIDCOMMAND_H
