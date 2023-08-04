package net.zscript.model.transformer.plugin;

import static java.util.Arrays.stream;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.nio.charset.Charset;
import java.util.List;
import java.util.stream.Collectors;

import org.apache.maven.plugin.AbstractMojo;
import org.apache.maven.plugin.MojoExecutionException;
import org.apache.maven.plugin.MojoFailureException;
import org.apache.maven.plugins.annotations.LifecyclePhase;
import org.apache.maven.plugins.annotations.Mojo;
import org.apache.maven.plugins.annotations.Parameter;
import org.apache.maven.project.MavenProject;
import org.apache.maven.shared.model.fileset.FileSet;
import org.apache.maven.shared.model.fileset.util.FileSetManager;
import org.yaml.snakeyaml.Yaml;

import com.github.mustachejava.DefaultMustacheFactory;
import com.github.mustachejava.Mustache;

/**
 * The entry class for the maven plugin
 */
@Mojo(name = "transform", defaultPhase = LifecyclePhase.GENERATE_SOURCES)
public class TransformerMojo extends AbstractMojo {

    private static final String TEMPLATE_DEFAULT_DIR = "src/main/templates";
    private static final String CONTEXT_DEFAULT_DIR  = "src/main/contexts";
    private static final String OUTPUT_DEFAULT_DIR   = "${project.build.directory}/generated-sources/zscript";

    @Parameter(required = false)
    private FileSet templates;

//    @Parameter(required = false)
//    private File template;
//
//    @Parameter(required = false, defaultValue = TEMPLATE_DEFAULT_DIR)
//    private File templateDirectory;

    @Parameter(required = false)
    private FileSet contexts;

    @Parameter(required = false)
    private File context;

    @Parameter(required = false)
    private File contextDirectory;

    /**
     * Specify output directory where the Java files are generated.
     */
    @Parameter(defaultValue = OUTPUT_DEFAULT_DIR)
    private File outputDirectory;

    @Parameter(defaultValue = "${project.build.sourceEncoding}")
    private String encoding;

    /**
     * Specifies whether sources are added to the {@code compile} or {@code test} scope.
     */
    @Parameter(property = "zscript.generateTestSources", defaultValue = "false")
    private boolean generateTestSources;

    @Parameter(defaultValue = "${project}", readonly = true, required = true)
    private MavenProject project;

    @Override
    public void execute() throws MojoExecutionException, MojoFailureException {
        getLog().info("8=8=8=8=8=8=8=8=8=8=8=8=8 TransformerMojo STARTED 8=8=8=8=8=8=8=8=8=8=8=8=8");

        Charset charset;
        if (encoding == null || encoding.trim().length() == 0) {
            getLog().warn("File encoding has not been set, using platform encoding " + Charset.defaultCharset()
                    + ", i.e. build is platform dependent!");
            charset = Charset.defaultCharset();
        } else {
            getLog().info("Setting transform encoding to " + encoding);
            charset = Charset.forName(encoding);
        }
        getLog().debug("Using charset encoding: " + charset);

        getLog().info("outputDirectory:\n" + outputDirectory);

        getLog().info("getCompileSourceRoots:\n" + project.getCompileSourceRoots());
        getLog().info("getPackaging:\n" + project.getPackaging());
        getLog().info("getDefaultGoal:\n" + project.getDefaultGoal());

        List<File> templateFileList = extractFileList(templates, TEMPLATE_DEFAULT_DIR);
        List<File> contextFileList  = extractFileList(contexts, CONTEXT_DEFAULT_DIR);

        // read in context files as YAML. Read in templates using Mustache.
        Yaml yaml = new Yaml();

        for (File template : templateFileList) {
            for (File context : contextFileList) {
                getLog().info("Combining context " + context + " with template " + template);
            }
        }

//        Object parsedContext = createContext(context, charset);
//
//        for (TemplateRunConfiguration configuration : templates) {
//            getLog().info("Generating '" + configuration.getOutputPath() + "'");
//            runTemplateConfiguration(parsedContext, configuration, charset);
//        }
        getLog().info("8=8=8=8=8=8=8=8=8=8=8=8=8 TransformerMojo ENDED 8=8=8=8=8=8=8=8=8=8=8=8=8");
    }

    private List<File> extractFileList(final FileSet fs, final String defaultDir) {
        final FileSet fileSet = fs != null ? fs : new FileSet();

        if (fileSet.getDirectory() == null) {
            String newDir = new File(project.getBasedir(), defaultDir).toString();
            fileSet.setDirectory(newDir);
            getLog().info("project.getBasedir: " + project.getBasedir());
            getLog().info("fileSet.getDirectory: " + fileSet.getDirectory());

        }

        getLog().info("fileSet:\n" + fileSet);
        getLog().info("fileSet.getDirectory:\n" + fileSet.getDirectory());

        FileSetManager fileSetManager = new FileSetManager();
        String[]       files          = fileSetManager.getIncludedFiles(fileSet);

        getLog().info("#files = " + files.length);
        stream(files).forEach(f -> getLog().info(f));

        List<File> fileList = stream(files).map(n -> new File(fileSet.getDirectory(), n)).collect(Collectors.toList());
        getLog().info("fileList: " + fileList);

        return fileList;
    }

    void addSourceRoot(File outputDir) {
        if (generateTestSources) {
            project.addTestCompileSourceRoot(outputDir.getPath());
        } else {
            project.addCompileSourceRoot(outputDir.getPath());
        }
    }

//    private void runTemplateConfiguration(Object globalContext, TemplateRunConfiguration configuration, Charset charset)
//            throws MojoFailureException, MojoExecutionException {
//        Object templateContext = createContext(configuration.getContext(), charset);
//        if (templateContext == null) {
//            if (globalContext == null) {
//                throw new MojoFailureException("Template has no defined context and plugin context is also empty");
//            }
//            templateContext = globalContext;
//        }
//
//        Mustache mustache   = createTemplate(configuration.getTemplateFile(), charset);
//        File     outputFile = new File(configuration.getOutputPath());
//        File     parent     = outputFile.getParentFile();
//        if (!parent.exists() || parent.mkdirs()) {
//            throw new MojoFailureException("Output directory cannot be created: " + parent);
//        }
//        try (Writer writer = new OutputStreamWriter(new FileOutputStream(outputFile), charset)) {
//            mustache.execute(writer, templateContext);
//        } catch (IOException e) {
//            throw new MojoFailureException(e, "Cannot open output file", "Cannot open output file: " + e.getMessage());
//        } catch (MustacheException e) {
//            throw new MojoFailureException(e, "Cannot process template", "Cannot process template: " + e.getMessage());
//        }
//    }
//
//    private Object createContext(String contextConfiguration, Charset charset) throws MojoFailureException {
//        if (contextConfiguration == null) {
//            return null;
//        }
//
//        Yaml yaml = new Yaml();
//
//        String      contextSource = obtainContextSource(contextConfiguration, charset);
//        Matcher     matcher       = PROPERTY_PATTERN.matcher(contextSource);
//        Set<String> props         = new LinkedHashSet<>(10);
//        while (matcher.find()) {
//            props.add(matcher.group(1));
//        }
//
//        for (String prop : props) {
//            if (project.getProperties().containsKey(prop)) {
//                contextSource = contextSource.replace(String.format("${%s}", prop), project.getProperties().getProperty(prop));
//            } else {
//                getLog().warn(String.format("Property '%s' referenced in context, but doesn't exist in Maven Project...", prop));
//            }
//        }
//
//        return yaml.load(contextSource);
//    }
//
//    private static String obtainContextSource(String contextConfiguration, Charset charset) throws MojoFailureException {
//        if (contextConfiguration.startsWith("---\n")) {
//            return contextConfiguration;
//        }
//
//        String trimmedContext = contextConfiguration.trim();
//        if (trimmedContext.startsWith(FILE_PREFIX)) {
//            String filename = trimmedContext.substring(FILE_PREFIX.length());
//            try {
//                byte[] bytes = Files.readAllBytes(Paths.get(filename));
//                return new String(bytes, charset);
//            } catch (IOException e) {
//                throw new MojoFailureException(e, "Cannot load yaml from file", "Cannot load yaml from file");
//            }
//        }
//
//        throw new MojoFailureException("Cannot load context. Either pass a filename in the form 'file:[filename]' or " +
//                "include a complete yaml document, prefied with '---\\n");
//    }
//
    private static Mustache createTemplate(File template, Charset charset) throws MojoFailureException {
        DefaultMustacheFactory mf = new DefaultMustacheFactory();
        Mustache               mustache;
        try (Reader reader = new InputStreamReader(new FileInputStream(template), charset)) {
            mustache = mf.compile(reader, "template");
        } catch (IOException e) {
            throw new MojoFailureException(e, "Cannot open template", "Cannot open template");
        }
        return mustache;
    }

//    public void setContext(String context) {
//        this.context = context;
//    }
//
//    public void setEncoding(String encoding) {
//        this.encoding = encoding;
//    }
//
////    public void setTemplates(List<TemplateRunConfiguration> templates) {
////        this.templates = templates;
////    }
//
//    public void setProject(MavenProject project) {
//        this.project = project;
//    }
}
