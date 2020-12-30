/*
 * RCodeFieldMap.cpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */
#include "RCodeFieldMap.hpp"

void RCodeFieldMap::copyFieldTo(RCodeOutStream *out, char c) const {
    bool hasSeenBefore = false;
    for (int i = 0; i < size; i++) {
        if (fields[i] == c) {
            if (!hasSeenBefore) {
                out->writeField(c, values[i]);
                hasSeenBefore = true;
            } else {
                out->continueField(values[i]);
            }
        } else if (hasSeenBefore) {
            break;
        }
    }

}
void RCodeFieldMap::copyTo(RCodeOutStream *out) const {
    char last = 0;
    for (int i = 0; i < size; i++) {
        if (fields[i] != 'E' && fields[i] != 'R' && fields[i] != 'S') {
            if (last == fields[i]) {
                out->continueField(values[i]);
            } else {
                out->writeField(fields[i], values[i]);
                last = fields[i];
            }
        }
    }
}
