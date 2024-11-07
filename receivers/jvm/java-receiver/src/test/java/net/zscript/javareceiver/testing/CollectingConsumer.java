package net.zscript.javareceiver.testing;

import javax.annotation.Nonnull;
import java.util.Iterator;
import java.util.Optional;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.function.Consumer;

import static java.util.Optional.ofNullable;

import net.zscript.util.OptIterator;

/**
 * Simple utility that accepts items as a consumer and collects them in a queue for later collection, guaranteeing same order.
 *
 * @param <T> the type of item collected.
 */
public class CollectingConsumer<T> implements Consumer<T> {
    private final Queue<T> queue = new ConcurrentLinkedQueue<>();

    /**
     * Receives the next item to be enqueued.
     *
     * @param item the item to enqueue
     */
    @Override
    public void accept(T item) {
        queue.add(item);
    }

    /**
     * Accesses the item received via the {@link #accept(T)} method, in the style of an iterator.
     *
     * @return the next item if present, empty if none available
     */
    @Nonnull
    public Optional<T> next() {
        return ofNullable(queue.poll());
    }

    /**
     * Presents the current collection as an OptIterator. Note that if the OptIterator returns empty and then new items are accepted, it will then return them correctly (ie this is
     * not undefined behaviour).
     *
     * @return an OptIterator representation of the collected items
     */
    @Nonnull
    public OptIterator<T> asOptIterator() {
        return new OptIterator<>() {
            @Nonnull
            @Override
            public Optional<T> next() {
                return CollectingConsumer.this.next();
            }
        };
    }

    /**
     * Presents the current collection as an Iterator. Note that if the {@link Iterator#hasNext()}  returns false and then new items are accepted, it will then return true and be
     * capable of returning them.
     *
     * @return an Iterator representation of the collected items
     */
    @Nonnull
    public Iterator<T> asIterator() {
        return asOptIterator().stream().iterator();
    }

}
