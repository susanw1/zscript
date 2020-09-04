package com.wittsfamily.rcode_acceptance_tests.acceptancetest_asserts;

import java.util.List;

public interface RCodeAcceptanceTestCondition {
    public void test(RCodeAcceptanceTestResponseSequence seq, List<Character> prevUsed);
}
