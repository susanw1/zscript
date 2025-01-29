package net.zscript.javaclient.devicenodes;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class EchoAssignerTest {
    EchoAssigner echoAssigner;

    @BeforeEach
    void setUp() {
        echoAssigner = new EchoAssigner();
    }

    @Test
    public void shouldGetEchoAndMoveAlong() {
        assertThat(echoAssigner.getEcho()).isEqualTo(0x100);
        assertThat(echoAssigner.getEcho()).isEqualTo(0x100);
        assertThat(echoAssigner.isReserved(0x100)).isFalse();
        assertThat(echoAssigner.getReservationCount()).isEqualTo(0);

        echoAssigner.moveEcho();
        assertThat(echoAssigner.isReserved(0x100)).isTrue();
        assertThat(echoAssigner.getReservationCount()).isEqualTo(1);
        assertThat(echoAssigner.getEcho()).isEqualTo(0x101);
    }

    @Test
    public void shouldUseWholeBlockUntilFullThenFail() {
        for (int i = 0; i < 0xff; i++) {
            assertThat(echoAssigner.getEcho()).isEqualTo(0x100 + i);
            echoAssigner.moveEcho();
        }
        assertThatThrownBy(() -> echoAssigner.moveEcho())
                .isInstanceOf(IllegalStateException.class).hasMessageContaining("Ran out of echo values to assign");
    }

    @Test
    public void shouldReleaseEchoValue() {
        for (int i = 0; i < 0x80; i++) {
            assertThat(echoAssigner.getEcho()).isEqualTo(0x100 + i);
            echoAssigner.moveEcho();
            assertThat(echoAssigner.isReserved(0x100 + i)).isTrue();
        }
        assertThat(echoAssigner.getReservationCount()).isEqualTo(128);

        for (int i = 0; i < 0x80; i += 2) {
            echoAssigner.responseArrivedNormal(0x100 + i);
            assertThat(echoAssigner.isReserved(0x100 + i)).isFalse();
        }
        assertThat(echoAssigner.getReservationCount()).isEqualTo(64);

        for (int i = 0; i < 0x80; i++) {
            assertThat(echoAssigner.getEcho()).isEqualTo(0x180 + i);
            echoAssigner.moveEcho();
        }
        assertThat(echoAssigner.getReservationCount()).isEqualTo(192);

        for (int i = 0; i < 0x7e; i += 2) {
            assertThat(echoAssigner.getEcho()).isEqualTo(0x100 + i);
            echoAssigner.moveEcho();
        }
        assertThat(echoAssigner.getReservationCount()).isEqualTo(255);
    }

    @Test
    public void shouldReserveManually() {
        echoAssigner.moveEcho();
        assertThat(echoAssigner.getReservationCount()).isEqualTo(1);
        echoAssigner.manualEchoUse(0x100);
        assertThat(echoAssigner.getReservationCount())
                .describedAs("no change in reservation count as already assigned").isEqualTo(1);

        echoAssigner.manualEchoUse(0x101);
        assertThat(echoAssigner.getReservationCount())
                .describedAs("change in reservation count because unassigned").isEqualTo(2);

        echoAssigner.manualEchoUse(0x103);
        assertThat(echoAssigner.getReservationCount())
                .describedAs("manually reserved just above next to make sure we skip it").isEqualTo(3);
        assertThat(echoAssigner.getEcho()).isEqualTo(0x102);

        echoAssigner.moveEcho();
        assertThat(echoAssigner.getEcho())
                .describedAs("normal moveEcho skipped manual one").isEqualTo(0x104);
    }

    @Test
    public void shouldReserveManuallyThenTrackAfterTimeouts() {
        setupTimeouts(0x20, 16);
        echoAssigner.manualEchoUse(0x200);
        assertThat(echoAssigner.isReserved(0x200)).isTrue();

        assertThat(echoAssigner.isReserved(0x100)).isTrue();
        echoAssigner.manualEchoUse(0x100);
        assertThat(echoAssigner.isReserved(0x100)).isTrue();
    }

    @Test
    public void shouldReleaseUnknownValue() {
        echoAssigner.responseArrivedNormal(0x200);
        assertThat(echoAssigner.getReservationCount()).isZero();
    }

    @Test
    public void shouldTimeout() {
        assertThat(echoAssigner.getTimeoutCount()).isZero();
        echoAssigner.timeout(0x100);
        assertThat(echoAssigner.getTimeoutCount()).isZero();
        echoAssigner.moveEcho();
        echoAssigner.timeout(0x100);
        assertThat(echoAssigner.getTimeoutCount()).isEqualTo(1);
        assertThat(echoAssigner.getReservationCount()).isEqualTo(1);
    }

    @Test
    public void shouldIdentifyReservations() {
        echoAssigner.moveEcho();
        assertThat(echoAssigner.isReserved(0x100)).isTrue();
        assertThat(echoAssigner.isReserved(0x4)).isFalse();
        assertThat(echoAssigner.isReserved(0x400)).isFalse();
    }

    @Test
    public void shouldAssignNewBlockOnManyTimeouts() {
        setupTimeouts(0x80, 15);

        assertThat(echoAssigner.getTimeoutCount()).isEqualTo(15);
        echoAssigner.timeout(0x10f);
        assertThat(echoAssigner.getTimeoutCount()).isZero();

        assertThat(echoAssigner.getEcho()).isEqualTo(0x200);
        echoAssigner.moveEcho();

        assertThat(echoAssigner.isReserved(0x104)).isTrue();
        assertThat(echoAssigner.isReserved(0x200)).isTrue();
    }

    @Test
    public void shouldReleaseReservationsFromPreviousBlock() {
        setupTimeouts(0x20, 16);
        assertThat(echoAssigner.getEcho()).isEqualTo(0x200);
        assertThat(echoAssigner.isReserved(0x100)).isTrue();
        echoAssigner.responseArrivedNormal(0x100);
        assertThat(echoAssigner.isReserved(0x100)).isFalse();
    }

    @Test
    public void shouldNotDecreaseReservationsOnUnknownValue() {
        assertThat(echoAssigner.getReservationCount()).isEqualTo(0);
        echoAssigner.moveEcho();
        assertThat(echoAssigner.isReserved(0x100)).isTrue();
        assertThat(echoAssigner.getReservationCount()).isEqualTo(1);
        echoAssigner.responseArrivedNormal(0x101);
        assertThat(echoAssigner.getReservationCount()).isEqualTo(1);

    }

    @Test
    public void shouldHandleUnmatchedReceive() {
        assertThat(echoAssigner.unmatchedReceive(0x100))
                .describedAs("unknown value, no change").isFalse();
        assertThat(echoAssigner.getReservationCount()).isEqualTo(0);
        assertThat(echoAssigner.getTimeoutCount()).isEqualTo(0);

        echoAssigner.moveEcho();
        assertThat(echoAssigner.getReservationCount()).isEqualTo(1);
        assertThat(echoAssigner.unmatchedReceive(0x100))
                .describedAs("reserved, ").isTrue();
        assertThat(echoAssigner.getReservationCount()).isEqualTo(0);
        assertThat(echoAssigner.getTimeoutCount()).isEqualTo(0);
    }

    @Test
    public void shouldHandleUnmatchedReceiveAfterTimeout() {
        setupTimeouts(0x20, 16);

        assertThat(echoAssigner.unmatchedReceive(0x100))
                .describedAs("reserved, but in previous block").isTrue();
        assertThat(echoAssigner.getReservationCount()).isEqualTo(0);
        assertThat(echoAssigner.getTimeoutCount()).isEqualTo(0);
    }

    @Test
    public void shouldHandleUnmatchedReceiveWithLargeValue() {
        assertThat(echoAssigner.unmatchedReceive(0x300))
                .describedAs("unreserved, and in no known block").isFalse();
    }

    private void setupTimeouts(int reservations, int timeouts) {
        int startValue = echoAssigner.getEcho();
        for (int i = 0; i < reservations; i++) {
            echoAssigner.moveEcho();
        }
        for (int i = 0; i < timeouts; i++) {
            echoAssigner.timeout(startValue + i);
        }
    }

}
