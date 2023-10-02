package net.zscript.ascii;

public class CharacterStyle {
    private final TextColor color;
    private final TextColor background;

    private final boolean isBold;

    public CharacterStyle(TextColor color, TextColor background, boolean isBold) {
        this.color = color;
        this.background = background;
        this.isBold = isBold;
    }

    public static CharacterStyle standardStyle() {
        return new CharacterStyle(TextColor.DEFAULT, TextColor.DEFAULT, false);
    }

    public TextColor getFGColor() {
        return color;
    }

    public TextColor getBGColor() {
        return background;
    }

    public boolean isBold() {
        return isBold;
    }
}
