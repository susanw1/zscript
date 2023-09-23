package net.zscript.model.datamodel;

import java.util.List;

import static net.zscript.model.ZscriptModel.standardModel;
import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

import net.zscript.model.ZscriptModel;

class ModelValidatorTest {

    @Test
    void shouldCheck() {
        ZscriptModel model = standardModel();
        new ModelValidator(model).check();
    }

    @Test
    void shouldCheckBitRange() {
        ModelValidator modelValidator = new ModelValidator(standardModel());
        for (int i = 0; i < 16; i++) {
            assertThat(modelValidator.fitsBits(i, 4)).isTrue();
        }
        assertThat(modelValidator.fitsBits(-1, 4)).isFalse();
        assertThat(modelValidator.fitsBits(16, 4)).isFalse();
    }

    @Test
    void shouldCheckIsLower() {
        ModelValidator modelValidator = new ModelValidator(standardModel());
        assertThat(modelValidator.stringLowerCamel("a")).isTrue();
        assertThat(modelValidator.stringLowerCamel("aB")).isTrue();
        assertThat(modelValidator.stringLowerCamel("dogFood")).isTrue();
        assertThat(modelValidator.stringLowerCamel("dog2Food")).isTrue();

        assertThat(modelValidator.stringLowerCamel("")).isFalse();
        assertThat(modelValidator.stringLowerCamel(null)).isFalse();
        assertThat(modelValidator.stringLowerCamel("A")).isFalse();
        assertThat(modelValidator.stringLowerCamel("AB")).isFalse();
        assertThat(modelValidator.stringLowerCamel("DogFood")).isFalse();
        assertThat(modelValidator.stringLowerCamel("dog_food")).isFalse();
    }

    @Test
    void shouldCheckIsUpper() {
        ModelValidator modelValidator = new ModelValidator(standardModel());
        assertThat(modelValidator.stringUpperCamel("A")).isTrue();
        assertThat(modelValidator.stringUpperCamel("AB")).isTrue();
        assertThat(modelValidator.stringUpperCamel("DogFood")).isTrue();
        assertThat(modelValidator.stringUpperCamel("Dog2Food")).isTrue();

        assertThat(modelValidator.stringUpperCamel("")).isFalse();
        assertThat(modelValidator.stringUpperCamel(null)).isFalse();
        assertThat(modelValidator.stringUpperCamel("a")).isFalse();
        assertThat(modelValidator.stringUpperCamel("aB")).isFalse();
        assertThat(modelValidator.stringUpperCamel("dogFood")).isFalse();
        assertThat(modelValidator.stringUpperCamel("dog_food")).isFalse();
        assertThat(modelValidator.stringUpperCamel("Dog_food")).isFalse();
    }

    @Test
    void shouldFindDupes() {
        ModelValidator modelValidator = new ModelValidator(standardModel());
        assertThat(modelValidator.findDuplicates(List.of(1, 5, 2, 5, 6, 3, 7, 1, 9), String::valueOf)).containsExactlyInAnyOrder("1", "5");
        assertThat(modelValidator.findDuplicates(List.of(), String::valueOf)).isEmpty();
        assertThat(modelValidator.findDuplicates(List.of(3), String::valueOf)).isEmpty();
        assertThat(modelValidator.findDuplicates(List.of(3, 3, 3), String::valueOf)).containsExactly("3");
    }
}
