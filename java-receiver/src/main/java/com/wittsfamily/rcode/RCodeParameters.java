package com.wittsfamily.rcode;

public class RCodeParameters {
    public final int bigFieldStandardLength = 32;
    public final int bigBigFieldLength = 1024;
    public final int fieldNum = 10;
    public final boolean cacheFieldResults = true;
    public final boolean isUsingInterruptVector = true;
    public final boolean findInterruptSourceAddress = true;
    public final int commandNum = 10;
    public final int interruptStoreNum = 5;
    public final int interruptVectorWorkingNum = 5;
    public final int executionOutBufferSize = 64;
    public final int executionInNum = 6;
    public final int executionOutNum = 5;
    public final int maxParallelRunning = 3;
    public final int slotNum = 20;
    public final int lockNum = 20;

    public final int highestBasicLockNum = 10;
    public final int lowestRwLockNum = 20;
    public final int highestRwLockNum = 25;
}
