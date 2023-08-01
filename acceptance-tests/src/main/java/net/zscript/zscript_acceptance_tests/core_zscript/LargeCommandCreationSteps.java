package net.zscript.zscript_acceptance_tests.core_zscript;

import static net.zscript.zscript_acceptance_tests.acceptancetest_asserts.AcceptanceTestAssert.assertThatCommand;
import static org.assertj.core.api.Assertions.assertThat;

import java.nio.charset.StandardCharsets;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import io.cucumber.java.en.When;
import net.zscript.zscript_acceptance_tests.AcceptanceTestCapabilityResult;
import net.zscript.zscript_acceptance_tests.AcceptanceTestConnectionManager;

public class LargeCommandCreationSteps {

    @When("the target is sent an echo command with as much data as possible")
    public void the_target_is_sent_an_echo_command_with_as_much_data_as_possible() {
        int bigl = AcceptanceTestCapabilityResult.getMaxBigFieldLength();
        if (bigl == -1) {
            bigl = 1024;
        }
        byte[] bigArr       = new byte[bigl * 2];
        byte[] bigArrActual = new byte[bigl];
        int    arrOffset    = 0;
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

        int maxFieldCount = AcceptanceTestCapabilityResult.getMaxFieldNum() - 1;

        if (maxFieldCount == -1) {
            maxFieldCount = 255;
        }

        if (AcceptanceTestCapabilityResult.getMaxFieldSize() != -1) {
            int b = 0;
            for (char c = 'A'; c < Math.min('Z', 'A' + maxFieldCount); c++) {
                String current = "";
                for (int j = 0; j < AcceptanceTestCapabilityResult.getMaxFieldSize(); j++) {
                    String s = Integer.toHexString(Byte.toUnsignedInt((byte) (b++)));
                    if (s.length() == 1) {
                        current += '0';
                    }
                    current += s;
                }
                fields.put(c, current);
            }
        } else {
            int l   = 1;
            int b   = 0;
            int pos = 0;
            for (char c = 'A'; c < 'Y' && pos + l < maxFieldCount; c++) {
                String current = "";
                for (int j = 0; j < l * AcceptanceTestCapabilityResult.getFieldSizeUnit(); j++) {
                    String s = Integer.toHexString(Byte.toUnsignedInt((byte) (b++)));
                    if (s.length() == 1) {
                        current += '0';
                    }
                    current += s;
                }
                fields.put(c, current);
                pos += l;
                l++;
            }
            if (pos < maxFieldCount) {
                String current = "";
                for (int j = 0; j + pos < (maxFieldCount) * AcceptanceTestCapabilityResult.getFieldSizeUnit(); j++) {
                    String s = Integer.toHexString(Byte.toUnsignedInt((byte) (b++)));
                    if (s.length() == 1) {
                        current += '0';
                    }
                    current += s;
                }
                fields.put('Y', current);
            }
        }
        String fieldStr = "";
        for (Entry<Character, String> e : fields.entrySet()) {
            fieldStr += e.getKey().charValue() + e.getValue();
        }
        byte[] fieldArr = fieldStr.getBytes(StandardCharsets.UTF_8);
        byte[] full     = new byte[4 + fieldArr.length + bigl];
        full[0] = 'Z';
        full[1] = '1';
        System.arraycopy(fieldArr, 0, full, 2, fieldArr.length);
        full[fieldArr.length + 2] = '"';
        System.arraycopy(bigArr, 0, full, 3 + fieldArr.length, bigl);
        full[fieldArr.length + bigl + 2] = '"';
        full[fieldArr.length + bigl + 3] = '\n';
        CoreZscriptSteps.testAssert = assertThatCommand(AcceptanceTestConnectionManager.getConnections().get(0), full, 0);
        CoreZscriptSteps.testExpectedFields = a -> {
            a.worked().hasBigStringField(bigArrActual);
            for (Entry<Character, String> e : fields.entrySet()) {
                a.hasField(e.getKey(), e.getValue());
            }
            return a;
        };

    }

    @When("the target is sent a capability request command")
    public void the_target_is_sent_a_capability_request_command() throws InterruptedException, ExecutionException, TimeoutException {
        assertThatCommand(AcceptanceTestConnectionManager.getConnections().get(0), "Z2\n").worked().hasField('M').getField('M', b -> {
            AcceptanceTestCapabilityResult.setMaxPageNum(Byte.toUnsignedInt(b[0]));
        }).send().get(2, TimeUnit.SECONDS);
        String command = "";
        for (int i = 0; i < AcceptanceTestCapabilityResult.getMaxPageNum(); i++) {
            command += "Z2P" + i + "&";
        }
        command += "\n";
        CoreZscriptSteps.testAssert = assertThatCommand(AcceptanceTestConnectionManager.getConnections().get(0), command);
        CoreZscriptSteps.testExpectedFields = a -> {
            for (int i = 0; i < AcceptanceTestCapabilityResult.getMaxPageNum(); i++) {
                a = a.worked().hasBigField().getBigField(b -> {
                    AcceptanceTestCapabilityResult.addSupportedCodes(b);
                }).hasField('M', AcceptanceTestCapabilityResult.getMaxPageNum()).getField('C', c -> {
                    assertThat(c).describedAs("Multi-byte page numbers not allowed").hasSize(1);
                    assertThat(AcceptanceTestCapabilityResult.getGeneralCapabilities()).describedAs("Target must give consistent capabilities").isIn(-1,
                            Byte.toUnsignedInt(c[0]));
                    AcceptanceTestCapabilityResult.setGeneralCapabilities(Byte.toUnsignedInt(c[0]));
                }).getField('B', b -> {
                    long size = 0;
                    for (byte d : b) {
                        size <<= 8;
                        size += d;
                        assertThat(size).describedAs("Target gave too large a maximum big field length").isLessThan(Integer.MAX_VALUE);
                    }
                    assertThat(AcceptanceTestCapabilityResult.getMaxBigFieldLength()).describedAs("Target must give consistent capabilities").isIn(-1, size);
                    AcceptanceTestCapabilityResult.setMaxBigFieldLength((int) size);
                }).getField('N', n -> {
                    assertThat(n).describedAs("Multi-byte field numbers not allowed").hasSize(1);
                    assertThat(AcceptanceTestCapabilityResult.getMaxFieldNum()).describedAs("Target must give consistent capabilities").isIn(-1, Byte.toUnsignedInt(n[0]));
                    AcceptanceTestCapabilityResult.setMaxFieldNum(Byte.toUnsignedInt(n[0]));
                }).getField('F', f -> {
                    assertThat(f).describedAs("Multi-byte field sizes not allowed").hasSize(1);
                    assertThat(AcceptanceTestCapabilityResult.getMaxFieldSize()).describedAs("Target must give consistent capabilities").isIn(-1, Byte.toUnsignedInt(f[0]));
                    AcceptanceTestCapabilityResult.setMaxFieldSize(Byte.toUnsignedInt(f[0]));
                }).getField('P', n -> {
                    long size = 0;
                    for (byte d : n) {
                        assertThat(size).describedAs("Target gave too large a persistent memory size").isLessThan(Long.MAX_VALUE >>> 8);
                        size <<= 8;
                        size += d;
                    }
                    assertThat(AcceptanceTestCapabilityResult.getPersistentMemorySize()).describedAs("Target must give consistent capabilities").isIn((long) -1, size);
                    AcceptanceTestCapabilityResult.setPersistentMemorySize(size);
                });
                if (i < AcceptanceTestCapabilityResult.getMaxPageNum() - 1) {
                    a = a.sequenceContinues().next();
                }
            }
            return a;
        };
    }

    @When("the target is sent an echo command with too much field data")
    public void the_target_is_sent_an_echo_command_with_too_much_field_data() {
        Map<Character, String> fields = new HashMap<>();
        int                    fieldl = AcceptanceTestCapabilityResult.getMaxFieldNum() * 2;
        if (fieldl == -1) {
            fieldl = 255;
        }
        if (AcceptanceTestCapabilityResult.getMaxFieldSize() != -1) {
            int b = 0;
            for (int i = 0; i < 26; i++) {
                if (i != 18) {
                    String current = "";
                    for (int j = 0; j < AcceptanceTestCapabilityResult.getMaxFieldSize() * 2; j++) {
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
            int l   = 1;
            int b   = 0;
            int pos = 0;
            for (int i = 0; i < 25 && pos + l < fieldl; i++) {
                if (i != 18) {
                    String current = "";
                    for (int j = 0; j < l * AcceptanceTestCapabilityResult.getFieldSizeUnit(); j++) {
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
                for (int j = 0; j + pos < (fieldl) * AcceptanceTestCapabilityResult.getFieldSizeUnit(); j++) {
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
        CoreZscriptSteps.testAssert = assertThatCommand(AcceptanceTestConnectionManager.getConnections().get(0), "Z1" + fieldStr + "\n");
    }

    @When("the target is sent an echo command with too much big-field data")
    public void the_target_is_sent_an_echo_command_with_too_much_big_field_data() {
        int    bigl         = AcceptanceTestCapabilityResult.getMaxBigFieldLength() + 200;
        byte[] bigArr       = new byte[bigl * 2];
        byte[] bigArrActual = new byte[bigl];
        int    arrOffset    = 0;
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
        full[0] = 'Z';
        full[1] = '1';
        full[2] = '"';
        System.arraycopy(bigArr, 0, full, 3, bigl);
        full[bigl + 2] = '"';
        full[bigl + 3] = '\n';
        CoreZscriptSteps.testAssert = assertThatCommand(AcceptanceTestConnectionManager.getConnections().get(0), full, 0);
    }
}
