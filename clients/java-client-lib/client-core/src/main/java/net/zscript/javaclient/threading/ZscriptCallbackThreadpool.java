package net.zscript.javaclient.threading;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.function.Consumer;

public class ZscriptCallbackThreadpool {
    private final ExecutorService callbackPool;

    public ZscriptCallbackThreadpool() {
        this.callbackPool = Executors.newCachedThreadPool();
    }

    public ZscriptCallbackThreadpool(ExecutorService callbackPool) {
        this.callbackPool = callbackPool;
    }

    public <T> void sendCallback(Consumer<T> callback, T content, Consumer<Exception> handler) {
        callbackPool.submit(() -> {
            try {
                callback.accept(content);
            } catch (Exception e) {
                handler.accept(e);
            }
        });
    }

    public <T extends Exception> void sendCallback(Consumer<T> callback, T content) {
        callbackPool.submit(() -> callback.accept(content));
    }

    public void sendCallback(Runnable callback, Consumer<Exception> handler) {
        callbackPool.submit(() -> {
            try {
                callback.run();
            } catch (Exception e) {
                handler.accept(e);
            }
        });
    }
}
