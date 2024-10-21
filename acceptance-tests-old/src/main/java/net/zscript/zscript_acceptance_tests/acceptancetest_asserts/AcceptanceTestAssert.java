package net.zscript.zscript_acceptance_tests.acceptancetest_asserts;

import java.io.ByteArrayInputStream;
import java.io.PushbackInputStream;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

public class AcceptanceTestAssert extends AcceptanceTestMessageAssert {
    private final AcceptanceTestConnection connection;
    private final byte[]                   toSend;
    private final AcceptanceFuture         future = new AcceptanceFuture();
    private       int                      otherMessagesInFlight;

    public AcceptanceTestAssert(AcceptanceTestConnection connection, byte[] toSend, int otherMessagesInFlight) {
        this.connection = connection;
        this.otherMessagesInFlight = otherMessagesInFlight;
        this.toSend = toSend;
    }

    public static AcceptanceTestAssert assertThatCommand(AcceptanceTestConnection connection, byte[] toSend, int otherMessagesInFlight) {
        return new AcceptanceTestAssert(connection, toSend, otherMessagesInFlight);
    }

    public static AcceptanceTestAssert assertThatCommand(AcceptanceTestConnection connection, String toSend) {
        return new AcceptanceTestAssert(connection, toSend.getBytes(StandardCharsets.UTF_8), 0);
    }

    @Override
    public Future<?> send() {
        connection.registerListener(b -> acceptResponse(b));
        connection.send(toSend);
        return future;
    }

    private void acceptResponse(byte[] resp) {
        try {
            PushbackInputStream            in      = new PushbackInputStream(new ByteArrayInputStream(resp));
            AcceptanceTestResponseSequence seq     = new AcceptanceTestResponseSequence();
            int                            c       = in.read();
            String                         current = "";
            while (c != -1) {
                in.unread(c);
                AcceptanceTestResponse parsedResp = new AcceptanceTestResponse();
                current = parsedResp.parseCommand(in, seq, current);
                if (parsedResp.getStatus() != AcceptanceTestResponseStatus.SKIP_COMMAND) {
                    seq.addResponse(parsedResp);
                }
                c = in.read();
            }
            List<Character> charsDone = new ArrayList<>();
            for (AcceptanceTestCondition condition : conditions) {
                condition.test(seq, charsDone);
            }
            future.complete();
        } catch (Throwable t) {
            if (otherMessagesInFlight == 0) {
                future.fail(t);
            } else {
                otherMessagesInFlight--;
            }
        }
    }

    private class AcceptanceFuture implements Future<Void> {
        private final ReentrantLock lock     = new ReentrantLock();
        private final Condition     complete = lock.newCondition();
        private       Throwable     failure  = null;
        private       boolean       comp     = false;

        public void complete() {
            lock.lock();
            try {
                comp = true;
                complete.signalAll();
            } finally {
                lock.unlock();
            }
        }

        public void fail(Throwable t) {
            lock.lock();
            try {
                failure = t;
                comp = true;
                complete.signalAll();
            } finally {
                lock.unlock();
            }
        }

        @Override
        public boolean cancel(boolean mayInterruptIfRunning) {
            return false;
        }

        @Override
        public boolean isCancelled() {
            return false;
        }

        @Override
        public boolean isDone() {
            lock.lock();
            try {
                return comp;
            } finally {
                lock.unlock();
            }
        }

        @Override
        public Void get() throws InterruptedException, ExecutionException {
            lock.lock();
            try {
                if (comp) {
                    if (failure != null) {
                        throw new ExecutionException(failure);
                    } else {
                        return null;
                    }
                } else {
                    complete.await();
                    if (failure != null) {
                        throw new ExecutionException(failure);
                    } else {
                        return null;
                    }
                }
            } finally {
                lock.unlock();
            }
        }

        @Override
        public Void get(long timeout, TimeUnit unit) throws InterruptedException, ExecutionException, TimeoutException {
            lock.lock();
            try {
                if (comp) {
                    if (failure != null) {
                        throw new ExecutionException(failure);
                    } else {
                        return null;
                    }
                } else {
                    if (!complete.await(timeout, unit)) {
                        throw new TimeoutException();
                    }
                    if (failure != null) {
                        throw new ExecutionException(failure);
                    } else {
                        return null;
                    }
                }
            } finally {
                lock.unlock();
            }
        }

    }
}
