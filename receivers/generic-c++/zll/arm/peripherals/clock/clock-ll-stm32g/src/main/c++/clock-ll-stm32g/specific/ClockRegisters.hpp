/*
 * ClockRegisters.hpp
 *
 *  Created on: 23 Dec 2022
 *      Author: alicia
 */

#ifndef PERIPHERALS_CLOCK_CLOCK_LL_STM32G_SRC_MAIN_C___CLOCK_LL_STM32G_SPECIFIC_CLOCKREGISTERS_HPP_
#define PERIPHERALS_CLOCK_CLOCK_LL_STM32G_SRC_MAIN_C___CLOCK_LL_STM32G_SPECIFIC_CLOCKREGISTERS_HPP_

#include <llIncludes.hpp>
#include "SystemClocks.hpp"
#include <clock-ll/ClockManager.hpp>

template<class LL>
struct ClockRegisters {
private:
    static const uint32_t clockStabalisationTimMs = 10;
    // currently the highest frequency of the device in kHz - could take a while when on LSI
    static const uint32_t clockStabalisationTimExtMs = 1000;

    bool enableHSI48() {
        const uint32_t enableHSI48 = 0x1;
        const uint32_t HSI48Ready = 0x2;

        CRRCR |= enableHSI48;
        uint32_t syncCounter = 0;
        uint32_t syncMax = clockStabalisationTimMs * ClockManager<LL>::getClock(SysClock)->freq;
        while (syncCounter++ < syncMax && !(CRRCR & HSI48Ready))
            ;
        return (CRRCR & HSI48Ready);
    }
    bool enableHSI16() {
        const uint32_t enableHSI16 = 0x100;
        const uint32_t HSI16Ready = 0x400;

        CR |= enableHSI16;
        uint32_t syncCounter = 0;
        uint32_t syncMax = clockStabalisationTimMs * ClockManager<LL>::getClock(SysClock)->freq;
        while (syncCounter++ < syncMax && !(CR & HSI16Ready))
            ;
        return (CR & HSI16Ready);
    }
    bool enableLSI() {
        const uint32_t enableLSI = 0x1;
        const uint32_t LSIReady = 0x2;

        CSR |= enableLSI;
        uint32_t syncCounter = 0;
        uint32_t syncMax = clockStabalisationTimMs * ClockManager<LL>::getClock(SysClock)->freq;
        while (syncCounter++ < syncMax && !(CSR & LSIReady))
            ;
        return (CSR & LSIReady);
    }
    bool enableHSE() {
        const uint32_t enableHSE = 0x10000;
        const uint32_t HSEReady = 0x20000;

        CR |= enableHSE;
        uint32_t syncCounter = 0;
        uint32_t syncMax = clockStabalisationTimExtMs * ClockManager<LL>::getClock(SysClock)->freq;
        while (syncCounter++ < syncMax && !(CR & HSEReady))
            ;
        return (CR & HSEReady);
    }
    bool enableLSE() {
        const uint32_t enableLSE = 0x1;
        const uint32_t LSEReady = 0x2;

        BDCR |= enableLSE;
        uint32_t syncCounter = 0;
        uint32_t syncMax = clockStabalisationTimExtMs * ClockManager<LL>::getClock(SysClock)->freq;
        while (syncCounter++ < syncMax && !(BDCR & LSEReady))
            ;
        return (BDCR & LSEReady);
    }
    bool enableVCO() {
        const uint32_t enablePLL = 0x1000000;
        const uint32_t PLLReady = 0x2000000;

        CR |= enablePLL;
        uint32_t syncCounter = 0;
        uint32_t syncMax = clockStabalisationTimMs * ClockManager<LL>::getClock(SysClock)->freq;
        while (syncCounter++ < syncMax && !(CR & PLLReady))
            ;
        return (CR & PLLReady);
    }

public:
    uint32_t CR;
    uint32_t ICSRC;
    uint32_t CFGR;
    uint32_t PLLCFGR;

    uint32_t CIER;
    uint32_t CIFR;
    uint32_t CICR;

    uint32_t AHB1RSTR;
    uint32_t AHB2RSTR;
    uint32_t AHB3RSTR;

    uint32_t APB1RSTR1;
    uint32_t APB1RSTR2;
    uint32_t APB2RSTR;

    uint32_t AHB1MENR;
    uint32_t AHB2MENR;
    uint32_t AHB3MENR;

    uint32_t APB1MENR1;
    uint32_t APB1MENR2;
    uint32_t APB2MENR;

    uint32_t CCIPR;
    uint32_t BDCR;
    uint32_t CSR;
    uint32_t CRRCR;
    uint32_t CCIPR2;

    void selectSysClock(SystemClock source) {
        const uint32_t sysClkSwitchMask = 0x03;
        const uint32_t sysClkSwitchHSI16 = 0x01;
        const uint32_t sysClkSwitchHSE = 0x02;
        const uint32_t sysClkSwitchPLL = 0x03;
        uint32_t cfgr = CFGR;
        cfgr &= ~sysClkSwitchMask;
        if (source == PLL_R) {
            cfgr |= sysClkSwitchPLL;
        } else if (source == HSI) {
            cfgr |= sysClkSwitchHSI16;
        } else if (source == HSE) {
            cfgr |= sysClkSwitchHSE;
        } else {
            return;
        }
        CFGR = cfgr;
    }

    bool isEnabled(SystemClock source) {
        const uint32_t enableHSI16 = 0x100;
        const uint32_t enableHSE = 0x10000;
        const uint32_t enablePLL = 0x1000000;
        const uint32_t enablePLL_P = 0x10000;
        const uint32_t enablePLL_Q = 0x100000;
        const uint32_t enablePLL_R = 0x1000000;
        const uint32_t enableHSI48 = 0x1;
        const uint32_t enableLSI = 0x1;
        const uint32_t enableLSE = 0x1;
        switch (source) {
        case HSI:
            return (CR & enableHSI16);
        case HSI48:
            return (CRRCR & enableHSI48);
        case LSI:
            return (CSR & enableLSI);
        case HSE:
            return (CR & enableHSE);
        case LSE:
            return (BDCR & enableLSE);
        case VCO:
            return (CR & enablePLL);
        case PLL_P:
            return (PLLCFGR & enablePLL_P);
        case PLL_Q:
            return (PLLCFGR & enablePLL_Q);
        case PLL_R:
            return (PLLCFGR & enablePLL_R);

        case SysClock:
            return true;
        case HCLK:
            return true;
        case PCLK_1:
            return true;
        case PCLK_2:
            return true;
        default:
            return false;
        }
    }

    bool enable(SystemClock source) {
        const uint32_t enablePLL_P = 0x10000;
        const uint32_t enablePLL_Q = 0x100000;
        const uint32_t enablePLL_R = 0x1000000;
        switch (source) {
        case HSI:
            return enableHSI16();
        case HSI48:
            return enableHSI48();
        case LSI:
            return enableLSI();
        case HSE:
            return enableHSE();
        case LSE:
            return enableLSE();
        case VCO:
            return enableVCO();

        case PLL_P:
            PLLCFGR |= enablePLL_P;
            return true;
        case PLL_Q:
            PLLCFGR |= enablePLL_Q;
            return true;
        case PLL_R:
            PLLCFGR |= enablePLL_R;
            return true;

        case SysClock:
            return true;
        case HCLK:
            return true;
        case PCLK_1:
            return true;
        case PCLK_2:
            return true;
        default:
            return false;
        }
    }
    void disable(SystemClock source) {
        const uint32_t enableHSI16 = 0x100;
        const uint32_t enableHSE = 0x10000;
        const uint32_t enablePLL = 0x1000000;
        const uint32_t enablePLL_P = 0x10000;
        const uint32_t enablePLL_Q = 0x100000;
        const uint32_t enablePLL_R = 0x1000000;
        const uint32_t enableHSI48 = 0x1;
        const uint32_t enableLSI = 0x1;
        const uint32_t enableLSE = 0x1;
        switch (source) {
        case HSI:
            CR &= ~enableHSI16;
            return;
        case HSI48:
            CRRCR &= ~enableHSI48;
            return;
        case LSI:
            CSR &= ~enableLSI;
            return;
        case HSE:
            CR &= ~enableHSE;
            return;
        case LSE:
            BDCR &= ~enableLSE;
            return;
        case VCO:
            CR &= ~enablePLL;
            return;
        case PLL_P:
            PLLCFGR &= ~enablePLL_P;
            return;
        case PLL_Q:
            PLLCFGR &= ~enablePLL_Q;
            return;
        case PLL_R:
            PLLCFGR &= ~enablePLL_R;
            return;

        case SysClock:
            return;
        case HCLK:
            return;
        case PCLK_1:
            return;
        case PCLK_2:
            return;
        default:
            return;
        }
    }

    bool setPllRatio(uint8_t num, uint8_t div, SystemClock source) {
        const uint32_t pllSrcHSI16 = 0x2;
        const uint32_t pllSrcHSE = 0x3;

        uint32_t cfg = PLLCFGR;
        cfg &= ~0x7FF0;
        cfg |= num << 8;
        cfg |= (div - 1) << 4;
        if (source == HSI) {
            cfg |= pllSrcHSI16;
        } else if (source == HSE) {
            cfg |= pllSrcHSE;
        } else {
            return false;
        }
        PLLCFGR = cfg;
        return true;
    }

    void setPllPDiv(uint8_t div) {
        const uint32_t pllPDivMask = 0xF8000000;
        const uint32_t enablePLL_P = 0x10000;

        uint32_t cfg = PLLCFGR;
        cfg &= ~pllPDivMask;
        cfg |= div << 27;
        cfg |= enablePLL_P;
        PLLCFGR = cfg;
    }

    void setPllQDiv(uint8_t div) {
        const uint32_t pllQDivMask = 0x00600000;
        const uint32_t enablePLL_Q = 0x100000;

        uint32_t cfg = PLLCFGR;
        cfg &= ~pllQDivMask;
        cfg |= (div - 1) << 21;
        cfg |= enablePLL_Q;
        PLLCFGR = cfg;
    }

    void setPllRDiv(uint8_t div) {
        const uint32_t pllRDivMask = 0x06000000;
        const uint32_t enablePLL_R = 0x1000000;

        uint32_t cfg = PLLCFGR;
        cfg &= ~pllRDivMask;
        cfg |= enablePLL_R;
        cfg |= (div - 1) << 25;
        PLLCFGR = cfg;
    }

    void setHclkPresc(uint8_t pow) {
        const uint32_t hclkPrescalerMask = 0xF0;

        uint32_t cfgr = CFGR;
        cfgr &= ~hclkPrescalerMask;
        cfgr |= pow << 4;
        CFGR = cfgr;
    }

    void setPclk1Presc(uint8_t pow) {
        const uint32_t pclk1PrescalerMask = 0x700;

        uint32_t cfgr = CFGR;
        cfgr &= ~pclk1PrescalerMask;
        cfgr |= pow << 8;
        CFGR = cfgr;
    }
    void setPclk2Presc(uint8_t pow) {
        const uint32_t pclk2PrescalerMask = 0x3800;

        uint32_t cfgr = CFGR;
        cfgr &= ~pclk2PrescalerMask;
        cfgr |= pow << 11;
        CFGR = cfgr;
    }
};

#endif /* PERIPHERALS_CLOCK_CLOCK_LL_STM32G_SRC_MAIN_C___CLOCK_LL_STM32G_SPECIFIC_CLOCKREGISTERS_HPP_ */
