package net.zscript.javaclient.commandbuilder;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

class ZscriptClientExceptionTest {
    @Test
    public void shouldCreateSimpleMessages() {
        assertThat(new ZscriptClientException("testing").getMessage()).isEqualTo("testing");
    }

    @Test
    public void shouldCreateFormattedMessages() {
        assertThat(new ZscriptClientException("msg a=%s, b=%s", "1", 2).getMessage()).isEqualTo("msg a=1, b=2");
    }

    @Test
    public void shouldCreateSimpleMessagesForFieldExceptions() {
        assertThat(new ZscriptMissingFieldException("aaa").getMessage()).isEqualTo("aaa");
        assertThat(new ZscriptFieldOutOfRangeException("bbb").getMessage()).isEqualTo("bbb");
    }

    @Test
    public void shouldCreateFormattedMessagesForFieldExceptions() {
        assertThat(new ZscriptMissingFieldException("x a=%s, b=%s", 3, 4).getMessage()).isEqualTo("x a=3, b=4");
        assertThat(new ZscriptFieldOutOfRangeException("y a=%s, b=%s", 5, 6).getMessage()).isEqualTo("y a=5, b=6");
    }

    @Test
    public void shouldCreateSimpleMessagesForFieldExceptionsWithCause() {
        assertThat(new ZscriptMissingFieldException("aaa").getMessage()).isEqualTo("aaa");
        assertThat(new ZscriptFieldOutOfRangeException("bbb").getMessage()).isEqualTo("bbb");
    }

    @Test
    public void shouldCreateFormattedMessagesForFieldExceptionsWithCause() {
        RuntimeException             e   = new RuntimeException();
        final ZscriptClientException ex1 = new ZscriptClientException("x a=%s, b=%s", e, 'g', "asd");
        assertThat(ex1.getMessage()).isEqualTo("x a=g, b=asd");
        assertThat(ex1.getCause()).isSameAs(e);

        final ZscriptMissingFieldException ex2 = new ZscriptMissingFieldException("x a=%s, b=%s", e, 3, 4);
        assertThat(ex2.getMessage()).isEqualTo("x a=3, b=4");
        assertThat(ex2.getCause()).isSameAs(e);

        final ZscriptFieldOutOfRangeException ex3 = new ZscriptFieldOutOfRangeException("y a=%s, b=%s", e, 5, 6);
        assertThat(ex3.getMessage()).isEqualTo("y a=5, b=6");
        assertThat(ex3.getCause()).isSameAs(e);
    }

}
