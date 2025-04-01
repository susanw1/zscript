package net.zscript.javaclient.commandbuilderapi;

import java.util.List;

import static net.zscript.javaclient.commandpaths.FieldElement.fieldOf;
import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.commandpaths.BigField;
import net.zscript.javaclient.commandpaths.ZscriptFieldSet;
import net.zscript.tokenizer.ZscriptExpression;

public class ValidatingResponseTest {

    @Test
    public void shouldValidateExpression() {
        ZscriptExpression fieldSet = ZscriptFieldSet.fromList(
                List.of(new BigField(new byte[] { 'a', 'b' }, false), new BigField(new byte[] { 'c', 'd', 'e' }, true)),
                List.of(fieldOf((byte) 'A', 12), fieldOf((byte) 'Z', 7), fieldOf((byte) 'C', 0), fieldOf((byte) 'D', 0x132))
                //                Map.of((byte) 'A', 12, (byte) 'Z', 7, (byte) 'C', 0, (byte) 'D', 0x123)
        );

        ValidatingResponse vr1 = new ValidatingResponse(fieldSet, new byte[] { 'A', 'D' });
        assertThat(vr1.expression()).isSameAs(fieldSet);
        assertThat(vr1.isValid()).isTrue();
        assertThat(vr1.getField((byte) 'A')).isPresent().hasValue(0xc);

        ValidatingResponse vr2 = new ValidatingResponse(fieldSet, new byte[] { 'B', 'D' });
        assertThat(vr2.isValid()).isFalse();
        assertThat(vr2.getField((byte) 'B')).isNotPresent();
    }
}
