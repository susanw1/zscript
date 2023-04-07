package org.zcode.javaclient.components;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

public class ZcodeExceptionTest {
    @Test
    void shouldCreateExceptionWithMessage() {
        Exception ex = new TestException("testing");
        assertThat(ex.getMessage()).isEqualTo("testing");
    }

    @Test
    void shouldCreateExceptionWithArgs() {
        Exception ex = new TestException("testing", "val1", 12, "val2", true, "val3", null);
        assertThat(ex.getMessage()).isEqualTo("testing [val1=12, val2=true, val3=null]");
        assertThat(ex.getCause()).isNull();
    }

    @Test
    void shouldCreateExceptionWithArgsAndCause() {
        Exception ex = new TestException("testing", new RuntimeException(), "val1", 12);
        assertThat(ex.getMessage()).isEqualTo("testing [val1=12]");
        assertThat(ex.getCause()).isInstanceOf(RuntimeException.class);
    }

    class TestException extends ZcodeException {
        public TestException(String message, Object... args) {
            super(message, args);
        }

        public TestException(String message, Throwable cause, Object... args) {
            super(message, cause, args);
        }
    }
}
