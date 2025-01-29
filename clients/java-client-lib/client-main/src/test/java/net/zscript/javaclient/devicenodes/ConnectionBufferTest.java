package net.zscript.javaclient.devicenodes;

import org.assertj.core.api.Assertions;
import org.junit.jupiter.api.Test;

class ConnectionBufferTest {
    @Test
    public void should() {

        Connection       connection = null;
        EchoAssigner     b          = null;
        int              c          = 0;
        ConnectionBuffer buffer     = new ConnectionBuffer(connection, b, c);

        Assertions.assertThat(0).isEqualTo(0);
    }

    static class MutableTimeSource implements ConnectionBuffer.TimeSource {
        long t = 0;

        @Override
        public long nanoTime() {
            return t;
        }
    }

}
