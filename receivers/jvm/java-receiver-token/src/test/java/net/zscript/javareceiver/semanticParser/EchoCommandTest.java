package net.zscript.javareceiver.semanticParser;

import static org.assertj.core.api.Assertions.assertThat;
import static org.junit.jupiter.params.provider.Arguments.of;

import java.io.IOException;
import java.util.stream.Stream;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import net.zscript.javareceiver.core.StringWriterOutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.modules.core.ZscriptCoreModule;
import net.zscript.javareceiver.tokenizer.TokenBuffer;
import net.zscript.javareceiver.tokenizer.TokenRingBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;

class EchoCommandTest {
    private final TokenBuffer buffer    = TokenRingBuffer.createBufferWithCapacity(256);
    private final Tokenizer   tokenizer = new Tokenizer(buffer.getTokenWriter(), 2);

    private final SemanticParser parser = new SemanticParser(buffer.getTokenReader(), new ExecutionActionFactory());

    private final Zscript zscript = new Zscript();

    private StringWriterOutStream outStream;
    private ParserActionTester    parserActionTester;

    @BeforeEach
    void setup() throws IOException {
        zscript.addModule(new ZscriptCoreModule());
        outStream = new StringWriterOutStream();
        parserActionTester = new ParserActionTester(zscript, buffer, tokenizer, parser, outStream);
    }

    @Test
    public void shouldSimplyEcho() {
        parserActionTester.parseSnippet("Z1\n", "!S\n");
        assertThat(outStream.isOpen()).isFalse();
    }

    @ParameterizedTest
    @MethodSource
    public void shouldEchoAllCases(final String text, String finalOutput) {
        parserActionTester.parseSnippet(text, finalOutput);
        assertThat(outStream.isOpen()).isFalse();
    }

    private static Stream<Arguments> shouldEchoAllCases() {
        return Stream.of(
                of("Z1A\n", "!AS\n"),
                of("Z1C3B2A1\n", "!C3B2A1S\n"),
                of("Z1ABfCffDfffEffff\n", "!ABfCffDfffEffffS\n"),
                of("Z1Afffff\n", "!10S20\n"), // TODO is this good?
                of("Z1A000000B0000001C00000ffff\n", "!AB1CffffS\n"),
                of("Z1A\"hello\"\n", "!A\"hello\"S\n"),
                of("Z1A\"hµllo\"\n", "!A\"hµllo\"S\n"),
                of("Z1A\"he=0allo\"\n", "!A\"he=0allo\"S\n"),
                of("Z1A\"he=0all=6f\"\n", "!A\"he=0allo\"S\n"),
                of("Z1A\"he=0all=6f\"\n", "!A\"he=0allo\"S\n"),
                of("Z1A\"1=0a2=003=3d=224\"\n", "!A\"1=0a2=003=3d=224\"S\n"),
                of("Z1+123456\n", "!+123456S\n"),
                of("Z1+123456789abcdef0\n", "!+123456789abcdef0S\n"),
                of("Z1AS2\n", "!AS2\n"),
                of("Z1AS2 | Z1\n", "!AS2|S\n"),
                of("Z1AS13\n", "!AS13\n"),
                of("Z1AS13 | Z1\n", "!AS13\n"),

                of("Z1A1A2\n", "!S43\n"), // TODO verify this is standard, or whether we just pick first one
                of("Z1+12+34\n", "!+12+34S\n"),
                of("Z1+12\"ab\"+34\n", "!+12\"ab\"+34S\n")

        );
    }

}
