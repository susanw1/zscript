/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

enum I2cFrequency {
    kHz10, kHz100, kHz400, kHz1000
};

enum I2cTerminationStatus {
    Complete,       // Successful completion - stopped when expected
    AddressNack,    // No response to address - device missing or busy
    DataNack,       // No response to a data byte - device stopped ack'ing. Probably bad.
    Address2Nack,   // No response to second address in send+receive pair. Eg bus noise, or device reset?
    BusError,       // Bus had a (bad) failure, eg jammed, or a STOP at wrong time, but recovered to usable state
    BusJammed,      // Bus jammed and recovery failed. Probably very bad.
    BusBusy,        // MultiMaster, so bus unavailable (MM not supported well at this time)
    MemoryError,    // probably indicates a bug! Unlikely in this setup
    OtherError      // eg setup error, or unknown error - probably a bug.
};