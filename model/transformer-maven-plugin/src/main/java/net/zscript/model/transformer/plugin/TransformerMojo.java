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
import java.util.stream.Collectors;

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

    private static final String FILE_TYPE_SUFFIX_DEFAULT = "java";

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

    @Parameter(defaultValue = FILE_TYPE_SUFFIX_DEFAULT)
    private String fileTypeSuffix;

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

        final Charset charset;
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

        final FileSet          templateFileSet  = initFileSet(templates, TEMPLATE_DEFAULT_DIR);
        final LoadableEntities templateEntities = extractFileList("Template", templateFileSet);
        final FileSet          contextFileSet   = initFileSet(contexts, CONTEXT_DEFAULT_DIR);
        final LoadableEntities contextEntities  = extractFileList("Context", contextFileSet);

        // read in context files as YAML and perform any field mapping as required. Read in templates ready to use Mustache.

        final List<LoadedEntityContent<Object>>   loadedMappedContexts = loadMappedContexts(contextEntities);
        final List<LoadedEntityContent<Mustache>> loadedTemplates      = loadTemplates(templateEntities);

        final Path outputDirectoryPath = outputDirectory.toPath();

        createDirIfRequired(outputDirectoryPath);

        for (LoadedEntityContent<Mustache> template : loadedTemplates) {
            for (LoadedEntityContent<Object> context : loadedMappedContexts) {
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

        if (fileTypeSuffix.equals(FILE_TYPE_SUFFIX_DEFAULT)) {
//            addSourceRoot(outputDirectoryPath);
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

        getLog().info("fileSet.getDirectory: " + rootPath);

        FileSetManager fileSetManager = new FileSetManager();
        List<Path>     files          = stream(fileSetManager.getIncludedFiles(fileSet)).map(f -> Path.of(f)).collect(Collectors.toList());

        if (failIfNoFiles && files.size() == 0) {
            throw new MojoExecutionException("No matching " + description + " files found in: " + rootPath);
        }

        getLog().info("#files = " + files.size());
        files.forEach(f -> getLog().info(f.toString()));

        return new LoadableEntities(description, rootPath, files, fileTypeSuffix);
    }

    private List<LoadedEntityContent<Object>> loadMappedContexts(LoadableEntities contextEntities) throws MojoExecutionException {
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
            final Path fullPath = entity.getRootPath().resolve(entity.getRelativePath());
            try (final Reader reader = Files.newBufferedReader(fullPath)) {
                final Mustache template = mf.compile(reader, entity.getRelativePath().toString());
                return entity.withContent(template, null);
            } catch (IOException e) {
                throw new UncheckedIOException("Cannot open " + entity.getDescription() + ": " + entity.getRelativePath(), e);
            }
        });
    }

    private void addSourceRoot(Path outputDir) {
        if (generateTestSources) {
            project.addTestCompileSourceRoot(outputDir.toString());
        } else {
            project.addCompileSourceRoot(outputDir.toString());
        }
    }
}
