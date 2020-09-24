package com.wittsfamily.rcode_acceptance_tests.core_rcode;

import static com.wittsfamily.rcode_acceptance_tests.acceptancetest_asserts.RCodeAcceptanceTestAssert.assertThatCommand;
import static org.assertj.core.api.Assertions.assertThat;

import java.nio.charset.StandardCharsets;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import com.wittsfamily.rcode_acceptance_tests.RCodeAcceptanceTestCapabilityResult;
import com.wittsfamily.rcode_acceptance_tests.RCodeAcceptanceTestConnectionManager;

import io.cucumber.java.en.When;

public class LargeCommandCreationSteps {

    @When("the target is sent an echo command with as much data as possible")
    public void the_target_is_sent_an_echo_command_with_as_much_data_as_possible() {
        int bigl = RCodeAcceptanceTestCapabilityResult.getMaxBigFieldLength();
        if (bigl == -1) {
            bigl = 1024;
        }
        byte[] bigArr = new byte[bigl * 2];
        byte[] bigArrActual = new byte[bigl];
        int arrOffset = 0;
        for (int i = 0; i < bigl; i++) {
            if (((byte) i) == '\n') {
                bigArr[i + arrOffset] = '\\';
                bigArr[i + 1 + arrOffset] = 'n';
                arrOffset++;
            } else if (((byte) i) == '"') {
                bigArr[i + arrOffset] = '\\';
                bigArr[i + 1 + arrOffset] = '"';
                arrOffset++;
            } else if (((byte) i) == '\\') {
                bigArr[i + arrOffset] = '\\';
                bigArr[i + 1 + arrOffset] = '\\';
                arrOffset++;
            } else {
                bigArr[i + arrOffset] = (byte) i;
            }
            bigArrActual[i] = (byte) i;
        }
        bigl += arrOffset + 1;
        Map<Character, String> fields = new HashMap<>();
        int fieldl = RCodeAcceptanceTestCapabilityResult.getMaxFieldNum() - 1;
        if (fieldl == -1) {
            fieldl = 255;
        }
        if (RCodeAcceptanceTestCapabilityResult.getMaxFieldSize() != -1) {
            int b = 0;
            for (int i = 0; i < Math.min(26, fieldl); i++) {
                if (i != 18) {
                    String current = "";
                    for (int j = 0; j < RCodeAcceptanceTestCapabilityResult.getMaxFieldSize(); j++) {
                        String s = Integer.toHexString(Byte.toUnsignedInt((byte) (b++)));
                        if (s.length() == 1) {
                            current += '0';
                        }
                        current += s;
                    }
                    fields.put((char) ('A' + i), current);
                }
            }
        } else {
            int l = 1;
            int b = 0;
            int pos = 0;
            for (int i = 0; i < 25 && pos + l < fieldl; i++) {
                if (i != 18) {
                    String current = "";
                    for (int j = 0; j < l * RCodeAcceptanceTestCapabilityResult.getFieldSizeUnit(); j++) {
                        String s = Integer.toHexString(Byte.toUnsignedInt((byte) (b++)));
                        if (s.length() == 1) {
                            current += '0';
                        }
                        current += s;
                    }
                    fields.put((char) ('A' + i), current);
                    pos += l;
                    l++;
                }
            }
            if (pos < fieldl) {
                String current = "";
                for (int j = 0; j + pos < (fieldl) * RCodeAcceptanceTestCapabilityResult.getFieldSizeUnit(); j++) {
                    String s = Integer.toHexString(Byte.toUnsignedInt((byte) (b++)));
                    if (s.length() == 1) {
                        current += '0';
                    }
                    current += s;
                }
                fields.put('Z', current);
            }
        }
        String fieldStr = "";
        for (Entry<Character, String> e : fields.entrySet()) {
            fieldStr += e.getKey().charValue() + e.getValue();
        }
        byte[] fieldArr = fieldStr.getBytes(StandardCharsets.UTF_8);
        byte[] full = new byte[4 + fieldArr.length + bigl];
        full[0] = 'R';
        full[1] = '1';
        System.arraycopy(fieldArr, 0, full, 2, fieldArr.length);
        full[fieldArr.length + 2] = '"';
        System.arraycopy(bigArr, 0, full, 3 + fieldArr.length, bigl);
        full[fieldArr.length + bigl + 2] = '"';
        full[fieldArr.length + bigl + 3] = '\n';
        CoreRCodeSteps.testAssert = assertThatCommand(RCodeAcceptanceTestConnectionManager.getConnections().get(0), full, 0);
        CoreRCodeSteps.testExpectedFields = a -> {
            a.worked().hasBigStringField(bigArrActual);
            for (Entry<Character, String> e : fields.entrySet()) {
                a.hasField(e.getKey(), e.getValue());
            }
            return a;
        };

    }

    @When("the target is sent a capability request command")
    public void the_target_is_sent_a_capability_request_command() throws InterruptedException, ExecutionException, TimeoutException {
        assertThatCommand(RCodeAcceptanceTestConnectionManager.getConnections().get(0), "R2\n").worked().hasField('M').getField('M', b -> {
            RCodeAcceptanceTestCapabilityResult.setMaxPageNum(Byte.toUnsignedInt(b[0]));
        }).send().get(2, TimeUnit.SECONDS);
        String command = "";
        for (int i = 0; i < RCodeAcceptanceTestCapabilityResult.getMaxPageNum(); i++) {
            command += "R2P" + i + "&";
        }
        command += "\n";
        CoreRCodeSteps.testAssert = assertThatCommand(RCodeAcceptanceTestConnectionManager.getConnections().get(0), command);
        CoreRCodeSteps.testExpectedFields = a -> {
            for (int i = 0; i < RCodeAcceptanceTestCapabilityResult.getMaxPageNum(); i++) {
                a = a.worked().hasBigField().getBigField(b -> {
                    RCodeAcceptanceTestCapabilityResult.addSupportedCodes(b);
                }).hasField('M', RCodeAcceptanceTestCapabilityResult.getMaxPageNum()).getField('C', c -> {
                    assertThat(c).hasSize(1);
                    assertThat(RCodeAcceptanceTestCapabilityResult.getGeneralCapabilities()).isIn(-1, Byte.toUnsignedInt(c[0]));
                    RCodeAcceptanceTestCapabilityResult.setGeneralCapabilities(Byte.toUnsignedInt(c[0]));
                }).getField('B', b -> {
                    long size = 0;
                    for (byte d : b) {
                        size <<= 8;
                        size += d;
                        assertThat(size).isLessThan(Integer.MAX_VALUE);
                    }
                    assertThat(RCodeAcceptanceTestCapabilityResult.getMaxBigFieldLength()).isIn(-1, size);
                    RCodeAcceptanceTestCapabilityResult.setMaxBigFieldLength((int) size);
                }).getField('N', n -> {
                    assertThat(n).hasSize(1);
                    assertThat(RCodeAcceptanceTestCapabilityResult.getMaxFieldNum()).isIn(-1, Byte.toUnsignedInt(n[0]));
                    RCodeAcceptanceTestCapabilityResult.setMaxFieldNum(Byte.toUnsignedInt(n[0]));
                }).getField('F', f -> {
                    assertThat(f).hasSize(1);
                    assertThat(RCodeAcceptanceTestCapabilityResult.getMaxFieldSize()).isIn(-1, Byte.toUnsignedInt(f[0]));
                    RCodeAcceptanceTestCapabilityResult.setMaxFieldSize(Byte.toUnsignedInt(f[0]));
                }).getField('P', n -> {
                    long size = 0;
                    for (byte d : n) {
                        assertThat(size).isLessThan(Long.MAX_VALUE >>> 8);
                        size <<= 8;
                        size += d;
                    }
                    assertThat(RCodeAcceptanceTestCapabilityResult.getPersistantMemorySize()).isIn(-1, size);
                    RCodeAcceptanceTestCapabilityResult.setPersistantMemorySize(size);
                });
                if (i < RCodeAcceptanceTestCapabilityResult.getMaxPageNum() - 1) {
                    a = a.sequenceContinues().next();
                }
            }
            return a;
        };
    }

    @When("the target is sent an echo command with too much field data")
    public void the_target_is_sent_an_echo_command_with_too_much_field_data() {
        Map<Character, String> fields = new HashMap<>();
        int fieldl = RCodeAcceptanceTestCapabilityResult.getMaxFieldNum() * 2;
        if (fieldl == -1) {
            fieldl = 255;
        }
        if (RCodeAcceptanceTestCapabilityResult.getMaxFieldSize() != -1) {
            int b = 0;
            for (int i = 0; i < 26; i++) {
                if (i != 18) {
                    String current = "";
                    for (int j = 0; j < RCodeAcceptanceTestCapabilityResult.getMaxFieldSize() * 2; j++) {
                        String s = Integer.toHexString(Byte.toUnsignedInt((byte) (b++)));
                        if (s.length() == 1) {
                            current += '0';
                        }
                        current += s;
                    }
                    fields.put((char) ('A' + i), current);
                }
            }
        } else {
            int l = 1;
            int b = 0;
            int pos = 0;
            for (int i = 0; i < 25 && pos + l < fieldl; i++) {
                if (i != 18) {
                    String current = "";
                    for (int j = 0; j < l * RCodeAcceptanceTestCapabilityResult.getFieldSizeUnit(); j++) {
                        String s = Integer.toHexString(Byte.toUnsignedInt((byte) (b++)));
                        if (s.length() == 1) {
                            current += '0';
                        }
                        current += s;
                    }
                    fields.put((char) ('A' + i), current);
                    pos += l;
                    l++;
                }
            }
            if (pos < fieldl) {
                String current = "";
                for (int j = 0; j + pos < (fieldl) * RCodeAcceptanceTestCapabilityResult.getFieldSizeUnit(); j++) {
                    String s = Integer.toHexString(Byte.toUnsignedInt((byte) (b++)));
                    if (s.length() == 1) {
                        current += '0';
                    }
                    current += s;
                }
                fields.put('Z', current);
            }
        }
        String fieldStr = "";
        for (Entry<Character, String> e : fields.entrySet()) {
            fieldStr += e.getKey().charValue() + e.getValue();
        }
        CoreRCodeSteps.testAssert = assertThatCommand(RCodeAcceptanceTestConnectionManager.getConnections().get(0), "R1" + fieldStr + "\n");
    }

    @When("the target is sent an echo command with too much big-field data")
    public void the_target_is_sent_an_echo_command_with_too_much_big_field_data() {
        int bigl = RCodeAcceptanceTestCapabilityResult.getMaxBigFieldLength() * 2;
        byte[] bigArr = new byte[bigl * 2];
        byte[] bigArrActual = new byte[bigl];
        int arrOffset = 0;
        for (int i = 0; i < bigl; i++) {
            if (((byte) i) == '\n') {
                bigArr[i + arrOffset] = '\\';
                bigArr[i + 1 + arrOffset] = 'n';
                arrOffset++;
            } else if (((byte) i) == '"') {
                bigArr[i + arrOffset] = '\\';
                bigArr[i + 1 + arrOffset] = '"';
                arrOffset++;
            } else if (((byte) i) == '\\') {
                bigArr[i + arrOffset] = '\\';
                bigArr[i + 1 + arrOffset] = '\\';
                arrOffset++;
            } else {
                bigArr[i + arrOffset] = (byte) i;
            }
            bigArrActual[i] = (byte) i;
        }
        bigl += arrOffset + 1;
        byte[] full = new byte[4 + bigl];
        full[0] = 'R';
        full[1] = '1';
        full[2] = '"';
        System.arraycopy(bigArr, 0, full, 3, bigl);
        full[bigl + 2] = '"';
        full[bigl + 3] = '\n';
        CoreRCodeSteps.testAssert = assertThatCommand(RCodeAcceptanceTestConnectionManager.getConnections().get(0), full, 0);
    }
}