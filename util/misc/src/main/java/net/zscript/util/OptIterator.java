package net.zscript.util;

import javax.annotation.Nonnull;
import java.util.Iterator;
import java.util.NoSuchElementException;
import java.util.Optional;
import java.util.function.Consumer;
import java.util.stream.Stream;

/**
 * Easy-to-implement alternative to the traditional {@link java.util.Iterator}. The {@link #next()} method returns successive values with each call; when the values run out, the
 * method returns {@link java.util.Optional#empty()}.
 * <p>
 * None of the items may be null.
 * <p>
 * Usefully, there's no hasNext() to implement. On the other hand, you cannot check *whether* there's another item without *taking* it.
 *
 * @param <T> items being iterated
 */
public interface OptIterator<T> {

    /**
     * Returns successive items from the sequence being iterated.
     * <p/>
     * Note: calling this method <i>after</i> it has returned {@code empty} may result in undefined behaviour. We'd expect implementations to throw {@link NoSuchElementException}
     * to highlight improper use, but they might not.
     *
     * @return value items wrapped as Optional, or {@link java.util.Optional#empty()} to signal end of items
     * @throws NoSuchElementException on going beyond the end (optional)
     */
    @Nonnull
    Optional<T> next();

    /**
     * Handy forEach implementation with similar semantics to {@link java.lang.Iterable#forEach(Consumer)}.
     *
     * @param action a consumer that will be fed the items until they run out
     */
    default void forEach(final Consumer<? super T> action) {
        Optional<T> t;
        while ((t = next()).isPresent()) {
            action.accept(t.get());
        }
    }

    /**
     * Exposes the elements as a conventional {@link java.util.stream.Stream}.
     *
     * @return a stream of elements
     */
    @Nonnull
    default Stream<T> stream() {
        return Stream.iterate(next(), Optional::isPresent, t -> next()).map(Optional::get);
    }

    /**
     * Exposes the contents of any {@link java.lang.Iterable} as an {@link OptIterator}.
     *
     * @param <T>      items being iterated
     * @param iterable the Iterable object to read
     * @return a suitable OptIterator
     */
    @Nonnull
    static <T> OptIterator<T> of(final Iterable<T> iterable) {
        return new OptIterator<>() {
            private final Iterator<T> origIterator = iterable.iterator();

            @Nonnull
            @Override
            public Optional<T> next() {
                return origIterator.hasNext() ? Optional.of(origIterator.next()) : Optional.empty();
            }
        };
    }
}
