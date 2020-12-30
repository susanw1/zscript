/*
 * RCodeBigField.cpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#include "RCodeBigField.hpp"

void RCodeStandardBigField::copyTo(RCodeOutStream *out) const {
    if (length != 0) {
        if (string) {
            out->writeBigStringField(data, length);
        } else {
            out->writeBigHexField(data, length);
        }
    }
}
void RCodeBigBigField::copyTo(RCodeOutStream *out) const {
    if (length != 0) {
        if (string) {
            out->writeBigStringField(data, length);
        } else {
            out->writeBigHexField(data, length);
        }
    }
}
