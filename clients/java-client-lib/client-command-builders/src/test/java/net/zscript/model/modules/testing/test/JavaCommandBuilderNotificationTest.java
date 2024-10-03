package net.zscript.model.modules.testing.test;

import java.util.List;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

import net.zscript.client.modules.test.testing.TestingModule;
import net.zscript.javaclient.addressing.CompleteAddressedResponse;
import net.zscript.javaclient.commandbuilder.ZscriptResponse;
import net.zscript.javaclient.commandbuilder.notifications.NotificationSection;
import net.zscript.javaclient.tokens.ExtendingTokenBuffer;
import net.zscript.tokenizer.TokenBuffer.TokenReader;
import net.zscript.tokenizer.Tokenizer;

public class JavaCommandBuilderNotificationTest {
    final ExtendingTokenBuffer buffer      = new ExtendingTokenBuffer();
    final Tokenizer            tokenizer   = new Tokenizer(buffer.getTokenWriter(), 2);
    final TokenReader          tokenReader = buffer.getTokenReader();

    @Test
    void shouldDefineNotificationClasses() {
        TestingModule.TestNtfANotificationId ntfIdA = TestingModule.TestNtfANotificationId.get();
        assertThat(ntfIdA.getHandleType()).isEqualTo(TestingModule.TestNtfANotificationHandle.class);

        TestingModule.TestNtfANotificationHandle handleA   = ntfIdA.newHandle();
        List<NotificationSection<?>>             sectionsA = handleA.getSections();
        assertThat(sectionsA).hasSize(1);
        assertThat(sectionsA).hasExactlyElementsOfTypes(TestingModule.Expr1NotificationSection.class);

        TestingModule.TestNtfBNotificationHandle handleB   = TestingModule.TestNtfBNotificationId.get().newHandle();
        List<NotificationSection<?>>             sectionsB = handleB.getSections();
        assertThat(sectionsB).hasSize(2);
        assertThat(sectionsB).hasExactlyElementsOfTypes(TestingModule.Expr1NotificationSection.class, TestingModule.Expr2NotificationSection.class);

        assertThat(sectionsB.get(0).getResponseType()).isEqualTo(TestingModule.Expr1NotificationSectionContent.class);
    }

    @Test
    void shouldCreateNotificationWithRequiredFields() {
        "!234 Dab Lcd & Xef\n".chars().forEach(c -> tokenizer.accept((byte) c));

        final TestingModule.TestNtfBNotificationHandle handle = TestingModule.TestNtfBNotificationId.get().newHandle();

        final CompleteAddressedResponse car = CompleteAddressedResponse.parse(tokenReader);
        assertThat(car.asResponse().hasAddress()).isFalse();
        assertThat(car.getContent().getResponseValue()).isEqualTo(0x234);

        final List<ZscriptResponse> sections = handle.buildNotificationContent(car.getContent().getExecutionPath());
        assertThat(sections)
                .hasExactlyElementsOfTypes(TestingModule.Expr1NotificationSectionContent.class, TestingModule.Expr2NotificationSectionContent.class);

        final TestingModule.Expr1NotificationSectionContent sec0 = (TestingModule.Expr1NotificationSectionContent) sections.get(0);
        final TestingModule.Expr2NotificationSectionContent sec1 = (TestingModule.Expr2NotificationSectionContent) sections.get(1);

        assertThat(sec0.getTestNtfARespDField1()).isEqualTo(0xab);
        assertThat(sec0.getTestNtfARespLField2()).isEqualTo(0xcd);

        assertThat(sec1.getTestNtfBRespXField1()).isEqualTo(0xef);
        assertThat(sec1.getTestNtfBRespYField2AsString()).isEqualTo("");

        assertThat(sec0.getField((byte) 'D')).as("field 'D'").hasValue(0xab);
        assertThat(sec0.getField((byte) 'L')).as("field 'L'").hasValue(0xcd);
        assertThat(sec1.getField((byte) 'X')).as("field 'X'").hasValue(0xef);
    }
}
