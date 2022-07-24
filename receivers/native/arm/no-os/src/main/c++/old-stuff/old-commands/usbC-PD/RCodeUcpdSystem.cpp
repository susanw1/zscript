/*
 * RCodeUcpdSystem.cpp
 *
 *  Created on: 6 Apr 2021
 *      Author: robert
 */

#include "../commands/RCodeUcpdSystem.hpp"

PowerStatus RCodeUcpdSystem::status = Negotiating;
uint16_t RCodeUcpdSystem::maxCurrent = 15;         // in 10mA
uint16_t RCodeUcpdSystem::minVoltage = 100;          // in 50mV
uint16_t RCodeUcpdSystem::maxVoltage = 100;          // in 50mV

UcpdSourceCapabilitiesMessage RCodeUcpdSystem::sourceCapMessage;
UcpdParsedExtendedSourceCapabilitiesMessage RCodeUcpdSystem::extendedSourceCapMessage;
UcpdParsedStatusMessage RCodeUcpdSystem::statusMessage;
UcpdParsedPPSStatusMessage RCodeUcpdSystem::PPSStatusMessage;
UcpdParsedManufacturerInfoMessage RCodeUcpdSystem::manufacturerInfoMessage;
