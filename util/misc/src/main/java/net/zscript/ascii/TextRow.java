package net.zscript.ascii;

public class TextRow {
    private final CharacterStyle[] styles;
    private final char[]           chars;

    public TextRow(char[] chars, CharacterStyle[] styles) {
        this.chars = chars;
        this.styles = styles;
    }

    public String toString(CharacterStylePrinter printer) {
        CharacterStyle prev    = CharacterStyle.standardStyle();
        StringBuilder  builder = new StringBuilder();
        for (int i = 0; i < chars.length; i++) {
            if (styles[i] != prev) {
                builder.append(printer.applyDiff(prev, styles[i]));
                prev = styles[i];
            }
            builder.append(chars[i]);
        }
        return builder.toString();
    }

    public char charAt(int i) {
        return chars[i];
    }

    public CharacterStyle styleAt(int i) {
        return styles[i];
    }
}
