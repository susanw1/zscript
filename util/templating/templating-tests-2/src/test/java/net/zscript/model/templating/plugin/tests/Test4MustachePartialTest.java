package net.zscript.model.templating.plugin.tests;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringWriter;
import java.net.URL;
import java.util.Map;

import static java.nio.charset.StandardCharsets.UTF_8;
import static org.assertj.core.api.Assertions.assertThat;

import com.github.mustachejava.DefaultMustacheFactory;
import com.github.mustachejava.Mustache;
import org.junit.jupiter.api.Test;
import org.yaml.snakeyaml.Yaml;

public class Test4MustachePartialTest {
    @Test
    public void shouldRenderClasspathTemplatesWithPartials() throws IOException {
        String expected = "Test-4 (classpath, partials): Test mustache file: receipt is Classpath example on Thu Mar 16 00:00:00 GMT 2023\n"
                + "Testing partials:\n"
                + "...\n";

        final DefaultMustacheFactory mustacheFactory = new DefaultMustacheFactory();

        final String templateResource = "/template-cp/test-4-cp-part.mustache";
        final URL    templateUrl      = getClass().getResource(templateResource);
        final URL    contextUrl       = getClass().getResource("/contexts-cp/example-2.yaml");

        System.out.println("templateUrl: " + templateUrl);
        System.out.println("contextUrl : " + contextUrl);

        assertThat(templateUrl).isNotNull();
        assertThat(contextUrl).isNotNull();

        InputStream templateStream = templateUrl.openStream();
        InputStream contextStream  = contextUrl.openStream();
        assertThat(templateStream).isNotNull();
        assertThat(contextStream).isNotNull();

        final Yaml yamlMapper = new Yaml();

        try (final Reader reader = new BufferedReader(new InputStreamReader(templateStream, UTF_8))) {
            final Mustache     mustache = mustacheFactory.compile(reader, templateResource);
            final StringWriter writer   = new StringWriter();
            Map<?, ?>          context  = yamlMapper.load(contextStream);
            mustache.execute(writer, context);

            assertThat(writer.toString()).isEqualTo(expected);
        }
    }
}
