package net.zscript.model.transformer.plugin;

import static java.util.Arrays.stream;
import static java.util.stream.Collectors.toList;

import java.io.File;
import java.io.IOException;
import java.io.Reader;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import java.util.function.BiFunction;

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

//    @Parameter(required = false)
//    private File context;
//
//    @Parameter(required = false)
//    private File contextDirectory;

    /**
     * Specify output directory where the Java files are generated.
     */
    @Parameter(defaultValue = OUTPUT_DEFAULT_DIR)
    private File outputDirectory;

    @Parameter(defaultValue = "${project.build.sourceEncoding}")
    private String encoding;

    @Parameter(defaultValue = "true")
    private boolean failIfNoFiles;

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

        getLog().info("outputDirectory: " + outputDirectory);

        getLog().info("getCompileSourceRoots:\n" + project.getCompileSourceRoots());
        getLog().info("getPackaging: " + project.getPackaging());
        getLog().info("getDefaultGoal: " + project.getDefaultGoal());

        final FileSet    templateFileSet    = initFileSet(templates, TEMPLATE_DEFAULT_DIR);
        List<FileEntity> templateEntityList = extractFileList(templateFileSet, "Template");
        final FileSet    contextFileSet     = initFileSet(contexts, CONTEXT_DEFAULT_DIR);
        List<FileEntity> contextEntityList  = extractFileList(contextFileSet, "Context");

        // read in context files as YAML. Read in templates using Mustache.

        List<FileEntityContent<Object>>   loadedContexts  = loadContexts(contextEntityList, "Context");
        List<FileEntityContent<Mustache>> loadedTemplates = loadTemplates(templateEntityList, "Template");

        Path outputPath = outputDirectory.toPath();
        if (!Files.isDirectory(outputPath)) {
            try {
                Files.createDirectories(outputPath);
            } catch (IOException e) {
                throw new MojoFailureException("Cannot create output directory: " + outputPath, e);
            }
        }

        for (FileEntityContent<Mustache> template : loadedTemplates) {
            for (FileEntityContent<Object> context : loadedContexts) {
                getLog().info("Applying context " + context.relativePath + " with template " + template.relativePath);
            }
        }

        getLog().info("8=8=8=8=8=8=8=8=8=8=8=8=8 TransformerMojo ENDED 8=8=8=8=8=8=8=8=8=8=8=8=8");
    }

    private List<FileEntityContent<Mustache>> loadTemplates(List<FileEntity> templateEntityList, String name) throws MojoFailureException {
        DefaultMustacheFactory mf = new DefaultMustacheFactory();
        return load(templateEntityList, name, (e, reader) -> mf.compile(reader, e.relativePath.toString()));
    }

    private List<FileEntityContent<Object>> loadContexts(List<FileEntity> contextEntityList, String name) throws MojoFailureException {
        Yaml yaml = new Yaml();
        return load(contextEntityList, name, (e, reader) -> yaml.load(reader));
    }

    private <T> List<FileEntityContent<T>> load(List<FileEntity> entityList, String name, BiFunction<FileEntity, Reader, T> loader) throws MojoFailureException {
        List<FileEntityContent<T>> entityContents = new ArrayList<>();
        for (FileEntity entity : entityList) {
            try (Reader reader = Files.newBufferedReader(entity.fullPath)) {
                T contextObject = loader.apply(entity, reader);
                entityContents.add(entity.withContent(contextObject));
            } catch (IOException e) {
                throw new MojoFailureException("Cannot open " + name + ": " + entity.fullPath, e);
            }
        }
        return entityContents;
    }

    class FileEntity {
        String name;
        Path   fullPath;
        Path   relativePath;

        public FileEntity(String name, Path fullPath, Path relativePath) {
            this.name = name;
            this.fullPath = fullPath;
            this.relativePath = relativePath;
        }

        public <T> FileEntityContent<T> withContent(T content) {
            return new FileEntityContent<T>(name, fullPath, relativePath, content);
        }
    }

    class FileEntityContent<T> extends FileEntity {
        T content;

        public FileEntityContent(String name, Path fullPath, Path relativePath, T content) {
            super(name, fullPath, relativePath);
            this.content = content;
        }
    }

    private FileSet initFileSet(final FileSet fs, final String defaultDir) {
        final FileSet fileSet = fs != null ? fs : new FileSet();

        Path dirToSet;
        if (fileSet.getDirectory() == null) {
            dirToSet = project.getBasedir().toPath().resolve(defaultDir);
            fileSet.setDirectory(dirToSet.toString());
            getLog().info("project.getBasedir: " + project.getBasedir());
            getLog().info("fileSet.getDirectory: " + fileSet.getDirectory());
        }
        return fileSet;
    }

    private List<FileEntity> extractFileList(final FileSet fileSet, String name) throws MojoFailureException {
        final Path rootPath = Path.of(fileSet.getDirectory());
        if (!Files.isDirectory(rootPath)) {
            throw new MojoFailureException(name + " directory not found: " + rootPath);
        }
        if (!Files.isReadable(rootPath)) {
            throw new MojoFailureException(name + " directory not readable: " + rootPath);
        }

        getLog().info("fileSet.getDirectory:\n" + rootPath);

        FileSetManager fileSetManager = new FileSetManager();
        String[]       files          = fileSetManager.getIncludedFiles(fileSet);

        if (failIfNoFiles && files.length == 0) {
            throw new MojoFailureException("No matching " + name + " files found in: " + rootPath);
        }

        getLog().info("#files = " + files.length);
        stream(files).forEach(f -> getLog().info(f));

        return stream(files).map(n -> new FileEntity(name, rootPath.resolve(n), Path.of(n))).collect(toList());
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
