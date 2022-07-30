package org.zcode.javaclient.builders;

import java.util.Optional;

public class Response {

    public <T> T getResponseParam(String name, Class<T> type) {
        return type.cast(Optional.of(1));
    }

}
