///*
// * RCodeInStream.cpp
// *
// *  Created on: 8 Sep 2020
// *      Author: robert
// */
//#include "RCodeInStream.hpp"
//
//int16_t RCodeInStream::readInternal() {
//    if (!hasReachedCommandEnd) {
//        int16_t prev = current;
//        if (sequenceIn->hasNext()) {
//            current = sequenceIn->next();
//            currentValid = true;
//            if (current == '\n'
//                    || ((current == '&' || current == '|') && !isInString)) {
//                hasReachedCommandEnd = true;
//                currentValid = false;
//                hasBackslash = false;
//                isInString = false;
//            } else if (current == '"') {
//                if (!hasBackslash) {
//                    isInString = !isInString;
//                } else {
//                    hasBackslash = false;
//                }
//            } else if (isInString) {
//                if (current == '\\') {
//                    hasBackslash = !hasBackslash;
//                } else {
//                    hasBackslash = false;
//                }
//            } else {
//                hasBackslash = false;
//            }
//        } else {
//            currentValid = false;
//        }
//        return prev;
//    } else {
//        return current;
//    }
//}
//RCodeLookaheadStream* RCodeInStream::getLookahead() {
//    lookahead.reset(sequenceIn->getLookahead());
//    return &lookahead;
//}
//
//bool RCodeInStream::hasNext() {
//    return currentValid || (!hasReachedCommandEnd && sequenceIn->hasNext());
//}
//
//void RCodeInStream::closeCommand() {
//    while (hasNext()) {
//        read();
//    }
//    if (current != '\n' && current != '&' && current != '|') {
//        current = sequenceIn->next();
//    }
//}
//void RCodeInStream::skipToError() {
//    while (hasNext() || current == '&') {
//        if (current == '&') {
//            openCommand();
//        }
//        read();
//    }
//    if (current != '\n' && current != '&' && current != '|') {
//        current = sequenceIn->next();
//    }
//}
//
//void RCodeInStream::skipSequence() {
//    if (current != '\n') {
//        sequenceIn->closeCommandSequence();
//        currentValid = false;
//        hasReachedCommandEnd = true;
//        current = -1;
//        hasBackslash = false;
//        isInString = false;
//    }
//}
