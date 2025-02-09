package net.zscript.javaclient.devicenodes;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class ZscriptWorkerThread {
    private final ScheduledExecutorService exec = Executors.newSingleThreadScheduledExecutor();

    private final ZscriptCallbackThreadpool     threadpool;
    private final Thread                        execThread;
    private final List<WeakReference<Runnable>> timeoutChecks = new ArrayList<>();

    public ZscriptWorkerThread() {
        this(new ZscriptCallbackThreadpool());
    }

    public ZscriptWorkerThread(ZscriptCallbackThreadpool threadpool) {
        this.threadpool = threadpool;
        try {
            // identify our thread, so we can avoid resubmitting unnecessarily
            execThread = exec.submit(Thread::currentThread).get();
        } catch (ExecutionException | InterruptedException e) {
            throw new RuntimeException(e);
        }
        exec.scheduleAtFixedRate(this::checkTimeouts, 100, 10, TimeUnit.MILLISECONDS);
    }

    //TODO: Consider batching this to reduce thread latency if lots of devices are present
    private void checkTimeouts() {
        for (Iterator<WeakReference<Runnable>> iter = timeoutChecks.iterator(); iter.hasNext(); ) {
            Runnable node = iter.next().get();
            if (node == null) {
                iter.remove();
            } else {
                node.run();
            }
        }
    }

    public void addTimeoutCheck(Runnable r) {
        timeoutChecks.add(new WeakReference<>(r));
    }

    public <T> T moveOntoThread(Callable<T> task) {
        if (Thread.currentThread() == execThread) {
            try {
                return task.call();
            } catch (RuntimeException e) {
                throw e;
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        } else {
            try {
                return exec.submit(task).get();
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            } catch (ExecutionException e) {
                if (e.getCause() instanceof RuntimeException) {
                    throw (RuntimeException) e.getCause();
                } else {
                    throw new RuntimeException(e.getCause());
                }
            }
        }
    }

    public void moveOntoThread(Runnable task) {
        if (Thread.currentThread() == execThread) {
            try {
                task.run();
                return;
            } catch (RuntimeException e) {
                throw e;
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        } else {
            try {
                exec.submit(task).get();
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            } catch (ExecutionException e) {
                if (e.getCause() instanceof RuntimeException) {
                    throw (RuntimeException) e.getCause();
                } else {
                    throw new RuntimeException(e.getCause());
                }
            }
        }
    }

    public Future<?> startOnThread(Runnable task) {
        if (Thread.currentThread() == execThread) {
            try {
                task.run();
                return CompletableFuture.completedFuture(null);
            } catch (Exception e) {
                return CompletableFuture.failedFuture(e);
            }
        } else {
            return exec.submit(task);
        }

    }

    public <T> Future<T> startOnThread(Callable<T> task) {
        if (Thread.currentThread() == execThread) {
            try {
                return CompletableFuture.completedFuture(task.call());
            } catch (Exception e) {
                return CompletableFuture.failedFuture(e);
            }
        } else {
            return exec.submit(task);
        }
    }

    public ZscriptCallbackThreadpool getCallbackPool() {
        return threadpool;
    }
}
