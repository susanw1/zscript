package org.zcode.javareceiver.semanticParser;

import static org.assertj.core.api.Assertions.assertThat;

import java.io.IOException;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.zcode.javareceiver.core.StringWriterOutStream;
import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeCommandOutStream;
import org.zcode.javareceiver.execution.ZcodeAddressingContext;
import org.zcode.javareceiver.execution.ZcodeCommandContext;
import org.zcode.javareceiver.modules.ZcodeModule;
import org.zcode.javareceiver.modules.core.ZcodeCoreModule;
import org.zcode.javareceiver.semanticParser.ContextView.AsyncActionNotifier;
import org.zcode.javareceiver.semanticParser.SemanticParser.State;
import org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType;
import org.zcode.javareceiver.tokenizer.OptIterator;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenRingBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

class SemanticParserAsyncCommandTest {
    private final ZcodeTokenBuffer buffer    = ZcodeTokenRingBuffer.createBufferWithCapacity(256);
    private final ZcodeTokenizer   tokenizer = new ZcodeTokenizer(buffer.getTokenWriter(), 2);

    private final SemanticParser parser = new SemanticParser(buffer.getTokenReader(), new ExecutionActionFactory());

    private final Zcode zcode = new Zcode();

    private StringWriterOutStream outStream;
    private ParserActionTester    parserActionTester;
    private AsyncTestModule       newModule;

    @BeforeEach
    void setup() throws IOException {
        zcode.addModule(new ZcodeCoreModule());

        newModule = new AsyncTestModule();
        zcode.addModule(newModule);

        outStream = new StringWriterOutStream();
        parserActionTester = new ParserActionTester(zcode, buffer, tokenizer, parser, outStream);
    }

    static class AsyncTestModule implements ZcodeModule {
        private final AsyncCommand c2        = new AsyncCommand();
        private volatile int       someValue = 0;

        private AsyncActionNotifier notifier;

        @Override
        public int getModuleID() {
            return 0xf;
        }

        @Override
        public void execute(ZcodeCommandContext ctx, int command) {
            switch (command) {
            case 2:
                // picked 2 at random. this is command 0xf2
                c2.execute(ctx);
            }
        }

        @Override
        public void moveAlong(ZcodeCommandContext ctx, int command) {
            switch (command) {
            case 2:
                c2.moveAlong(ctx);
            }
        }

        /**
         * For testing, use 2nd part of address to match someValue if it's <10. Otherwise, don't go async at all.
         */
        @Override
        public void address(ZcodeAddressingContext ctx) {
            OptIterator<Integer> s = ctx.getAddressSegments();
            s.next(); // throw away module-lookup key
            if (s.next().get() < 10) {
                ctx.commandNotComplete();
                notifier = ctx.getAsyncActionNotifier();
            }
        }

        @Override
        public void addressMoveAlong(ZcodeAddressingContext ctx) {
            OptIterator<Integer> s = ctx.getAddressSegments();
            s.next(); // throw away module-lookup key
            if (s.next().get() != someValue) {
                ctx.commandNotComplete();
            }
        }

        public void increment() {
            someValue++;
            notifier.notifyNeedsAction();
        }

        /**
         * Test command with async behaviour: supply 'E' ("end") value - if 0, just writes B=0; otherwise requires attention every time its value is incremented. When value reaches
         * the 'E' value, if even, it writes that value as 'C'; if odd, returns a fail status.
         */
        class AsyncCommand {

            public void execute(ZcodeCommandContext ctx) {
                ZcodeCommandOutStream out = ctx.getOutStream();
                out.writeField('A', 0xff);

                int endOnValue = ctx.getField((byte) 'E', 1);
                if (endOnValue == 0) {
                    ctx.getOutStream().writeField('B', 0);
                } else {
                    notifier = ctx.getAsyncActionNotifier();
                    ctx.commandNotComplete();
                }
            }

            public void moveAlong(ZcodeCommandContext ctx) {
                int endOnValue = ctx.getField((byte) 'E', 1);

                if (someValue < endOnValue) {
                    ctx.commandNotComplete();
                } else if (someValue == endOnValue) {
                    if (endOnValue % 2 == 0) {
                        ctx.getOutStream().writeField('C', endOnValue);
                    } else {
                        ctx.status((byte) 1);
                    }
                }
            }

        }

    }

    @Test
    public void shouldRunAsyncCommand() {
        assertThat(outStream.isOpen()).isFalse();
        parserActionTester.parseSnippet("Z2 & Zf2 ", ActionType.RUN_FIRST_COMMAND, State.COMMAND_COMPLETE, "!AS");
        assertThat(outStream.isOpen()).isTrue();
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.COMMAND_COMPLETE_NEEDS_TOKENS, "!AS");

        parserActionTester.parseSnippet("\n", ActionType.RUN_COMMAND, State.COMMAND_INCOMPLETE, "!AS&Aff");
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_ASYNC, State.COMMAND_INCOMPLETE, "!AS&Aff");

        // simulate async interaction with the command's state
        newModule.increment();

        parserActionTester.parseSnippet("", ActionType.COMMAND_MOVEALONG, State.COMMAND_FAILED, "!AS&AffS1");
        parserActionTester.parseSnippet("", ActionType.END_SEQUENCE, State.PRESEQUENCE, "!AS&AffS1\n");
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!AS&AffS1\n");
    }

    @Test
    public void shouldRunMultiplePasses() {
        assertThat(outStream.isOpen()).isFalse();
        parserActionTester.parseSnippet("Z2 & Zf2 E2 ", ActionType.RUN_FIRST_COMMAND, State.COMMAND_COMPLETE, "!AS");
        assertThat(outStream.isOpen()).isTrue();
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.COMMAND_COMPLETE_NEEDS_TOKENS, "!AS");

        parserActionTester.parseSnippet("\n", ActionType.RUN_COMMAND, State.COMMAND_INCOMPLETE, "!AS&Aff");
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_ASYNC, State.COMMAND_INCOMPLETE, "!AS&Aff");

        // simulate async interaction with the command's state
        newModule.increment();
        assertThat(parser.getState()).isEqualTo(State.COMMAND_NEEDS_ACTION);

        parserActionTester.parseSnippet("", ActionType.COMMAND_MOVEALONG, State.COMMAND_INCOMPLETE, "!AS&Aff");
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_ASYNC, State.COMMAND_INCOMPLETE, "!AS&Aff");

        newModule.increment();

        parserActionTester.parseSnippet("", ActionType.COMMAND_MOVEALONG, State.COMMAND_COMPLETE, "!AS&AffC2S");
        parserActionTester.parseSnippet("", ActionType.END_SEQUENCE, State.PRESEQUENCE, "!AS&AffC2S\n");
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!AS&AffC2S\n");
    }

    @Test
    public void shouldRunMultiplePassesWithErrorMidflight() {
        assertThat(outStream.isOpen()).isFalse();

        parserActionTester.parseSnippet("Z2 & Zf2 E2 &", ActionType.RUN_FIRST_COMMAND, State.COMMAND_COMPLETE, "!AS");
        assertThat(outStream.isOpen()).isTrue();
        parserActionTester.parseSnippet("", ActionType.RUN_COMMAND, State.COMMAND_INCOMPLETE, "!AS&Aff");
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_ASYNC, State.COMMAND_INCOMPLETE, "!AS&Aff");

        tokenizer.dataLost();

        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_ASYNC, State.COMMAND_INCOMPLETE, "!AS&Aff");
        parserActionTester.parseSnippet("\n", ActionType.WAIT_FOR_ASYNC, State.COMMAND_INCOMPLETE, "!AS&Aff");
        assertThat(outStream.isOpen()).isTrue();

        // simulate async interaction with the command's state
        newModule.increment();
        assertThat(parser.getState()).isEqualTo(State.COMMAND_NEEDS_ACTION);

        parserActionTester.parseSnippet("", ActionType.COMMAND_MOVEALONG, State.COMMAND_INCOMPLETE, "!AS&Aff");
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_ASYNC, State.COMMAND_INCOMPLETE, "!AS&Aff");
        assertThat(outStream.isOpen()).isTrue();

        newModule.increment();

        parserActionTester.parseSnippet("", ActionType.COMMAND_MOVEALONG, State.COMMAND_COMPLETE, "!AS&AffC2S");
        parserActionTester.parseSnippet("", ActionType.ERROR, State.PRESEQUENCE, "!AS&AffC2S!10S10\n");
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!AS&AffC2S!10S10\n");
    }

    @Test
    public void shouldRunMultiplePassesInAddressing() {
        assertThat(outStream.isOpen()).isFalse();

        parser.activate();
        parserActionTester.parseSnippet("@f.2 Z1A\n", ActionType.INVOKE_ADDRESSING, State.ADDRESSING_INCOMPLETE, "");
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_ASYNC, State.ADDRESSING_INCOMPLETE, "");

        assertThat(outStream.isOpen()).isFalse();

        // simulate async interaction with the command's state
        newModule.increment();
        assertThat(parser.getState()).isEqualTo(State.ADDRESSING_NEEDS_ACTION);

        parserActionTester.parseSnippet("", ActionType.ADDRESSING_MOVEALONG, State.ADDRESSING_INCOMPLETE, "");
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_ASYNC, State.ADDRESSING_INCOMPLETE, "");

        newModule.increment();
        assertThat(parser.getState()).isEqualTo(State.ADDRESSING_NEEDS_ACTION);
        parserActionTester.parseSnippet("", ActionType.ADDRESSING_MOVEALONG, State.ADDRESSING_COMPLETE, "");
        parserActionTester.parseSnippet("", ActionType.END_SEQUENCE, State.PRESEQUENCE, "");

        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "");
    }

}
