package org.zcode.javaclient.components;

import java.util.List;

import org.zcode.javaclient.components.Operation.Precondition;
import org.zcode.javaclient.parsing.Address;
import org.zcode.javaclient.parsing.Comment;

public abstract class ZcodeSequence<T extends Operation> {

    private final Address address;
    private final Comment comment;
    private final List<T> operations;

    protected ZcodeSequence(Address address, Comment comment, T... operations) {
        this.address = address;
        this.comment = comment;

        // sanity check for preconditions - the first must be unconditional, and the rest must not be.
        for (int i = 0; i < operations.length; i++) {
            if (i == 0 && operations[i].getPrecondition() != Precondition.UNCONDITIONAL) {
                throw new ZcodeValidationException("First operation in sequence must be unconditional", "operation", operations[i]);
            } else if (i > 0 && operations[i].getPrecondition() == Precondition.UNCONDITIONAL) {
                throw new ZcodeValidationException("Only first operation in sequence may be unconditional", "operation", operations[i]);
            }
        }
        this.operations = List.of(operations);
    }
}
