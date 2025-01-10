package net.zscript.zscript_acceptance_tests.acceptancetest_asserts;

import java.util.List;

public interface AcceptanceTestCondition {
    public void test(AcceptanceTestResponseSequence seq, List<Character> prevUsed);
}
