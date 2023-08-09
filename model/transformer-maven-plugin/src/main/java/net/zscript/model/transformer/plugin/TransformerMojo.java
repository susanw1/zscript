package net.zscript.model.transformer.plugin;

import static java.util.Arrays.stream;

import java.io.File;
import java.io.IOException;
import java.io.Reader;
import java.io.UncheckedIOException;
import java.io.Writer;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;

import org.apache.maven.plugin.AbstractMojo;
import org.apache.maven.plugin.MojoExecutionException;
import org.apache.maven.plugins.annotations.LifecyclePhase;
import org.apache.maven.plugins.annotations.Mojo;
import org.apache.maven.plugins.annotations.Parameter;
import org.apache.maven.project.MavenProject;
import org.apache.maven.shared.model.fileset.FileSet;
import org.apache.maven.shared.model.fileset.util.FileSetManager;

import com.github.mustachejava.DefaultMustacheFactory;
import com.github.mustachejava.Mustache;

import net.zscript.model.transformer.adapter.LoadableEntities;
import net.zscript.model.transformer.adapter.LoadableEntities.LoadedEntityContent;
import net.zscript.model.transformer.adapter.TransformerPluginMapper;

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

    @Parameter(defaultValue = "net.zscript.model.transformer.adapter.YamlTransformerPluginMapper")
    private String transformMapperClass;

//    TransformerPluginMapper m;
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
    public void execute() throws MojoExecutionException, MojoExecutionException {
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

        final FileSet    templateFileSet  = initFileSet(templates, TEMPLATE_DEFAULT_DIR);
        LoadableEntities templateEntities = extractFileList("Template", templateFileSet);
        final FileSet    contextFileSet   = initFileSet(contexts, CONTEXT_DEFAULT_DIR);
        LoadableEntities contextEntities  = extractFileList("Context", contextFileSet);

        // read in context files as YAML. Read in templates using Mustache.

        List<LoadedEntityContent<Object>>   loadedContexts  = loadContexts(contextEntities);
        List<LoadedEntityContent<Mustache>> loadedTemplates = loadTemplates(templateEntities);

        Path outputDirectoryPath = outputDirectory.toPath();
        createDirIfRequired(outputDirectoryPath);

        for (LoadedEntityContent<Mustache> template : loadedTemplates) {
            for (LoadedEntityContent<Object> context : loadedContexts) {
                try {
                    final Path outputFileFullPath = outputDirectoryPath.resolve(context.getRelativeOutputFilename());
                    Path       parentDir          = outputFileFullPath.getParent();
                    createDirIfRequired(parentDir);

                    getLog().info("Applying context " + context.getRelativePath() + " with template " + template.getRelativePath() + " to " + outputFileFullPath);
                    try (Writer out = Files.newBufferedWriter(outputFileFullPath)) {
                        template.getContent().execute(out, context.getContent());
                    }
                } catch (IOException e) {
                    throw new MojoExecutionException("Cannot write output file: " + outputDirectoryPath, e);
                }
            }
        }

        getLog().info("8=8=8=8=8=8=8=8=8=8=8=8=8 TransformerMojo ENDED 8=8=8=8=8=8=8=8=8=8=8=8=8");
    }

    private void createDirIfRequired(Path outputDirectoryPath) throws MojoExecutionException {
        if (!Files.isDirectory(outputDirectoryPath)) {
            try {
                getLog().info("Creating output directory: " + outputDirectoryPath);
                Files.createDirectories(outputDirectoryPath);
            } catch (IOException e) {
                throw new MojoExecutionException("Cannot create output directory: " + outputDirectoryPath, e);
            }
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

    private LoadableEntities extractFileList(String description, final FileSet fileSet) throws MojoExecutionException {
        final Path rootPath = Path.of(fileSet.getDirectory());
        if (!Files.isDirectory(rootPath)) {
            throw new MojoExecutionException(description + " directory not found: " + rootPath);
        }
        if (!Files.isReadable(rootPath)) {
            throw new MojoExecutionException(description + " directory not readable: " + rootPath);
        }

        getLog().info("fileSet.getDirectory:\n" + rootPath);

        FileSetManager fileSetManager = new FileSetManager();
        String[]       files          = fileSetManager.getIncludedFiles(fileSet);

        if (failIfNoFiles && files.length == 0) {
            throw new MojoExecutionException("No matching " + description + " files found in: " + rootPath);
        }

        getLog().info("#files = " + files.length);
        stream(files).forEach(f -> getLog().info(f));

        return new LoadableEntities(description, rootPath.toString(), files);
    }

    private List<LoadedEntityContent<Object>> loadContexts(LoadableEntities contextEntities) throws MojoExecutionException {
        TransformerPluginMapper mapper;
        try {
            mapper = (TransformerPluginMapper) Class.forName(transformMapperClass).getDeclaredConstructor().newInstance();
        } catch (Exception e) {
            throw new MojoExecutionException("Cannot load class '" + transformMapperClass + "'");
        }
        return mapper.loadAndMap(contextEntities);
    }

    private <T> List<LoadedEntityContent<Mustache>> loadTemplates(LoadableEntities entities) {
        DefaultMustacheFactory mf = new DefaultMustacheFactory();

        return entities.loadEntities(entity -> {
            final Path fullPath = Path.of(entity.getRootPathName(), entity.getRelativePath());
            try (final Reader reader = Files.newBufferedReader(fullPath)) {
                final Mustache template = mf.compile(reader, entity.getRelativePath().toString());
                return entity.withContent(template, "");
            } catch (IOException e) {
                throw new UncheckedIOException("Cannot open " + entity.getDescription() + ": " + entity.getRelativePath(), e);
            }
        });
    }

    void addSourceRoot(File outputDir) {
        if (generateTestSources) {
            project.addTestCompileSourceRoot(outputDir.getPath());
        } else {
            project.addCompileSourceRoot(outputDir.getPath());
        }
    }

//    private void runTemplateConfiguration(Object globalContext, TemplateRunConfiguration configuration, Charset charset)
//            throws MojoExecutionException, MojoExecutionException {
//        Object templateContext = createContext(configuration.getContext(), charset);
//        if (templateContext == null) {
//            if (globalContext == null) {
//                throw new MojoExecutionException("Template has no defined context and plugin context is also empty");
//            }
//            templateContext = globalContext;
//        }
//
//        Mustache mustache   = createTemplate(configuration.getTemplateFile(), charset);
//        File     outputFile = new File(configuration.getOutputPath());
//        File     parent     = outputFile.getParentFile();
//        if (!parent.exists() || parent.mkdirs()) {
//            throw new MojoExecutionException("Output directory cannot be created: " + parent);
//        }
//        try (Writer writer = new OutputStreamWriter(new FileOutputStream(outputFile), charset)) {
//            mustache.execute(writer, templateContext);
//        } catch (IOException e) {
//            throw new MojoExecutionException(e, "Cannot open output file", "Cannot open output file: " + e.getMessage());
//        } catch (MustacheException e) {
//            throw new MojoExecutionException(e, "Cannot process template", "Cannot process template: " + e.getMessage());
//        }
//    }
//
//    private Object createContext(String contextConfiguration, Charset charset) throws MojoExecutionException {
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
//    private static String obtainContextSource(String contextConfiguration, Charset charset) throws MojoExecutionException {
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
//                throw new MojoExecutionException(e, "Cannot load yaml from file", "Cannot load yaml from file");
//            }
//        }
//
//        throw new MojoExecutionException("Cannot load context. Either pass a filename in the form 'file:[filename]' or " +
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
