package net.zscript.util;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

import static java.util.stream.Collectors.toList;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.fail;

import org.junit.jupiter.api.Test;

class OptIteratorTest {
    final OptIterator<Integer> simpleListIterator = OptIterator.of(List.of(1, 3, 5));

    @Test
    void shouldIterateList() {
        assertThat(simpleListIterator.next()).isPresent().get().isEqualTo(1);
        assertThat(simpleListIterator.next()).isPresent().get().isEqualTo(3);
        assertThat(simpleListIterator.next()).isPresent().get().isEqualTo(5);
        assertThat(simpleListIterator.next()).isEmpty();
    }

    @Test
    void shouldIterateEmptyList() {
        OptIterator<Integer> oi = OptIterator.of(List.of());
        assertThat(oi.next()).isEmpty();
    }

    @Test
    void shouldCreateStream() {
        List<Integer> filteredList = simpleListIterator.stream().filter(n -> n != 3).collect(toList());
        assertThat(filteredList).isEqualTo(List.of(1, 5));
    }

    @Test
    void shouldExecuteForEach() {
        AtomicInteger total = new AtomicInteger(); // using this as a mutable int
        simpleListIterator.forEach(total::addAndGet);
        assertThat(total.get()).isEqualTo(9);
    }

    @Test
    void shouldWorkInForeachLoops() {
        AtomicInteger total = new AtomicInteger(); // using this as a mutable int
        for (int i : simpleListIterator.toIterable()) {
            total.addAndGet(i);
        }
        assertThat(total.get()).isEqualTo(9);
    }

    @Test
    void shouldCreateList() {
        assertThat(simpleListIterator.toList()).isEqualTo(List.of(1, 3, 5));
    }

    @Test
    void shouldWorkInForeachLoopsWhenEmpty() {
        List<Integer> collect = new ArrayList<>();
        for (int i : OptIterator.of(List.<Integer>of()).toIterable()) {
            fail("unexpectedly found " + i);
        }
    }

}
