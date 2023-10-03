package net.zscript.model.templating.plugin;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.Writer;
import java.net.MalformedURLException;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

import static java.nio.charset.StandardCharsets.UTF_8;
import static java.util.Arrays.stream;

import com.github.mustachejava.DefaultMustacheFactory;
import com.github.mustachejava.Mustache;
import com.github.mustachejava.MustacheFactory;
import org.apache.maven.plugin.AbstractMojo;
import org.apache.maven.plugin.MojoExecutionException;
import org.apache.maven.plugins.annotations.Parameter;
import org.apache.maven.project.MavenProject;
import org.apache.maven.shared.model.fileset.FileSet;
import org.apache.maven.shared.model.fileset.util.FileSetManager;

import net.zscript.model.templating.adapter.LoadableEntities;
import net.zscript.model.templating.adapter.LoadableEntities.LoadedEntityContent;
import net.zscript.model.templating.adapter.TemplatingPluginMapper;

abstract class TemplatingBaseMojo extends AbstractMojo {
    private static final String FILE_TYPE_SUFFIX_DEFAULT = "java";

    @Parameter(required = true)
    protected String mainTemplate;
    //    /**
    //     * A fileset describing a set of templates (ie mustache files) to apply for each context file.
    //     */
    //    @Parameter
    //    protected FileSet additionalTemplates;

    /**
     * A fileset describing a set of context files (ie JSON/YAML files for the default transformer). Defaults to src/main/contexts. If the directory element does not correspond to
     * an existing directory, then it will be checked as a URL, allowing "classpath:" scheme. Note, only specific &lt;include> tags with relative paths are supported with URLs - no
     * wildcards, no excludes etc.
     */
    @Parameter
    protected FileSet contexts;

    /**
     * Specify output directory where the transformed output files are placed. This is added to the Maven Compile Source Root list if the {@code fileTypeSuffix} is "java".
     */
    @Parameter
    protected File outputDirectory;

    /**
     * The fully-qualified classmame of a TemplatingPluginMapper to use for loading and mapping the files described by the {@code contexts}. Changing this allows you to perform
     * arbitrary transformations from any file-type you can read.
     */
    @Parameter(defaultValue = "net.zscript.model.templating.adapter.YamlTemplatingPluginMapper")
    protected String templatingMapperClass;

    /**
     * If true, then an empty template/context fileset is considered an error.
     */
    @Parameter(defaultValue = "true")
    protected boolean failIfNoFiles;

    /**
     * The file suffix to add to output files. If "java", then the output directory is added to the compile/test scope.
     */
    @Parameter(defaultValue = FILE_TYPE_SUFFIX_DEFAULT)
    protected String fileTypeSuffix;

    /**
     * Specifies whether sources are added to the {@code compile} scope, or not. If unset, sources will be generated if fileTypeSuffix is ".java".
     */
    @Parameter
    protected String generateSources;

    @Parameter(defaultValue = "${project}", readonly = true, required = true)
    protected MavenProject project;

    protected final List<String> templateDefaultDirs = new ArrayList<>();

    public String executeBase(String contextDefaultDir, String outputDefaultDir) throws MojoExecutionException {
        final URL mainTemplateUrl = getMainTemplateUrl(project.getBasedir().toPath(), templateDefaultDirs, mainTemplate);

        //        if (additionalTemplates != null && !templateDefaultDirs.isEmpty()) {
        //            final FileSet                   templateFileSet           = initFileSet(additionalTemplates, templateDefaultDirs.get(0));
        //            final LoadableEntities          templateEntities          = extractFileList("AdditionalTemplate", templateFileSet);
        //            final List<LoadedEntityContent> additionalLoadedTemplates = loadTemplates(templateEntities);
        //            getLog().info("additionalLoadedTemplates.size: " + additionalLoadedTemplates.size());
        //        }
        final FileSet          contextFileSet  = initFileSet(contexts, contextDefaultDir);
        final LoadableEntities contextEntities = extractFileList("Context", contextFileSet);

        // read in context files as YAML and perform any field mapping as required. Read in templates ready to use Mustache.
        final List<LoadedEntityContent> loadedMappedContexts = loadMappedContexts(contextEntities);
        getLog().info("outputDir: " + outputDirectory);

        if (outputDirectory == null) {
            outputDirectory = new File(project.getBuild().getDirectory(), outputDefaultDir);
        }
        getLog().info("outputDir: " + outputDirectory);

        final Path outputDirectoryPath = outputDirectory.toPath();
        createDirIfRequired(outputDirectoryPath);

        for (LoadedEntityContent context : loadedMappedContexts) {
            try {
                final Path outputFileFullPath = outputDirectoryPath.resolve(context.getRelativeOutputPath());
                final Path outputParentDir    = outputFileFullPath.getParent();
                createDirIfRequired(outputParentDir);

                final Mustache mustache = loadTemplate(new DefaultMustacheFactory(), mainTemplateUrl, mainTemplateUrl.getPath());
                getLog().info("Applying context " + context.getRelativePath() + " with template " + mainTemplateUrl + " to " + outputFileFullPath);
                try (final Writer out = Files.newBufferedWriter(outputFileFullPath)) {
                    mustache.execute(out, context.getContents());
                }
            } catch (final IOException e) {
                throw new MojoExecutionException("Failed to generate output file: " + outputDirectoryPath, e);
            }
        }

        if (Boolean.parseBoolean(generateSources) || generateSources == null && fileTypeSuffix.equals(FILE_TYPE_SUFFIX_DEFAULT)) {
            return outputDirectoryPath.toString();
        }

        return null;
    }

    private URL getMainTemplateUrl(Path baseDir, List<String> templateDefaultDirs, String mainTemplatePath) {
        try {
            final URI mainTemplateUri = new URI(mainTemplatePath);
            if (mainTemplateUri.getScheme() == null) {
                // Template is a local file
                for (String defaultDir : templateDefaultDirs) {
                    final Path resolvedDir = baseDir.resolve(defaultDir);
                    if (!Files.isDirectory(resolvedDir)) {
                        continue;
                    }
                    final Path resolvedPath = resolvedDir.resolve(mainTemplatePath);
                    if (Files.isRegularFile(resolvedPath)) {
                        System.out.println("mainTemplatePath: " + mainTemplatePath + ", Resolved path: " + resolvedPath);
                        return resolvedPath.toUri().toURL();
                    }
                }
                throw new TemplatingMojoFailureException("Cannot locate template: " + mainTemplatePath);
            } else if (mainTemplateUri.getScheme().equals("classpath")) {
                // Template is a classpath resource
                return getClass().getResource(mainTemplateUri.getPath());
            } else {
                // Template is some other kind of URL
                return mainTemplateUri.toURL();
            }
        } catch (URISyntaxException | MalformedURLException e) {
            throw new TemplatingMojoFailureException("Bad URI: " + mainTemplatePath, e);
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

        try {
            URI rootUri = new URI(directoryString);
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

        if (failIfNoFiles && files.isEmpty()) {
            throw new MojoExecutionException("No matching " + description + " files found in: " + rootPath);
        }

        getLog().debug("    #files = " + files.size());
        files.forEach(f -> getLog().debug("    " + f));

        return new LoadableEntities(description, rootUri, files, fileTypeSuffix);
    }

    private List<LoadedEntityContent> loadMappedContexts(LoadableEntities contextEntities) throws MojoExecutionException {
        final TemplatingPluginMapper mapper;
        try {
            mapper = (TemplatingPluginMapper) Class.forName(templatingMapperClass).getDeclaredConstructor().newInstance();
        } catch (final Exception e) {
            throw new MojoExecutionException("Cannot load class '" + templatingMapperClass + "'", e);
        }
        return mapper.loadAndMap(contextEntities);
    }

    private Mustache loadTemplate(MustacheFactory mustacheFactory, URL fullTemplateUrl, String relativePath) throws IOException {
        try (final Reader reader = new BufferedReader(new InputStreamReader(fullTemplateUrl.openStream(), UTF_8))) {
            return mustacheFactory.compile(reader, relativePath);
        }
    }

    //    private List<LoadedEntityContent> loadTemplates(LoadableEntities templateEntities) {
    //        final DefaultMustacheFactory mf = new DefaultMustacheFactory();
    //
    //        return templateEntities.loadEntities(templateEntity -> {
    //            try {
    //                Mustache template = loadTemplate(mf, templateEntity.getFullPathAsUrl(), templateEntity.getRelativePath());
    //                return List.of(templateEntity.withContents(List.of(template), Path.of(templateEntity.getRelativePath())));
    //            } catch (MalformedURLException e) {
    //                throw new TemplatingMojoFailureException("URL error " + templateEntity.getFullPath() + ": " + templateEntity.getRelativePath(), e);
    //            } catch (IOException e) {
    //                throw new TemplatingMojoFailureException("Cannot open " + templateEntity.getDescription() + ": " + templateEntity.getRelativePath(), e);
    //            }
    //        });
    //    }

    static class TemplatingMojoFailureException extends RuntimeException {
        public TemplatingMojoFailureException(String msg, Exception e) {
            super(msg, e);
        }

        public TemplatingMojoFailureException(String msg) {
            super(msg);
        }
    }
}
