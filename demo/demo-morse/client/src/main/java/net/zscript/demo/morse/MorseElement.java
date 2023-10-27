package net.zscript.demo.morse;

import java.util.ArrayList;
import java.util.List;

public enum MorseElement {
    DIT(1, true),
    DAR(3, true),
    LETTER_SPACE(1, false),
    WORD_SPACE(1, false);
    //Space is implied between all other elements, so LETTER_SPACE only requires 1 dit of blank
    // WORD_SPACE is put in for the space character, so only one dit of blank is required (since there is a letter space either side)

    private final int     length;
    private final boolean isHigh;

    MorseElement(int length, boolean isHigh) {
        this.length = length;
        this.isHigh = isHigh;
    }

    public int getLength() {
        return length;
    }

    public boolean isHigh() {
        return isHigh;
    }

    public static List<MorseElement> translate(String ditDar) {
        ArrayList<MorseElement> elements = new ArrayList<>();
        for (char c : ditDar.toCharArray()) {
            if (c == '.') {
                elements.add(DIT);
            } else if (c == '-') {
                elements.add(DAR);
            } else {
                throw new IllegalArgumentException("Dit-Dar string cannot contain characters other than '.' or '-'\n Found: " + c);
            }
        }
        return elements;
    }
}
