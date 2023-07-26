package org.zcode.javareceiver.tokenizer;

import java.util.NoSuchElementException;
import java.util.Optional;

/**
 * Easy alternative to the traditional {@link java.util.Iterator}. The {@link #next()} method returns successive values with each call; when the values run out, the method returns
 * {@link java.util.Optional#empty()}.
 *
 * None of the items may be null.
 *
 * Usefully, there's no hasNext() to implement. On the other hand, you cannot check whether there's another item without taking it.
 *
 * @param <T> items being iterated
 */
public interface OptIterator<T> {

    /**
     * Returns successive items from the sequence being iterated.
     *
     * Note: calling this method <i>after</i> it has returned {@code empty} may result in undefined behaviour. We'd expect implementations to throw {@link NoSuchElementException}
     * to highlight improper use, but they might not.
     *
     * @return value items wrapped as Optional, or {@link java.util.Optional#empty()} to signal end of items
     * @exception NoSuchElementException on going beyond the end (optional)
     */
    Optional<T> next();
    
}
