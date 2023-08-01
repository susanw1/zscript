package net.zscript.zscript_acceptance_tests.acceptancetest_asserts;

import java.util.concurrent.Future;

public class AcceptanceTestAssertIntermediateEach extends AcceptanceTestMessageAssert {
    private final AcceptanceTestMessageAssert parent;

    public AcceptanceTestAssertIntermediateEach(AcceptanceTestMessageAssert parent) {
        this.parent = parent;
    }

    @Override
    public Future<?> send() {
        return parent.send();
    }

    public AcceptanceTestCondition getAsCondition() {
        return (seq, prev) -> {
            for (AcceptanceTestCondition condition : conditions) {
                condition.test(seq, prev);
            }
        };
    }

}
