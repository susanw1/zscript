package net.zscript.model.templating.plugin.tests;

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;

import static org.assertj.core.api.Assertions.assertThat;

import org.apache.commons.io.IOUtils;
import org.junit.jupiter.api.Test;

public class Test3LocalFileTemplatingPartialTest {
    @Test
    public void shouldCreateFileUsingPartials() throws IOException {
        String expectedContent = "Test-3: Test mustache file: receipt is Oz-Ware Purchase Invoice on Mon Aug 06 01:00:00 BST 2012\n"
                + "Partial#1: Test-3a: Bill-to: 123 Tornado Alley&#10;Suite 16&#10;\n\n"
                + "Partial#2: Test-3b: - Ship-to: 123 Tornado Alley&#10;Suite 16&#10;\n\n";

        InputStream input = getClass().getResourceAsStream("/templates-out/test3/exampleA-1.txt");
        assertThat(input).isNotNull();

        String content = IOUtils.toString(input, StandardCharsets.UTF_8);
        assertThat(content).isEqualTo(expectedContent);
    }
}
