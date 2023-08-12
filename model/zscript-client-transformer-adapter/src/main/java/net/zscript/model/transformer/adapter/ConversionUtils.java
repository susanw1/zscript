package net.zscript.model.transformer.adapter;

import static com.google.common.base.CaseFormat.LOWER_CAMEL;
import static com.google.common.base.CaseFormat.LOWER_HYPHEN;
import static com.google.common.base.CaseFormat.LOWER_UNDERSCORE;
import static com.google.common.base.CaseFormat.UPPER_CAMEL;
import static com.google.common.base.CaseFormat.UPPER_UNDERSCORE;

import java.util.function.Function;
import java.util.stream.Collectors;
import java.util.stream.Stream;

public class ConversionUtils {
    public Function<String, String> toUpper() {
        return s -> s.toUpperCase();
    }

    public Function<String, String> toLower() {
        return s -> s.toLowerCase();
    }

    /**
     * Function which converts a toString'ed List (','-separated, encased in '[]', to a '.'-separated string. eg, "[a, b, c]" becomes "a.b.c". Good for Java package names.
     */
    public Function<String, String> listDotSeparated() {
        return s -> Stream.of(s.replaceAll("[\\]\\[]", "").split(", *")).map(lowerUnderscore()::apply).collect(Collectors.joining("."));
    }

    public Function<String, String> lowerCamel() {
        return s -> UPPER_CAMEL.to(LOWER_CAMEL, fixNonJavaIdentifiers(s));
    }

    public Function<String, String> lowerHyphen() {
        return s -> UPPER_CAMEL.to(LOWER_HYPHEN, fixNonJavaIdentifiers(s));
    }

    public Function<String, String> lowerUnderscore() {
        return s -> UPPER_CAMEL.to(LOWER_UNDERSCORE, fixNonJavaIdentifiers(s));
    }

    public Function<String, String> upperCamel() {
        return s -> LOWER_CAMEL.to(UPPER_CAMEL, fixNonJavaIdentifiers(s));
    }

    public Function<String, String> upperUnderscore() {
        return s -> UPPER_CAMEL.to(UPPER_UNDERSCORE, fixNonJavaIdentifiers(s));
    }

    private String fixNonJavaIdentifiers(String s) {
        return s.replaceAll("[^A-Za-z0-9_]+", "_");
    }
}
