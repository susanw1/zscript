package net.zscript.javaclient.commandbuilderapi;

import java.util.List;

import static net.zscript.javaclient.commandpaths.FieldElement.fieldOf;
import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.commandpaths.ZscriptFieldSet;
import net.zscript.tokenizer.ZscriptExpression;

public class ValidatingResponseTest {

    @Test
    public void shouldValidateExpression() {
        ZscriptExpression fieldSet = ZscriptFieldSet.fromList(
                List.of(fieldOf((byte) 'A', 12), fieldOf((byte) 'Z', 7), fieldOf((byte) 'C', 0), fieldOf((byte) 'D', 0x132))
        );

        ValidatingResponse vr1 = new ValidatingResponse(fieldSet, new byte[] { 'A', 'D' });
        assertThat(vr1.expression()).isSameAs(fieldSet);
        assertThat(vr1.isValid()).isTrue();
        assertThat(vr1.getFieldValue('A')).isPresent().hasValue(0xc);

        ValidatingResponse vr2 = new ValidatingResponse(fieldSet, new byte[] { 'B', 'D' });
        assertThat(vr2.isValid()).isFalse();
        assertThat(vr2.getFieldValue('B')).isNotPresent();
    }
}
