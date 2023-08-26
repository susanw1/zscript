package net.zscript.javaclient.commandbuilder;

public interface ZscriptResponseListener<T extends ZscriptResponse> {
    void accept(T resp);

    // TODO: call this consistently
    default void notExecuted() {
    }

    private static <T extends ZscriptResponse> ZscriptResponseListener<T> onNotExecuted(Runnable r) {
        return new ZscriptResponseListener<T>() {
            @Override
            public void accept(T resp) {
            }

            @Override
            public void notExecuted() {
                r.run();
            }
        };
    }
}