package net.zscript.model.templating.plugin.tests;

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;

import static org.assertj.core.api.Assertions.assertThat;

import org.apache.commons.io.IOUtils;
import org.junit.jupiter.api.Test;

public class BasicTemplatingTest {
    @Test
    public void shouldCreateFirstOutputFile() throws IOException {
        String      expectedContent = "Test-1: Test mustache file: receipt is Oz-Ware Purchase Invoice on Mon Aug 06 01:00:00 BST 2012\n";
        InputStream input           = getClass().getResourceAsStream("/templates-out/test1/exampleA-1.txt");
        String      content         = IOUtils.toString(input, StandardCharsets.UTF_8);
        assertThat(content).isEqualTo(expectedContent);
    }

    @Test
    public void shouldCreateSecondOutputFile() throws IOException {
        String      expectedContent = "Test-1: Test mustache file: receipt is Something Else on Sun Jan 06 00:00:00 GMT 2013\n";
        InputStream input           = getClass().getResourceAsStream("/templates-out/test1/exampleB-1.txt");
        String      content         = IOUtils.toString(input, StandardCharsets.UTF_8);
        assertThat(content).isEqualTo(expectedContent);
    }
}
