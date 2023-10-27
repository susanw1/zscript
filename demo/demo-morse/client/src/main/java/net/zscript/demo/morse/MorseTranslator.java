package net.zscript.demo.morse;

import java.util.ArrayList;
import java.util.List;

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

    public List<MorseElement> translate(String s) {
        List<MorseElement> elements = new ArrayList<>();
        for (char c : s.toCharArray()) {
            char effective = Character.toUpperCase(c);
            switch (effective) {
            case 'A':
                elements.addAll(morseA);
                break;
            case 'B':
                elements.addAll(morseB);
                break;
            case 'C':
                elements.addAll(morseC);
                break;
            case 'D':
                elements.addAll(morseD);
                break;
            case 'E':
                elements.addAll(morseE);
                break;
            case 'F':
                elements.addAll(morseF);
                break;
            case 'G':
                elements.addAll(morseG);
                break;
            case 'H':
                elements.addAll(morseH);
                break;
            case 'I':
                elements.addAll(morseI);
                break;
            case 'J':
                elements.addAll(morseJ);
                break;
            case 'K':
                elements.addAll(morseK);
                break;
            case 'L':
                elements.addAll(morseL);
                break;
            case 'M':
                elements.addAll(morseM);
                break;
            case 'N':
                elements.addAll(morseN);
                break;
            case 'O':
                elements.addAll(morseO);
                break;
            case 'P':
                elements.addAll(morseP);
                break;
            case 'Q':
                elements.addAll(morseQ);
                break;
            case 'R':
                elements.addAll(morseR);
                break;
            case 'S':
                elements.addAll(morseS);
                break;
            case 'T':
                elements.addAll(morseT);
                break;
            case 'U':
                elements.addAll(morseU);
                break;
            case 'V':
                elements.addAll(morseV);
                break;
            case 'W':
                elements.addAll(morseW);
                break;
            case 'X':
                elements.addAll(morseX);
                break;
            case 'Y':
                elements.addAll(morseY);
                break;
            case 'Z':
                elements.addAll(morseZ);
                break;
            case '1':
                elements.addAll(morse1);
                break;
            case '2':
                elements.addAll(morse2);
                break;
            case '3':
                elements.addAll(morse3);
                break;
            case '4':
                elements.addAll(morse4);
                break;
            case '5':
                elements.addAll(morse5);
                break;
            case '6':
                elements.addAll(morse6);
                break;
            case '7':
                elements.addAll(morse7);
                break;
            case '8':
                elements.addAll(morse8);
                break;
            case '9':
                elements.addAll(morse9);
                break;
            case '0':
                elements.addAll(morse0);
                break;
            default:
                elements.add(MorseElement.WORD_SPACE);
            }
            elements.add(MorseElement.LETTER_SPACE);
        }
        return elements;
    }
}
