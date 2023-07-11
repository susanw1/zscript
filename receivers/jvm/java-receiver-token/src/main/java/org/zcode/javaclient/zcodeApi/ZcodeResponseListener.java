package org.zcode.javaclient.zcodeApi;

public interface ZcodeResponseListener<T extends ZcodeResponse> {
    void accept(T resp);

    default void notExecuted() {
    }
}
