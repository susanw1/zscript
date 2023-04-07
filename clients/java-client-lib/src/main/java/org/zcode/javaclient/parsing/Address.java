package org.zcode.javaclient.parsing;

import java.util.ArrayList;
import java.util.List;

public final class Address {
    private final List<Integer> components;

    public Address(List<Integer> components) {
        this.components = List.copyOf(components);
    }

    public boolean isEmpty() {
        return components.isEmpty();
    }

    public List<Integer> getComponents() {
        return components;
    }

    public static AddressBuilder builder() {
        return new AddressBuilder();
    }

    public static class AddressBuilder {
        private final List<Integer> components = new ArrayList<>();
        private int                 nibblePos  = -1;

        private AddressBuilder() {
        }

        public boolean add4(byte nibble) {
            if (nibblePos >= 4) {
                return false;
            }
            if (nibblePos == -1) {
                components.add(nibble & 0xf);
            } else {
                int index = components.size() - 1;
                components.set(index, (components.get(index) << 4) | (nibble & 0xf));
            }

            nibblePos++;
            return true;
        }

        public void newComponent() {
            nibblePos = 0;
            components.add(0);
        }

        public void reset() {
            components.clear();
            nibblePos = -1;
        }

        public Address build() {
            return new Address(components);
        }
    }
}
