package org.zcode.javareceiver.tokenizer;

import java.util.Optional;

public interface OptIterator<T> {

    Optional<T> next();
}
