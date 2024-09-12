package net.zscript.javareceiver.notifications;

import java.io.IOException;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import net.zscript.javareceiver.core.LockSet;
import net.zscript.javareceiver.core.StringWriterOutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.execution.NotificationContext;
import net.zscript.javareceiver.modules.ZscriptModule;
import net.zscript.javareceiver.modules.core.ZscriptCoreModule;
import net.zscript.javareceiver.semanticParser.ContextView.AsyncActionNotifier;

public class NotificationTest {
    private final Zscript zscript = new Zscript();

    private StringWriterOutStream outStream;

    private volatile int someValue = 0;

    private volatile int                 lastID           = -1;
    private volatile boolean             lastIsAddressing = false;
    private volatile boolean             lastIsMoveAlong  = false;
    private volatile AsyncActionNotifier notifier         = null;

    private volatile String                    toWrite = "";
    private final    ZscriptNotificationSource source  = new ZscriptNotificationSource();

    class TestModule implements ZscriptModule {

        @Override
        public int getModuleId() {
            return 0xf;
        }

        @Override
        public void execute(CommandContext ctx, int command) {
        }

        @Override
        public void notification(NotificationContext ctx, boolean moveAlong) {
            lastID = ctx.getID();
            lastIsAddressing = ctx.isAddressing();
            lastIsMoveAlong = moveAlong;
            if (someValue != 0) {
                ctx.notificationNotComplete();
                someValue--;
            }
            notifier = ctx.getAsyncActionNotifier();
            ctx.getOutStream().asCommandOutStream().writeBigFieldQuoted(toWrite);
        }

    }

    @BeforeEach
    void setup() throws IOException {
        TestModule test = new TestModule();
        zscript.addModule(new ZscriptCoreModule());
        zscript.addModule(test);
        zscript.addNotificationSource(source);
        outStream = new StringWriterOutStream();
        zscript.setNotificationOutStream(outStream);
    }

    @Test
    void shouldProduceBasicNotification() {
        toWrite = "data";

        source.set(LockSet.allLocked(), 0xf0, false);

        for (int i = 0; i < 20; i++) {
            zscript.progress();
        }
        assertThat(outStream.getStringAndReset()).isEqualTo("!f\"data\"\n");
        assertThat(outStream.isOpen()).isFalse();
    }

    @Test
    void shouldProduceBasicAsyncNotification() {
        toWrite = "data";
        someValue = 1;

        source.set(LockSet.allLocked(), 0xf0, false);

        for (int i = 0; i < 20; i++) {
            zscript.progress();
        }
        assertThat(outStream.getStringAndReset()).isEqualTo("!f\"data\"");
        assertThat(outStream.isOpen()).isTrue();
        assertThat(lastID).isEqualTo(0xf0);
        assertThat(lastIsAddressing).isFalse();
        assertThat(lastIsMoveAlong).isFalse();

        toWrite = "MoreData";
        notifier.notifyNeedsAction();
        for (int i = 0; i < 20; i++) {
            zscript.progress();
        }
        assertThat(outStream.getStringAndReset()).isEqualTo("\"MoreData\"\n");
        assertThat(lastID).isEqualTo(0xf0);
        assertThat(lastIsAddressing).isFalse();
        assertThat(lastIsMoveAlong).isTrue();
        assertThat(outStream.isOpen()).isFalse();

    }

    @Test
    void shouldProduceBasicAsyncAddressingNotification() {
        toWrite = "data";
        someValue = 2;

        source.set(LockSet.allLocked(), 0xfd, true);

        for (int i = 0; i < 20; i++) {
            zscript.progress();
        }
        assertThat(outStream.getStringAndReset()).isEqualTo("!\"data\"");
        assertThat(lastID).isEqualTo(0xfd);
        assertThat(lastIsAddressing).isTrue();
        assertThat(lastIsMoveAlong).isFalse();
        assertThat(outStream.isOpen()).isTrue();

        toWrite = "MoreData";
        notifier.notifyNeedsAction();
        for (int i = 0; i < 20; i++) {
            zscript.progress();
        }
        assertThat(outStream.getStringAndReset()).isEqualTo("\"MoreData\"");
        assertThat(lastID).isEqualTo(0xfd);
        assertThat(lastIsAddressing).isTrue();
        assertThat(lastIsMoveAlong).isTrue();
        assertThat(outStream.isOpen()).isTrue();

        toWrite = "YetMoreData";
        notifier.notifyNeedsAction();
        for (int i = 0; i < 20; i++) {
            zscript.progress();
        }
        assertThat(outStream.getStringAndReset()).isEqualTo("\"YetMoreData\"\n");
        assertThat(lastID).isEqualTo(0xfd);
        assertThat(lastIsAddressing).isTrue();
        assertThat(lastIsMoveAlong).isTrue();
        assertThat(outStream.isOpen()).isFalse();

    }

}
