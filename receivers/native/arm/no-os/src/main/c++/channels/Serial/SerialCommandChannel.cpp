/*
 * SerialCommandChannel.cpp
 *
 *  Created on: 2 Aug 2021
 *      Author: robert
 */

#include "SerialCommandChannel.hpp"

SerialCommandChannel *SerialCommandChannel::channel = NULL;

void SerialCommandChannelNewlineCallback(SerialIdentifier id) {
    SerialCommandChannel::channel->hasNewline = true;
}
