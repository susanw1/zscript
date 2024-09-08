package net.zscript.tokenizer;

public interface ZscriptField extends Iterable<Byte> {
    byte getKey();

    int getValue();

    boolean isBigField();

    @Override
    BlockIterator iterator();
}
