package net.zscript.model.templating.contextloader;

import java.time.Instant;
import java.util.List;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

class ConversionHelperTest {
    ConversionHelper helper = new ConversionHelper();

    @Test
    public void shouldFixNonJavaIdentifiers() {
        assertThat(helper.fixNonJavaIdentifiers("  a-b  cD;e#!f,g ")).isEqualTo("a_b_cD_e_f_g");
    }

    @Test
    public void shouldConvertToUpper() {
        assertThat(helper.toUpper().apply("aBcD 3 eF")).isEqualTo("ABCD 3 EF");
    }

    @Test
    public void shouldConvertToLower() {
        assertThat(helper.toLower().apply("ABcD 3 eF")).isEqualTo("abcd 3 ef");
    }

    @Test
    public void shouldConvertToLowerCamel() {
        assertThat(helper.lowerCamel().apply("ThreeBlindMice")).isEqualTo("threeBlindMice");
        assertThat(helper.lowerCamel().apply("threeBlindMice")).isEqualTo("threeBlindMice");
        assertThat(helper.lowerCamel().apply("Three_blind_mice")).isEqualTo("three_blind_mice");
        assertThat(helper.lowerCamel().apply("three_blind_mice")).isEqualTo("three_blind_mice");
    }

    @Test
    public void shouldConvertToUpperCamel() {
        assertThat(helper.upperCamel().apply("ThreeBlindMice")).isEqualTo("ThreeBlindMice");
        assertThat(helper.upperCamel().apply("threeBlindMice")).isEqualTo("ThreeBlindMice");
        assertThat(helper.upperCamel().apply("Three_blind_mice")).isEqualTo("Three_blind_mice");
        assertThat(helper.upperCamel().apply("three_blind_mice")).isEqualTo("Three_blind_mice");
    }

    @Test
    public void shouldConvertToLowerSnake() {
        assertThat(helper.lowerUnderscore().apply("ThreeBlindMice")).isEqualTo("three_blind_mice");
        assertThat(helper.lowerUnderscore().apply("threeBlindMice")).isEqualTo("three_blind_mice");
        assertThat(helper.lowerUnderscore().apply("Three_blind_mice")).isEqualTo("three_blind_mice");
        assertThat(helper.lowerUnderscore().apply("three_blind_mice")).isEqualTo("three_blind_mice");
    }

    @Test
    public void shouldConvertToUpperSnake() {
        assertThat(helper.upperUnderscore().apply("ThreeBlindMice")).isEqualTo("THREE_BLIND_MICE");
        assertThat(helper.upperUnderscore().apply("threeBlindMice")).isEqualTo("THREE_BLIND_MICE");
        assertThat(helper.upperUnderscore().apply("Three_blind_mice")).isEqualTo("THREE_BLIND_MICE");
        assertThat(helper.upperUnderscore().apply("three_blind_mice")).isEqualTo("THREE_BLIND_MICE");
    }

    @Test
    public void shouldConvertToLowerHyphen() {
        assertThat(helper.lowerHyphen().apply("ThreeBlindMice")).isEqualTo("three-blind-mice");
        assertThat(helper.lowerHyphen().apply("threeBlindMice")).isEqualTo("three-blind-mice");
        assertThat(helper.lowerHyphen().apply("Three_blind_mice")).isEqualTo("three_blind_mice"); // note - it thinks we're in camel
        assertThat(helper.lowerHyphen().apply("three_blind_mice")).isEqualTo("three_blind_mice");
    }

    @Test
    public void shouldGetTime() {
        String time = helper.timeNow().apply("");
        assertThat(Instant.parse(time)).isNotNull();
    }

    @Test
    public void shouldBuildDottedList() {
        assertThat(helper.listDotSeparated().apply(List.of("Three", "Blind", "Mice").toString())).isEqualTo("three.blind.mice");
        assertThat(helper.listDotSeparated().apply("[net, zscript, model]")).isEqualTo("net.zscript.model");
        assertThat(helper.listDotSeparated().apply("[sr71, #brown, micro$oft]")).isEqualTo("sr71._brown.micro_oft");
    }

    @Test
    public void shouldFindHex() {
        assertThat(helper.toHex().apply("12")).isEqualTo("c");
    }

    @Test
    public void shouldAccessAddtionalMap() {
        assertThat(helper.getAdditional()).isNotNull();
        helper.getAdditional().put("x", "hello");
        assertThat(helper.getAdditional().get("x")).isEqualTo("hello");
    }
}
