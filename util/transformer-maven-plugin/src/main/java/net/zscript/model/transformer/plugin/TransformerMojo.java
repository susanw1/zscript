package net.zscript.model.transformer.plugin;

import static java.util.Arrays.stream;

import java.io.File;
import java.io.IOException;
import java.io.Reader;
import java.io.Writer;
import java.net.URI;
import java.net.URISyntaxException;
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
    private static final String OUTPUT_DEFAULT_DIR   = "${project.build.directory}/generated-sources/java";

    private static final String FILE_TYPE_SUFFIX_DEFAULT = "java";

    /**
     * A fileset describing a set of templates (ie mustache files) to apply for each context file.
     */
    @Parameter(required = false)
    private FileSet templates;

    /**
     * A fileset describing a set of context files (ie JSON/YAML files for the default transformer). Defaults to src/main/contexts. If the directory element does not correspond to
     * an existing directory, then it will be checked as a URL, allowing "classpath:" scheme. Note, only specific &lt;include> tags with relative paths are supported with URLs - no
     * wildcards, no excludes etc.
     */
    @Parameter(required = false)
    private FileSet contexts;

    /**
     * Specify output directory where the transformed output files are placed. This is added to the Maven Compile Source Root list if the {@code fileTypeSuffix} is "java".
     */
    @Parameter(defaultValue = OUTPUT_DEFAULT_DIR)
    private File outputDirectory;

    /**
     * The fully-qualified classmame of a TransformerPluginMapper to use for loading and mapping the files described by the {@code contexts}. Changing this allows you to perform
     * arbitrary transformations from any file-type you can read.
     */
    @Parameter(defaultValue = "net.zscript.model.transformer.adapter.YamlTransformerPluginMapper")
    private String transformMapperClass;

    /**
     * Not used.
     */
    @Parameter(defaultValue = "${project.build.sourceEncoding}")
    private String encoding;

    /**
     * If true, then an empty template/context fileset is considered an error.
     */
    @Parameter(defaultValue = "true")
    private boolean failIfNoFiles;

    /**
     * The file suffix to add to output files. If "java", then the output directory is added to the compile/test scope.
     */
    @Parameter(defaultValue = FILE_TYPE_SUFFIX_DEFAULT)
    private String fileTypeSuffix;

    /**
     * Specifies whether sources are added to the {@code compile} scope, or not. If unset, sources will be generated if fileTypeSuffix is ".java".
     */
    @Parameter(property = "transformer.generateSources")
    private Boolean generateSources;

    @Parameter(defaultValue = "${project}", readonly = true, required = true)
    private MavenProject project;

    @Override
    public void execute() throws MojoExecutionException, MojoExecutionException {
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
        final List<LoadedEntityContent> loadedMappedContexts = loadMappedContexts(contextEntities);
        final List<LoadedEntityContent> loadedTemplates      = loadTemplates(templateEntities);

        final Path outputDirectoryPath = outputDirectory.toPath();
        createDirIfRequired(outputDirectoryPath);

        for (LoadedEntityContent template : loadedTemplates) {
            for (LoadedEntityContent context : loadedMappedContexts) {
                try {
                    final Path outputFileFullPath = outputDirectoryPath.resolve(context.getRelativeOutputPath());
                    final Path outputParentDir    = outputFileFullPath.getParent();
                    createDirIfRequired(outputParentDir);

                    final Mustache mustache = (Mustache) template.getContents().get(0);

                    getLog().info("Applying context " + context.getRelativePath() + " with template " + template.getRelativePath() + " to " + outputFileFullPath);
                    try (final Writer out = Files.newBufferedWriter(outputFileFullPath)) {
                        mustache.execute(out, context.getContents());
                    }
                } catch (final IOException e) {
                    throw new MojoExecutionException("Cannot write output file: " + outputDirectoryPath, e);
                }
            }
        }

        if (Boolean.TRUE.equals(generateSources) || generateSources == null && fileTypeSuffix.equals(FILE_TYPE_SUFFIX_DEFAULT)) {
//            if (generateTestSources) {
//                project.addTestCompileSourceRoot(outputDirectoryPath.toString());
//            } else {
            project.addCompileSourceRoot(outputDirectoryPath.toString());
//            }
        }
    }

    private void createDirIfRequired(final Path outputDirectoryPath) throws MojoExecutionException {
        if (!Files.isDirectory(outputDirectoryPath)) {
            try {
                getLog().debug("Creating output directory: " + outputDirectoryPath);
                Files.createDirectories(outputDirectoryPath);
            } catch (final IOException e) {
                throw new MojoExecutionException("Cannot create output directory: " + outputDirectoryPath, e);
            }
        }
    }

    private FileSet initFileSet(final FileSet fs, final String defaultDir) {
        final FileSet fileSet = fs != null ? fs : new FileSet();

        final Path dirToSet;
        if (fileSet.getDirectory() == null) {
            dirToSet = project.getBasedir().toPath().resolve(defaultDir);
            fileSet.setDirectory(dirToSet.toString());
        }
        return fileSet;
    }

    private LoadableEntities extractFileList(String description, final FileSet fileSet) throws MojoExecutionException {
        final String directoryString = fileSet.getDirectory();

        URI rootUri;
        try {
            rootUri = new URI(directoryString);
            if (rootUri.getScheme() != null) {
                getLog().debug(description + ": directory is valid URI, so assuming using limited includes paths: " + directoryString);
                return new LoadableEntities(description, rootUri, fileSet.getIncludes(), fileTypeSuffix);
            }
        } catch (URISyntaxException e) {
            getLog().debug(description + ": directory isn't valid URI, so assuming local directory: " + directoryString);
        }

        return extractFileListAsLocalFiles(description, fileSet, Path.of(directoryString));
    }

    private LoadableEntities extractFileListAsLocalFiles(String description, final FileSet fileSet, Path rootPath) throws MojoExecutionException {
        URI rootUri;
        if (!Files.isDirectory(rootPath)) {
            throw new MojoExecutionException(description + " directory not found: " + rootPath);
        }
        if (!Files.isReadable(rootPath)) {
            throw new MojoExecutionException(description + " directory not readable: " + rootPath);
        }
        rootUri = rootPath.toUri();

        getLog().debug("    " + description + ": fileSet.getDirectory: " + rootPath + "; rootUri: " + rootUri);

        final FileSetManager fileSetManager = new FileSetManager();
        final List<String>   files          = stream(fileSetManager.getIncludedFiles(fileSet)).collect(Collectors.toList());

        if (failIfNoFiles && files.size() == 0) {
            throw new MojoExecutionException("No matching " + description + " files found in: " + rootPath);
        }

        getLog().debug("    #files = " + files.size());
        files.forEach(f -> getLog().debug("    " + f.toString()));

        return new LoadableEntities(description, rootUri, files, fileTypeSuffix);
    }

    private List<LoadedEntityContent> loadMappedContexts(LoadableEntities contextEntities) throws MojoExecutionException {
        final TransformerPluginMapper mapper;
        try {
            mapper = (TransformerPluginMapper) Class.forName(transformMapperClass).getDeclaredConstructor().newInstance();
        } catch (final Exception e) {
            throw new MojoExecutionException("Cannot load class '" + transformMapperClass + "'", e);
        }
        return mapper.loadAndMap(contextEntities);
    }

    private List<LoadedEntityContent> loadTemplates(LoadableEntities entities) {
        final DefaultMustacheFactory mf = new DefaultMustacheFactory();

        return entities.loadEntities(entity -> {
            final Path fullPath = Path.of(entity.getFullPath());
            try (final Reader reader = Files.newBufferedReader(fullPath)) {
                final Mustache template = mf.compile(reader, entity.getRelativePath().toString());
                return List.of(entity.withContents(List.of(template), Path.of(entity.getRelativePath())));
            } catch (final IOException e) {
                throw new TransformerMojoFailureException("Cannot open " + entity.getDescription() + ": " + entity.getRelativePath(), e);
            }
        });
    }

    static class TransformerMojoFailureException extends RuntimeException {
        public TransformerMojoFailureException(String msg, Exception e) {
            super(msg, e);
        }

        public TransformerMojoFailureException(String msg) {
            super(msg);
        }
    }
}
