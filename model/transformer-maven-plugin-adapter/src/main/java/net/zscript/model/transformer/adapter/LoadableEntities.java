package net.zscript.model.transformer.adapter;

import static java.util.stream.Collectors.toList;

import java.nio.file.Path;
import java.util.List;
import java.util.Objects;
import java.util.function.Function;

public class LoadableEntities {
    private final String     entityDescription;
    private final Path       rootPath;
    private final List<Path> relativePaths;
    private final String     fileTypeSuffix;

    public LoadableEntities(String entityDescription, Path rootPath, List<Path> relativePaths, String fileTypeSuffix) {
        this.entityDescription = entityDescription;
        this.rootPath = rootPath;
        this.relativePaths = relativePaths;
        this.fileTypeSuffix = fileTypeSuffix;

    }

    public <T> List<LoadedEntityContent<T>> loadEntities(Function<LoadableEntity, LoadedEntityContent<T>> loader) {
        return relativePaths.stream()
                .map(LoadableEntity::new)
                .map(loader::apply)
                .filter(Objects::nonNull)
                .collect(toList());
    }

    public class LoadableEntity {
        private final Path relativePath;

        public LoadableEntity(Path relativePath) {
            this.relativePath = relativePath;
        }

        public Path getRelativePath() {
            return relativePath;
        }

        public Path getRootPath() {
            return rootPath;
        }

        public String getDescription() {
            return entityDescription;
        }

        public String getFileTypeSuffix() {
            return fileTypeSuffix;
        }

        public <T> LoadedEntityContent<T> withContent(T content, Path relativeOutputFilename) {
            return new LoadedEntityContent<>(relativePath, content, relativeOutputFilename);
        }
    }

    public class LoadedEntityContent<T> extends LoadableEntity {
        private final T    content;
        private final Path relativeOutputFilename;

        public LoadedEntityContent(Path relativePath, T content, Path relativeOutputPath) {
            super(relativePath);
            this.content = content;
            this.relativeOutputFilename = relativeOutputPath;
        }

        public T getContent() {
            return content;
        }

        public Path getRelativeOutputFilename() {
            return relativeOutputFilename;
        }
    }
}
