package net.zscript.demo.morse;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

public class MorseTranslator {
    private final List<MorseElement> morseA = MorseElement.translate(".-");
    private final List<MorseElement> morseB = MorseElement.translate("-...");
    private final List<MorseElement> morseC = MorseElement.translate("-.-.");
    private final List<MorseElement> morseD = MorseElement.translate("-..");
    private final List<MorseElement> morseE = MorseElement.translate(".");
    private final List<MorseElement> morseF = MorseElement.translate("..-.");
    private final List<MorseElement> morseG = MorseElement.translate("--.");
    private final List<MorseElement> morseH = MorseElement.translate("....");
    private final List<MorseElement> morseI = MorseElement.translate("..");
    private final List<MorseElement> morseJ = MorseElement.translate(".---");
    private final List<MorseElement> morseK = MorseElement.translate("-.-");
    private final List<MorseElement> morseL = MorseElement.translate(".-..");
    private final List<MorseElement> morseM = MorseElement.translate("--");
    private final List<MorseElement> morseN = MorseElement.translate("-.");
    private final List<MorseElement> morseO = MorseElement.translate("---");
    private final List<MorseElement> morseP = MorseElement.translate(".--.");
    private final List<MorseElement> morseQ = MorseElement.translate("--.-");
    private final List<MorseElement> morseR = MorseElement.translate(".-.");
    private final List<MorseElement> morseS = MorseElement.translate("...");
    private final List<MorseElement> morseT = MorseElement.translate("-");
    private final List<MorseElement> morseU = MorseElement.translate("..-");
    private final List<MorseElement> morseV = MorseElement.translate("...-");
    private final List<MorseElement> morseW = MorseElement.translate(".--");
    private final List<MorseElement> morseX = MorseElement.translate("-..-");
    private final List<MorseElement> morseY = MorseElement.translate("-.--");
    private final List<MorseElement> morseZ = MorseElement.translate("--..");

    private final List<MorseElement> morse1 = MorseElement.translate(".----");
    private final List<MorseElement> morse2 = MorseElement.translate("..---");
    private final List<MorseElement> morse3 = MorseElement.translate("...--");
    private final List<MorseElement> morse4 = MorseElement.translate("....-");
    private final List<MorseElement> morse5 = MorseElement.translate(".....");
    private final List<MorseElement> morse6 = MorseElement.translate("-....");
    private final List<MorseElement> morse7 = MorseElement.translate("--...");
    private final List<MorseElement> morse8 = MorseElement.translate("---..");
    private final List<MorseElement> morse9 = MorseElement.translate("----.");
    private final List<MorseElement> morse0 = MorseElement.translate("-----");

    private final List<MorseElement> morsePeriod = MorseElement.translate(".-.-.-");
    private final List<MorseElement> morseComma  = MorseElement.translate("--..--");
    private final List<MorseElement> morseQMark  = MorseElement.translate("..--..");
    private final List<MorseElement> morseApos   = MorseElement.translate(".----.");
    private final List<MorseElement> morseSlash  = MorseElement.translate("-..-.");
    private final List<MorseElement> morseOpenP  = MorseElement.translate("-.--.");
    private final List<MorseElement> morseCloseP = MorseElement.translate("-.--.-");
    private final List<MorseElement> morseColon  = MorseElement.translate("---...");
    private final List<MorseElement> morseEquals = MorseElement.translate("-...-");
    private final List<MorseElement> morsePlus   = MorseElement.translate(".-.-.");
    private final List<MorseElement> morseMinus  = MorseElement.translate("-....-");
    private final List<MorseElement> morseQuote  = MorseElement.translate(".-..-.");
    private final List<MorseElement> morseAt     = MorseElement.translate(".--.-.");

    private final List<MorseElement> morseError      = MorseElement.translate("........");
    private final List<MorseElement> morseMessageSep = MorseElement.translate("-.-.-");

    private final List<MorseElement> morseSpace = List.of(MorseElement.WORD_SPACE);

    private final List<MorseElement>[] morseElArray = new List[] {
            morseA, morseB, morseC, morseD, morseE,
            morseF, morseG, morseH, morseI, morseJ,
            morseK, morseL, morseM, morseN, morseO,
            morseP, morseQ, morseR, morseS, morseT,
            morseU, morseV, morseW, morseX, morseY,
            morseZ,
            morse0, morse1, morse2, morse3, morse4,
            morse5, morse6, morse7, morse8, morse9,
            morsePeriod, morseComma, morseQMark, morseApos, morseSlash,
            morseOpenP, morseCloseP, morseColon, morseEquals, morsePlus,
            morseMinus, morseQuote, morseAt,
            morseError, morseMessageSep,
            morseSpace
    };

    private final char[] morseChArray = new char[] {
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
            'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
            'U', 'V', 'W', 'X', 'Y', 'Z',
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
            '.', ',', '?', '\'', '/', '(', ')', ':', '=', '+',
            '-', '"', '@',
            '�', '\n',
            ' '
    };

    public List<MorseElement> translate(String s) {
        List<MorseElement> elements         = new ArrayList<>();
        boolean            needsLetterSpace = false; // is needed to avoid letter spaces on word spaces.
        for (char c : s.toCharArray()) {
            boolean found     = false;
            char    effective = Character.toUpperCase(c);
            if (needsLetterSpace && effective != ' ') {
                elements.add(MorseElement.LETTER_SPACE);
            }
            for (int i = 0; i < morseChArray.length; i++) {
                if (effective == morseChArray[i]) {
                    elements.addAll(morseElArray[i]);
                    found = true;
                    break;
                }
            }
            if (!found) {
                elements.add(MorseElement.LETTER_SPACE);
                elements.addAll(morseError);
            }
            needsLetterSpace = effective != ' ';
        }
        return elements;
    }

    public char translate(List<MorseElement> element) {
        List<MorseElement> elementClean = element.stream().filter(e -> e != MorseElement.LETTER_SPACE).collect(Collectors.toList());
        for (int i = 0; i < morseElArray.length; i++) {
            List<MorseElement> match = morseElArray[i];
            if (match.equals(elementClean)) {
                return morseChArray[i];
            }
        }
        return '�';
    }
}
