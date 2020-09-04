package com.wittsfamily.rcode_acceptance_tests.acceptancetest_asserts;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.Future;
import java.util.function.Consumer;

public abstract class RCodeAcceptanceTestMessageAssert {
    final List<RCodeAcceptanceTestCondition> conditions = new ArrayList<>();

    public RCodeAcceptanceTestMessageAssert worked() {
        return hasStatus(RCodeAcceptanceTestResponseStatus.OK);
    }

    public RCodeAcceptanceTestMessageAssert printSequence() {
        conditions.add((seq, prev) -> {
            for (RCodeAcceptanceTestResponse resp : seq.getResps()) {
                System.out.println(resp.getFields().asString() + resp.getBigField().asString());
            }
            System.out.println();
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert printCommand() {
        conditions.add((seq, prev) -> {
            System.out.println(seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString());
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert anyOf(Consumer<RCodeAcceptanceTestAssertIntermediateOr> options) {
        RCodeAcceptanceTestAssertIntermediateOr or = new RCodeAcceptanceTestAssertIntermediateOr(this);
        options.accept(or);
        conditions.add(or.getAsCondition());
        return this;
    }

    public RCodeAcceptanceTestMessageAssert eachOf(Consumer<RCodeAcceptanceTestAssertIntermediateEach> options) {
        RCodeAcceptanceTestAssertIntermediateEach and = new RCodeAcceptanceTestAssertIntermediateEach(this);
        options.accept(and);
        conditions.add(and.getAsCondition());
        return this;
    }

    public RCodeAcceptanceTestMessageAssert hasStatus(RCodeAcceptanceTestResponseStatus r) {
        conditions.add((seq, prev) -> {
            prev.add('S');
            if (seq.peekFirst().getFields().getFieldLength('S') != 1) {
                if (seq.peekFirst().getFields().getFieldLength('S') < 1) {
                    String str = seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString();
                    throw new AssertionError("\nResponse received contained no S field, where S" + Integer.toHexString(r.getValue()) + " was expected.\nInstead received:\n" + str);
                } else {
                    String str = seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString();
                    throw new AssertionError(
                            "\nResponse received contained multi-byte S field, where S" + Integer.toHexString(r.getValue()) + " was expected.\nInstead received:\n" + str);
                }
            }
            byte b = seq.peekFirst().getFields().getField('S')[0];
            if (b != r.getValue()) {
                String str = seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString();
                RCodeAcceptanceTestResponseStatus actual = null;
                for (RCodeAcceptanceTestResponseStatus v : RCodeAcceptanceTestResponseStatus.values()) {
                    if (v.getValue() == b) {
                        actual = v;
                        break;
                    }
                }
                String actStr = "Unknown status";
                if (actual != null) {
                    actStr = actual.name().toLowerCase();
                }
                throw new AssertionError("\nResponse received contained S" + Integer.toHexString(Byte.toUnsignedInt(b)) + " (" + actStr + "), where S"
                        + Integer.toHexString(r.getValue()) + " (" + r.name().toLowerCase() + ") was expected.\nReceived:\n" + str);
            }
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert isNotification() {
        conditions.add((seq, prev) -> {
            if (!seq.isNotification()) {
                String str = seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString();
                throw new AssertionError("\nResponse received not notification, when notification expected.\nReceived (without !/*):\n" + str);
            }
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert isBroadcast() {
        conditions.add((seq, prev) -> {
            if (!seq.isBroadcast()) {
                String str = seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString();
                throw new AssertionError("\nResponse received not broadcast, when broadcast expected.\nReceived (without !/*):\n" + str);
            }
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert isNotNotification() {
        conditions.add((seq, prev) -> {
            if (seq.isNotification()) {
                String str = seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString();
                throw new AssertionError("\nResponse is notification, when non-notification expected.\nReceived (without !/*):\n" + str);
            }
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert isNotBroadcast() {
        conditions.add((seq, prev) -> {
            if (seq.isBroadcast()) {
                String str = seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString();
                throw new AssertionError("\nResponse is broadcast, when non-broadcast expected.\nReceived (without !/*):\n" + str);
            }
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert hasField(char f) {
        conditions.add((seq, prev) -> {
            prev.add(f);
            if (seq.peekFirst().getFields().getFieldLength(f) == 0) {
                String str = seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString();
                throw new AssertionError("\nResponse received contained no " + f + " field.\nInstead received:\n" + str);
            }
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert getField(char f, Consumer<byte[]> target) {
        conditions.add((seq, prev) -> {
            prev.add(f);
            if (seq.peekFirst().getFields().getFieldLength(f) != 0) {
                target.accept(seq.peekFirst().getFields().getField(f));
            }
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert hasField(char f, int value) {
        conditions.add((seq, prev) -> {
            prev.add(f);
            if (seq.peekFirst().getFields().getFieldLength(f) != 1) {
                if (seq.peekFirst().getFields().getFieldLength(f) < 1) {
                    String str = seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString();
                    throw new AssertionError(
                            "\nResponse received contained no " + f + " field, where " + f + Integer.toHexString(value) + " was expected.\nInstead received:\n" + str);
                } else {
                    String str = seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString();
                    throw new AssertionError(
                            "\nResponse received contained multi-byte " + f + " field, where " + f + Integer.toHexString(value) + " was expected.\nInstead received:\n" + str);
                }
            }
            byte b = seq.peekFirst().getFields().getField(f)[0];
            if (b != value) {
                String str = seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString();
                throw new AssertionError("\nResponse received contained " + f + Integer.toHexString(Byte.toUnsignedInt(b)) + ", where " + f + Integer.toHexString(value)
                        + " was expected.\nInstead received:\n" + str);
            }
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert hasField(char f, byte[] value) {
        conditions.add((seq, prev) -> {
            prev.add(f);
            if (seq.peekFirst().getFields().getFieldLength(f) == 0) {
                String str = seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString();
                throw new AssertionError("\nResponse received contained no " + f + " field, where " + f + value + " was expected.\nInstead received:\n" + str);
            }
            byte[] actual = seq.peekFirst().getFields().getField(f);
            if (!Arrays.equals(value, actual)) {
                String str = seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString();
                String str1 = seq.peekFirst().getFields().asString(f);
                RCodeAcceptanceTestFieldMap map = new RCodeAcceptanceTestFieldMap();
                for (byte b : value) {
                    map.addField(f, b);
                }
                throw new AssertionError("\nResponse received contained " + str1 + ", where " + map.asString() + " was expected.\nInstead received:\n" + str);
            }
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert hasField(char f, String value) {
        int startOffset = 0;
        while (startOffset < value.length() && value.charAt(startOffset) == '0')
            startOffset++;
        boolean oddStart = false;
        if ((value.length() - startOffset) % 2 == 1) {
            startOffset--;
            oddStart = true;
        }
        byte[] data = new byte[(value.length() - startOffset + 1) / 2];
        int datapos = 0;
        for (int i = startOffset; i < value.length(); i += 2) {
            if (i != startOffset || !oddStart) {
                if (value.charAt(i) >= '0' && value.charAt(i) <= '9') {
                    data[datapos] = (byte) ((value.charAt(i) - '0') << 4);
                } else {
                    data[datapos] = (byte) ((value.charAt(i) - 'a' + 10) << 4);
                }
            }
            if (value.charAt(i + 1) >= '0' && value.charAt(i + 1) <= '9') {
                data[datapos] |= (byte) (value.charAt(i + 1) - '0');
            } else {
                data[datapos] |= (byte) (value.charAt(i + 1) - 'a' + 10);
            }
            datapos++;
        }
        if (data.length == 0) {
            return hasField(f, (byte) 0);
        }
        return hasField(f, data);
    }

    public RCodeAcceptanceTestMessageAssert hasBigField() {
        conditions.add((seq, prev) -> {
            if (seq.peekFirst().getBigField().getField().isEmpty()) {
                throw new AssertionError(
                        "\nResponse received contained no big field.\nReceived:\n" + seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString());
            }
            if (seq.peekFirst().getBigField().isString()) {
                prev.add('"');
            } else {
                prev.add('+');
            }
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert getBigField(Consumer<byte[]> target) {
        conditions.add((seq, prev) -> {
            if (seq.peekFirst().getBigField().getField().size() != 0) {
                byte[] array = new byte[seq.peekFirst().getBigField().getField().size()];
                int i = 0;
                for (byte b : seq.peekFirst().getBigField().getField()) {
                    array[i++] = b;
                }
                target.accept(array);
            }
            if (seq.peekFirst().getBigField().isString()) {
                prev.add('"');
            } else {
                prev.add('+');
            }
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert hasBigField(byte[] value) {
        conditions.add((seq, prev) -> {
            if (seq.peekFirst().getBigField().isString()) {
                prev.add('"');
            } else {
                prev.add('+');
            }
            getBigFieldCondition(value).test(seq, prev);
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert hasBigDataField() {
        conditions.add((seq, prev) -> {
            if (seq.peekFirst().getBigField().getField().isEmpty()) {
                throw new AssertionError(
                        "\nResponse received contained no big field.\nReceived:\n" + seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString());
            }
            if (seq.peekFirst().getBigField().isString()) {
                throw new AssertionError("\nResponse received contained string big field, not data big field.\nActual:\n" + seq.peekFirst().getBigField().asString()
                        + "\nReceived:\n" + seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString());
            }
            prev.add('+');
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert hasBigDataField(String value) {
        if (value.length() % 2 != 0) {
            throw new IllegalArgumentException("Big data fields must have even lengths");
        }
        byte[] data = new byte[value.length() / 2];
        for (int i = 0; i < value.length(); i += 2) {
            if (value.charAt(i) >= '0' && value.charAt(i) <= '9') {
                data[i / 2] = (byte) ((value.charAt(i) - '0') << 4);
            } else {
                data[i / 2] = (byte) ((value.charAt(i) - 'a' + 10) << 4);
            }
            if (value.charAt(i + 1) >= '0' && value.charAt(i + 1) <= '9') {
                data[i / 2] |= (byte) (value.charAt(i + 1) - '0');
            } else {
                data[i / 2] |= (byte) (value.charAt(i + 1) - 'a' + 10);
            }
        }
        conditions.add((seq, prev) -> {
            if (seq.peekFirst().getBigField().isString() && !seq.peekFirst().getBigField().getField().isEmpty()) {
                throw new AssertionError("\nResponse received contained string big field, not data big field.\nExpected:\n+" + value + "\nActual:\n"
                        + seq.peekFirst().getBigField().asString() + "\nReceived:\n" + seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString());
            }
            prev.add('+');
            getBigFieldCondition(data).test(seq, prev);
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert hasBigDataField(byte[] value) {
        conditions.add((seq, prev) -> {
            if (seq.peekFirst().getBigField().isString() && !seq.peekFirst().getBigField().getField().isEmpty()) {
                String failStr = "";
                for (byte b : value) {
                    String str1 = Integer.toHexString(Byte.toUnsignedInt(b));
                    if (str1.length() == 1) {
                        str1 = "0" + str1;
                    }
                    failStr += str1;
                }
                throw new AssertionError("\nResponse received contained string big field, not data big field.\nExpected:\n+" + failStr + "\nActual:\n"
                        + seq.peekFirst().getBigField().asString() + "\nReceived:\n" + seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString());
            }
            prev.add('+');
            getBigFieldCondition(value).test(seq, prev);
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert hasBigStringField() {
        conditions.add((seq, prev) -> {
            if (seq.peekFirst().getBigField().getField().isEmpty()) {
                throw new AssertionError(
                        "\nResponse received contained no big field.\nReceived:\n" + seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString());
            }
            if (!seq.peekFirst().getBigField().isString()) {
                throw new AssertionError("\nResponse received contained data big field, not string big field.\nActual:\n" + seq.peekFirst().getBigField().asString()
                        + "\nReceived:\n" + seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString());
            }
            prev.add('"');
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert hasBigStringField(String value) {
        byte[] data = value.getBytes(StandardCharsets.UTF_8);
        conditions.add((seq, prev) -> {
            if (!seq.peekFirst().getBigField().isString() && !seq.peekFirst().getBigField().getField().isEmpty()) {
                throw new AssertionError("\nResponse received contained data big field, not string big field.\nExpected:\n\"" + value + "\"\nActual:\n"
                        + seq.peekFirst().getBigField().asString() + "\nReceived:\n" + seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString());
            }
            prev.add('"');
            getBigFieldCondition(data).test(seq, prev);
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert hasBigStringField(byte[] value) {
        conditions.add((seq, prev) -> {
            if (!seq.peekFirst().getBigField().isString() && !seq.peekFirst().getBigField().getField().isEmpty()) {
                String str = StandardCharsets.UTF_8.decode(ByteBuffer.wrap(value)).toString();
                throw new AssertionError("\nResponse received contained data big field, not string big field.\nExpected:\n\"" + str + "\"\nActual:\n"
                        + seq.peekFirst().getBigField().asString() + "\nReceived:\n" + seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString());
            }
            prev.add('"');
            getBigFieldCondition(value).test(seq, prev);
        });
        return this;
    }

    private RCodeAcceptanceTestCondition getBigFieldCondition(byte[] value) {
        return (seq, prev) -> {
            if (seq.peekFirst().getBigField().getField().isEmpty()) {
                RCodeAcceptanceTestBigField expectedField = new RCodeAcceptanceTestBigField();
                for (byte b : value) {
                    expectedField.addByte(b);
                }
                expectedField.setIsString(false);
                throw new AssertionError("\nResponse received contained no big field.\nExpected:\n" + expectedField.asString() + "\nReceived:\n"
                        + seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString());
            }
            List<Byte> dataReceived = seq.peekFirst().getBigField().getField();
            if (dataReceived.size() != value.length) {
                RCodeAcceptanceTestBigField expectedField = new RCodeAcceptanceTestBigField();
                for (byte b : value) {
                    expectedField.addByte(b);
                }
                expectedField.setIsString(seq.peekFirst().getBigField().isString());
                String s = "";
                for (byte b : seq.peekFirst().getBigField().getField()) {
                    s += Integer.toHexString(Byte.toUnsignedInt(b)) + " ";
                }
                String s2 = "";
                for (byte b : value) {
                    s2 += Integer.toHexString(Byte.toUnsignedInt(b)) + " ";
                }
                throw new AssertionError("\nReceived big field of different length to expected.\nExpected:\n" + expectedField.asString() + "\n(" + s2 + ")\nActual:\n"
                        + seq.peekFirst().getBigField().asString() + "\n(" + s + ")\nReceived:\n" + seq.peekFirst().getFields().asString()
                        + seq.peekFirst().getBigField().asString());
            }
            boolean matches = true;
            for (int i = 0; i < dataReceived.size(); i++) {
                if (dataReceived.get(i) != value[i]) {
                    matches = false;
                    break;
                }
            }
            if (!matches) {
                RCodeAcceptanceTestBigField expectedField = new RCodeAcceptanceTestBigField();
                for (byte b : value) {
                    expectedField.addByte(b);
                }
                expectedField.setIsString(seq.peekFirst().getBigField().isString());
                throw new AssertionError("\nReceived big field different to expected.\nExpected:\n" + expectedField.asString() + "\nActual:\n"
                        + seq.peekFirst().getBigField().asString() + "\nReceived:\n" + seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString());
            }
        };
    }

    public RCodeAcceptanceTestMessageAssert hasNoBigField() {
        conditions.add((seq, prev) -> {
            if (!seq.peekFirst().getBigField().getField().isEmpty()) {
                throw new AssertionError("\nResponse received contained big field, when none should have been present.\nBig Field:\n" + seq.peekFirst().getBigField().asString()
                        + "\nReceived:\n" + seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString());
            }
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert isLastInSequence() {
        conditions.add((seq, prev) -> {
            if (seq.peekFirst().getEnd() != '\n') {
                throw new AssertionError("\nResponse received was terminated with" + seq.peekFirst().getEnd() + "when \\n expected.\nReceived:\n"
                        + seq.peekFirst().getFields().asString() + seq.peekFirst().getBigField().asString());
            }
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert sequenceContinues() {
        conditions.add((seq, prev) -> {
            if (seq.peekFirst().getEnd() != ';') {
                if (seq.peekFirst().getEnd() == '\n') {
                    throw new AssertionError("\nResponse received was terminated with \\n when ; expected.");
                } else {
                    throw new AssertionError("\nResponse received was terminated with " + seq.peekFirst().getEnd() + " when ; expected.");
                }
            }
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert next() {
        conditions.add((seq, prev) -> {
            prev.clear();
            seq.pollFirst();
            if (seq.peekFirst() == null) {
                throw new AssertionError("\nResponse received ends when more is expected");
            }
        });
        return this;
    }

    public RCodeAcceptanceTestMessageAssert hasNoOtherFields() {
        conditions.add((seq, prev) -> {
            for (char i = 'A'; i <= 'Z'; i++) {
                if (seq.peekFirst().getFields().hasField(i) && !prev.contains(i)) {
                    throw new AssertionError("\nResponse received contains field not tested for: " + i + "\nReceived:\n" + seq.peekFirst().getFields().asString()
                            + seq.peekFirst().getBigField().asString());
                }
            }
            if (!seq.peekFirst().getBigField().getField().isEmpty()) {
                if (seq.peekFirst().getBigField().isString() && !prev.contains('"')) {
                    throw new AssertionError("\nResponse received contains big string field not tested for" + "\nReceived:\n" + seq.peekFirst().getFields().asString()
                            + seq.peekFirst().getBigField().asString());
                } else if (!prev.contains('+')) {
                    throw new AssertionError("\nResponse received contains big data field not tested for" + "\nReceived:\n" + seq.peekFirst().getFields().asString()
                            + seq.peekFirst().getBigField().asString());
                }
            }
        });
        return this;
    }

    public abstract Future<?> send();
}
