package net.zscript.ascii;

import javax.annotation.Nonnull;

public interface CharacterStylePrinter {

    @Nonnull
    String apply(CharacterStyle style);

    @Nonnull
    String applyDiff(CharacterStyle prev, CharacterStyle next);

    @Nonnull
    String cancel(CharacterStyle style);
}
