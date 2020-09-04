package com.wittsfamily.rcode_acceptance_tests.acceptancetest_asserts;

import java.util.concurrent.Future;

public class RCodeAcceptanceTestAssertIntermediateEach extends RCodeAcceptanceTestMessageAssert {
    private final RCodeAcceptanceTestMessageAssert parent;

    public RCodeAcceptanceTestAssertIntermediateEach(RCodeAcceptanceTestMessageAssert parent) {
        this.parent = parent;
    }

    @Override
    public Future<?> send() {
        return parent.send();
    }

    public RCodeAcceptanceTestCondition getAsCondition() {
        return (seq, prev) -> {
            for (RCodeAcceptanceTestCondition condition : conditions) {
                condition.test(seq, prev);
            }
        };
    }

}
