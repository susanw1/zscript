package net.zscript.util;

import javax.annotation.Nonnull;
import java.util.Iterator;
import java.util.List;
import java.util.NoSuchElementException;
import java.util.Optional;
import java.util.function.Consumer;
import java.util.stream.Collectors;
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

    /**
     * Drains this OptIterator's items into a list (as per {@link Collectors#toList()}).
     *
     * @return a list of the items
     */
    @Nonnull
    default List<T> toList() {
        return stream().collect(Collectors.toList());
    }

    /**
     * Converts this OptIterator into a simple Iterable with a corresponding Iterator.
     * <p>
     * This method is intended to facilitate usage in foreach loops:
     * <pre>
     * {@code
     * for (Thing t : thingSource.things().toIterable()) {
     *      System.out.println(t);
     * }
     * }
     * </pre>
     *
     * Warning: using the returned Iterable and its corresponding Iterator will drain this OptIterator's items (they aren't stored or copied)! Dependence on exactly how this
     * behaves or interacts with use of the original OptIterator is fragile.
     *
     * @return this OptIterator in Iterable form
     */
    default Iterable<T> toIterable() {
        return new Iterable<>() {
            @Override
            @Nonnull
            public Iterator<T> iterator() {
                return new Iterator<>() {
                    @SuppressWarnings("OptionalUsedAsFieldOrParameterType")
                    Optional<T> cur = OptIterator.this.next();

                    @Override
                    public boolean hasNext() {
                        return cur.isPresent();
                    }

                    @Override
                    public T next() {
                        final T tmp = cur.orElseThrow(NoSuchElementException::new);
                        cur = OptIterator.this.next();
                        return tmp;
                    }
                };
            }
        };
    }
}
