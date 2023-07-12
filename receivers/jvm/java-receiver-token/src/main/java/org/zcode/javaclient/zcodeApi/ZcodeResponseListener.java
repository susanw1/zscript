package org.zcode.javaclient.zcodeApi;

public interface ZcodeResponseListener<T extends ZcodeResponse> {
    void accept(T resp);

    // TODO: call this consistently
    default void notExecuted() {
    }
}
