package net.zscript.javaclient.threading;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.function.BiConsumer;
import java.util.function.Consumer;

public class ZscriptCallbackThreadpool {
    private final ExecutorService callbackPool;

    public ZscriptCallbackThreadpool() {
        this.callbackPool = Executors.newCachedThreadPool();
    }

    public ZscriptCallbackThreadpool(ExecutorService callbackPool) {
        this.callbackPool = callbackPool;
    }

    public <T> void sendCallback(Consumer<T> callback, T content, Consumer<Exception> exceptionhandler) {
        callbackPool.submit(() -> {
            try {
                callback.accept(content);
            } catch (Exception e) {
                exceptionhandler.accept(e);
            }
        });
    }

    public <T, U> void sendCallback(BiConsumer<T, U> callback, T content1, U content2, Consumer<Exception> exceptionhandler) {
        callbackPool.submit(() -> {
            try {
                callback.accept(content1, content2);
            } catch (Exception e) {
                exceptionhandler.accept(e);
            }
        });
    }

    public <T extends Exception> void sendCallback(Consumer<T> callback, T content) {
        callbackPool.submit(() -> callback.accept(content));
    }

    public void sendCallback(Runnable callback, Consumer<Exception> exceptionhandler) {
        callbackPool.submit(() -> {
            try {
                callback.run();
            } catch (Exception e) {
                exceptionhandler.accept(e);
            }
        });
    }
}
