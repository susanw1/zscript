package net.zscript.zscript_acceptance_tests.acceptancetest_asserts;

import java.util.concurrent.Future;

public class ZcodeAcceptanceTestAssertIntermediateEach extends ZcodeAcceptanceTestMessageAssert {
    private final ZcodeAcceptanceTestMessageAssert parent;

    public ZcodeAcceptanceTestAssertIntermediateEach(ZcodeAcceptanceTestMessageAssert parent) {
        this.parent = parent;
    }

    @Override
    public Future<?> send() {
        return parent.send();
    }

    public ZcodeAcceptanceTestCondition getAsCondition() {
        return (seq, prev) -> {
            for (ZcodeAcceptanceTestCondition condition : conditions) {
                condition.test(seq, prev);
            }
        };
    }

}
