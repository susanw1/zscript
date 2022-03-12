package org.zcode.zcode_acceptance_tests.acceptancetest_asserts;

import java.util.List;

public interface ZcodeAcceptanceTestCondition {
    public void test(ZcodeAcceptanceTestResponseSequence seq, List<Character> prevUsed);
}
