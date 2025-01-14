package net.zscript.javareceiver.treeparser;

import java.util.Iterator;
import java.util.List;

public class ZscriptAndSequenceUnit implements ZscriptSequenceUnit {
    private final List<ZscriptSequenceUnitPlace> units;

    public ZscriptAndSequenceUnit(List<ZscriptSequenceUnitPlace> units) {
        this.units = units;
    }

    @Override
    public boolean parse() {
        boolean doneAnything = false;
        for (ZscriptSequenceUnitPlace unit : units) {
            doneAnything |= unit.parse();
        }
        return doneAnything;
    }

    @Override
    public String toString() {
        StringBuilder b = new StringBuilder();
        for (Iterator<ZscriptSequenceUnitPlace> iterator = units.iterator(); iterator.hasNext(); ) {
            ZscriptSequenceUnitPlace unit = iterator.next();
            b.append(unit.toString());
            if (iterator.hasNext()) {
                b.append('&');
            }
        }
        return b.toString();
    }
}
