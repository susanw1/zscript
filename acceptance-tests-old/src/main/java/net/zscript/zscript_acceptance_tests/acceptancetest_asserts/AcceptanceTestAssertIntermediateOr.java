package net.zscript.zscript_acceptance_tests.acceptancetest_asserts;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.Future;

public class AcceptanceTestAssertIntermediateOr extends AcceptanceTestMessageAssert {
    private final AcceptanceTestMessageAssert parent;

    public AcceptanceTestAssertIntermediateOr(AcceptanceTestMessageAssert parent) {
        this.parent = parent;
    }

    @Override
    public Future<?> send() {
        return parent.send();
    }

    public AcceptanceTestCondition getAsCondition() {
        return (seq, prev) -> {
            List<AssertionError> errors = new ArrayList<>();
            boolean worked = false;
            for (AcceptanceTestCondition condition : conditions) {
                AcceptanceTestResponseSequence seqclone = new AcceptanceTestResponseSequence();
                for (AcceptanceTestResponse resp : seq.getResps()) {
                    AcceptanceTestResponse respClone = new AcceptanceTestResponse();
                    List<Byte> data = resp.getBigField().getField();
                    for (byte b : data) {
                        respClone.getBigField().addByte(b);
                    }
                    respClone.getBigField().setIsString(resp.getBigField().isString());
                    for (char i = 'A'; i <= 'Z'; i++) {
                        if (resp.getFields().hasField(i)) {
                            resp.getFields().addField(i, resp.getFields().getField(i));
                        }
                    }
                    seqclone.addResponse(respClone);
                }
                List<Character> prevclone = new ArrayList<>(prev);
                try {
                    condition.test(seqclone, prevclone);
                    worked = true;
                    break;
                } catch (AssertionError a) {
                    errors.add(a);
                }
            }
            if (!worked) {
                String message = "\nOr condition failed, as all options failed.\nFirst option gave{";
                for (Iterator<AssertionError> iterator = errors.iterator(); iterator.hasNext();) {
                    AssertionError error = iterator.next();
                    message += error.getMessage();
                    if (iterator.hasNext()) {
                        message += "\n}Next option gave:{";
                    }
                }
                message += "\n}";
                throw new AssertionError(message);
            }
        };
    }

}
