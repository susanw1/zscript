/*
 * ZcodeLocalChannel.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */
#include "ZcodeLocalChannel.hpp"

char ZcodeLocalLookaheadStream::read() {
    return parent->charAt(relativePos++);
}
