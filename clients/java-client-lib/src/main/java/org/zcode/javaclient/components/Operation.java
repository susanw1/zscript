package org.zcode.javaclient.components;

import org.zcode.javaclient.parsing.BigField;
import org.zcode.javaclient.parsing.Params;

public abstract class Operation {
    private final Params       params;
    private final BigField     bigField;
    private final Precondition precondition;

    public enum Precondition {
        UNCONDITIONAL,
        AND_THEN,
        OR_ELSE
    }

    public Operation(Params params, BigField bigField) {
        this(params, bigField, Precondition.UNCONDITIONAL);
    }

    public Operation(Params params, BigField bigField, Precondition precondition) {
        this.params = params;
        this.bigField = bigField;
        this.precondition = precondition;
    }

    public BigField getBigField() {
        return bigField;
    }

    public Params getParams() {
        return params;
    }

    public Precondition getPrecondition() {
        return precondition;
    }
}
