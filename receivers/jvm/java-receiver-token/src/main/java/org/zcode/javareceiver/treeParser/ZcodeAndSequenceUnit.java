package org.zcode.javareceiver.treeParser;

import java.util.Iterator;
import java.util.List;

public class ZcodeAndSequenceUnit implements ZcodeSequenceUnit {
    private final List<ZcodeSequenceUnitPlace> units;

    public ZcodeAndSequenceUnit(List<ZcodeSequenceUnitPlace> units) {
        this.units = units;
    }

    @Override
    public boolean parse() {
        boolean doneAnything = false;
        for (ZcodeSequenceUnitPlace unit : units) {
            doneAnything |= unit.parse();
        }
        return doneAnything;
    }

    @Override
    public String toString() {
        StringBuilder b = new StringBuilder();
        for (Iterator<ZcodeSequenceUnitPlace> iterator = units.iterator(); iterator.hasNext();) {
            ZcodeSequenceUnitPlace unit = iterator.next();
            b.append(unit.toString());
            if (iterator.hasNext()) {
                b.append('&');
            }
        }
        return b.toString();
    }
}
