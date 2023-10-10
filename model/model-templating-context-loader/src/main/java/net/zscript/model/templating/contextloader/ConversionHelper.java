package net.zscript.model.templating.contextloader;

import java.time.Instant;
import java.time.format.DateTimeFormatter;
import java.util.HashMap;
import java.util.Map;
import java.util.function.Function;
import java.util.stream.Stream;

import static com.google.common.base.CaseFormat.LOWER_CAMEL;
import static com.google.common.base.CaseFormat.LOWER_HYPHEN;
import static com.google.common.base.CaseFormat.LOWER_UNDERSCORE;
import static com.google.common.base.CaseFormat.UPPER_CAMEL;
import static com.google.common.base.CaseFormat.UPPER_UNDERSCORE;
import static java.util.stream.Collectors.joining;

@SuppressWarnings("unused")
public final class ConversionHelper {
    private final Map<String, Object> additional = new HashMap<>();

    public Map<String, Object> getAdditional() {
        return additional;
    }

    public Function<String, String> toUpper() {
        return String::toUpperCase;
    }

    public Function<String, String> toLower() {
        return String::toLowerCase;
    }

    /**
     * Function which converts a toString'ed List (','-separated, encased in '[]', to a '.'-separated string. eg, "[a, b, c]" becomes "a.b.c". Good for Java package names.
     */
    public Function<String, String> listDotSeparated() {
        return s -> Stream.of(s.replaceAll("[\\]\\[]", "").split(", *")).map(lowerUnderscore()).collect(joining("."));
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

    public Function<String, String> timeNow() {
        return s -> DateTimeFormatter.ISO_INSTANT.format(Instant.now());
    }

    public Function<String, String> toHex() {
        return s -> Integer.toHexString(Integer.parseInt(s));
    }
}
