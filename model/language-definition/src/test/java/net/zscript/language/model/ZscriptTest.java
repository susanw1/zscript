package net.zscript.language.model;

import static org.assertj.core.api.Assertions.assertThat;

import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.CharStreams;
import org.antlr.v4.runtime.CommonTokenStream;
import org.antlr.v4.runtime.Lexer;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import net.zscript.language.model.ZscriptParser.SequenceContext;

class ZscriptTest {

    @BeforeEach
    void setUp() throws Exception {

    }

    @Test
    void shouldParse() {
        CharStream        stream = CharStreams.fromString("Z023\n", "test source");
        Lexer             lexer  = new ZscriptLexer(stream);
        CommonTokenStream tokens = new CommonTokenStream(lexer);
        ZscriptParser     parser = new ZscriptParser(tokens);

        final SequenceContext r = parser.sequence();
        System.out.println(r.toStringTree());

        assertThat(r.toStringTree()).isEqualTo("([] ([36] ([111 36] ([129 111 36] Z 023))) \\n)");
    }

}
