package net.zscript.javareceiver.testing;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

class CollectingConsumerTest {
    @Test
    public void shouldCaptureAndDispenseItem() {
        CollectingConsumer<String> rc = new CollectingConsumer<>();
        rc.accept("hello");
        assertThat(rc.next()).isPresent().get().isEqualTo("hello");
        assertThat(rc.next()).isNotPresent();
    }

    @Test
    public void shouldCaptureAndDispenseSeveralItemsAsAQueue() {
        CollectingConsumer<String> rc = new CollectingConsumer<>();
        rc.accept("foo");
        rc.accept("bar");
        rc.accept("spam");
        assertThat(rc.next()).isPresent().get().isEqualTo("foo");
        assertThat(rc.next()).isPresent().get().isEqualTo("bar");
        assertThat(rc.next()).isPresent().get().isEqualTo("spam");
        assertThat(rc.next()).isNotPresent();

        rc.accept("eggs");
        assertThat(rc.next()).isPresent().get().isEqualTo("eggs");
        assertThat(rc.next()).isNotPresent();
    }
}
