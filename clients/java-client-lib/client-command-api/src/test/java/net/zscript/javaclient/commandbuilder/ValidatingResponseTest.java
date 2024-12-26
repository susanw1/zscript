package net.zscript.javaclient.commandbuilder;

import java.util.List;
import java.util.Map;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.commandPaths.ZscriptFieldSet;
import net.zscript.tokenizer.ZscriptExpression;

public class ValidatingResponseTest {

    @Test
    public void shouldValidateExpression() {
        ZscriptExpression fieldSet = ZscriptFieldSet.fromMap(List.of(new byte[] { 'a', 'b' }, new byte[] { 'c', 'd', 'e' }), List.of(false, true),
                Map.of((byte) 'A', 12, (byte) 'Z', 7, (byte) 'C', 0, (byte) 'D', 0x123));

        ValidatingResponse vr1 = new ValidatingResponse(fieldSet, new byte[] { 'A', 'D' });
        assertThat(vr1.expression()).isSameAs(fieldSet);
        assertThat(vr1.isValid()).isTrue();
        assertThat(vr1.getField((byte) 'A')).isPresent().hasValue(0xc);

        ValidatingResponse vr2 = new ValidatingResponse(fieldSet, new byte[] { 'B', 'D' });
        assertThat(vr2.isValid()).isFalse();
        assertThat(vr2.getField((byte) 'B')).isNotPresent();
    }
}
