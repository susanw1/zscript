package net.zscript.model.templating.plugin.tests;

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;

import static org.assertj.core.api.Assertions.assertThat;

import org.apache.commons.io.IOUtils;
import org.junit.jupiter.api.Test;

public class Test2ClasspathTest {
    @Test
    public void shouldCreateOutputFile() throws IOException {
        String      expectedContent = "Test-2 (classpath): Test mustache file: receipt is Classpath example on Thu Mar 16 00:00:00 GMT 2023\n";
        InputStream input           = getClass().getResourceAsStream("/templates-out/test2/example-2.txt");
        String      content         = IOUtils.toString(input, StandardCharsets.UTF_8);
        assertThat(content).isEqualTo(expectedContent);
    }
}
