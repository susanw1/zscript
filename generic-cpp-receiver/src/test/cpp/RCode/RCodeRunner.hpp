/*
 * RCodeRunner.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_RCODERUNNER_HPP_
#define SRC_TEST_CPP_RCODE_RCODERUNNER_HPP_
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"

class RCode;
class RCodeCommandSequence;

class RCodeRunner {
private:
    RCode *const rcode;
    RCodeCommandSequence *running[RCodeParameters::maxParallelRunning];
    int parallelNum = 0;
    bool canBeParallel = false;

    void runSequence(RCodeCommandSequence *target, int targetInd);

    bool finishRunning(RCodeCommandSequence *target, int targetInd);

    RCodeCommandSequence* findNextToRun();
public:
    RCodeRunner(RCode *rcode) :
            rcode(rcode) {
    }

    void runNext();

};

#include "commands/RCodeActivateCommand.hpp"
#include "RCodeOutStream.hpp"
#include "RCode.hpp"
#include "parsing/RCodeCommandSequence.hpp"

#endif /* SRC_TEST_CPP_RCODE_RCODERUNNER_HPP_ */
