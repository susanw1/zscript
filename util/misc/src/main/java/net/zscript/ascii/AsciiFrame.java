package net.zscript.ascii;

import java.util.Iterator;

public interface AsciiFrame extends Iterable<TextRow> {

    int getWidth();

    int getHeight();

    boolean setWidth(int width);

}
