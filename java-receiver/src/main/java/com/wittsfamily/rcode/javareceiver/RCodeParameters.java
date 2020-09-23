package com.wittsfamily.rcode.javareceiver;

public class RCodeParameters {
    public final int bigFieldStandardLength = 32;
    public final int bigBigFieldLength = 1024;
    public final int fieldNum = 10;
    public final boolean cacheFieldResults = true;
    public final boolean isUsingInterruptVector;
    public final boolean findInterruptSourceAddress = true;
    public final int commandNum = 10;
    public final int interruptStoreNum = 5;
    public final int interruptVectorWorkingNum = 5;
    public final int interruptVectorNum = 5;
    public final int executionLength = 100;
    public final int executionOutBufferSize = 64;
    public final int executionInNum = 6;
    public final int executionOutNum = 5;
    public final int maxParallelRunning = 3;
    public final int slotNum = 1;
    public final int lockNum = 20;

    public final int highestBasicLockNum = 10;
    public final int lowestRwLockNum = 20;
    public final int highestRwLockNum = 25;
    public final boolean hasMultiByteCommands = false;
    public final int debugBufferLength = 25;

    public RCodeParameters(boolean isUsingInterruptVector) {
        this.isUsingInterruptVector = isUsingInterruptVector;
    }
}
