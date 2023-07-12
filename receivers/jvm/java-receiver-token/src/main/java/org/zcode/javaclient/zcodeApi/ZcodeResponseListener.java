package org.zcode.javaclient.zcodeApi;

public interface ZcodeResponseListener<T extends ZcodeResponse> {
    void accept(T resp);

    // TODO: call this consistently
    default void notExecuted() {
    }

    private static <T extends ZcodeResponse> ZcodeResponseListener<T> onNotExecuted(Runnable r) {
        return new ZcodeResponseListener<T>() {
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
