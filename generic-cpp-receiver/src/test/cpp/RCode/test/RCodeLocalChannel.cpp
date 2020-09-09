/*
 * RCodeLocalChannel.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */
#include "RCodeLocalChannel.hpp"

char RCodeLocalLookaheadStream::read() {
    return parent->charAt(relativePos++);
}
