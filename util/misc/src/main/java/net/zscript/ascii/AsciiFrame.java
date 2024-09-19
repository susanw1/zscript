package net.zscript.ascii;

import javax.annotation.Nonnull;

public interface AsciiFrame extends Iterable<TextRow> {

    int getWidth();

    int getHeight();

    boolean setWidth(int width);

    @Nonnull
    default String generateString(CharacterStylePrinter printer) {
        StringBuilder b = new StringBuilder();
        for (TextRow row : this) {
            b.append(row.toString(printer));
            b.append("\n");
        }
        return b.toString();
    }

}
