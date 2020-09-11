/*
 * RCode.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_RCODE_HPP_
#define SRC_TEST_CPP_RCODE_RCODE_HPP_
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "parsing/RCodeParser.hpp"
#include "parsing/RCodeCommandFinder.hpp"
#include "RCodeRunner.hpp"

class RCodeCommandChannel;
class RCodeLockSet;

class RCode {
private:
    RCodeParser parser;
    RCodeRunner runner;
    RCodeCommandFinder finder;
    RCodeCommandChannel **channels = NULL;
    uint8_t channelNum = 0;
public:
    RCode() :
            parser(this), runner(this) {
    }
    void setChannels(RCodeCommandChannel **channels, uint8_t channelNum) {
        this->channels = channels;
        this->channelNum = channelNum;
    }
    void progressRCode() {
        parser.parseNext();
        runner.runNext();
    }
    RCodeCommandChannel** getChannels() {
        return channels;
    }
    uint8_t getChannelNumber() {
        return channelNum;
    }
    RCodeCommandFinder* getCommandFinder() {
        return &finder;
    }

    bool canLock(RCodeLockSet *locks) {
        return true;
    }

    void lock(RCodeLockSet *locks) {
        ;
    }

    void unlock(RCodeLockSet *locks) {
        ;
    }
    //TODO: Implement real locking for RCode
};

#endif /* SRC_TEST_CPP_RCODE_RCODE_HPP_ */
