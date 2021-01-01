/*
 * RCode.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "RCode.hpp"

void RCode::progressRCode() {
    debug.attemptFlush();
    parser.parseNext();
    runner.runNext();
}
