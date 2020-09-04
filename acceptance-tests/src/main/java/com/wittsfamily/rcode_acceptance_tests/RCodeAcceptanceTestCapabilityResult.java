package com.wittsfamily.rcode_acceptance_tests;

public class RCodeAcceptanceTestCapabilityResult {
    private static byte[] supportedCodes = null;
    private static int generalCapabilities = -1;
    private static int maxBigFieldLength = -1;
    private static int maxFieldNum = -1;
    private static int maxFieldSize = -1;
    private static int fieldSizeUnit = 1;
    private static int maxPageNum = -1;
    private static long persistantMemorySize = -1;
    private static boolean hasBeenRead = false;

    public static void addSupportedCodes(byte[] supportedCodes) {
        if (RCodeAcceptanceTestCapabilityResult.supportedCodes == null) {
            RCodeAcceptanceTestCapabilityResult.supportedCodes = supportedCodes;
        } else {
            byte[] tmp = RCodeAcceptanceTestCapabilityResult.supportedCodes;
            RCodeAcceptanceTestCapabilityResult.supportedCodes = new byte[tmp.length + supportedCodes.length];
            System.arraycopy(tmp, 0, RCodeAcceptanceTestCapabilityResult.supportedCodes, 0, tmp.length);
            System.arraycopy(supportedCodes, 0, RCodeAcceptanceTestCapabilityResult.supportedCodes, tmp.length, supportedCodes.length);
        }
    }

    public static void setGeneralCapabilities(int generalCapabilities) {
        RCodeAcceptanceTestCapabilityResult.generalCapabilities = generalCapabilities;
    }

    public static void setMaxBigFieldLength(int maxBigFieldLength) {
        RCodeAcceptanceTestCapabilityResult.maxBigFieldLength = maxBigFieldLength;
    }

    public static void setMaxFieldNum(int maxFieldNum) {
        RCodeAcceptanceTestCapabilityResult.maxFieldNum = maxFieldNum;
    }

    public static void setMaxFieldSize(int maxFieldSize) {
        RCodeAcceptanceTestCapabilityResult.maxFieldSize = maxFieldSize;
    }

    public static void setFieldSizeUnit(int fieldSizeUnit) {
        RCodeAcceptanceTestCapabilityResult.fieldSizeUnit = fieldSizeUnit;
    }

    public static void setMaxPageNum(int maxPageNum) {
        RCodeAcceptanceTestCapabilityResult.maxPageNum = maxPageNum;
    }

    public static void setPersistantMemorySize(long persistantMemorySize) {
        RCodeAcceptanceTestCapabilityResult.persistantMemorySize = persistantMemorySize;
    }

    public static void setHasBeenRead() {
        RCodeAcceptanceTestCapabilityResult.hasBeenRead = true;
    }

    public static byte[] getSupportedCodes() {
        return supportedCodes;
    }

    public static int getGeneralCapabilities() {
        return generalCapabilities;
    }

    public static int getMaxBigFieldLength() {
        return maxBigFieldLength;
    }

    public static int getMaxFieldNum() {
        return maxFieldNum;
    }

    public static int getMaxFieldSize() {
        return maxFieldSize;
    }

    public static int getFieldSizeUnit() {
        return fieldSizeUnit;
    }

    public static int getMaxPageNum() {
        return maxPageNum;
    }

    public static long getPersistantMemorySize() {
        return persistantMemorySize;
    }

    public static boolean hasBeenRead() {
        return hasBeenRead;
    }

}
