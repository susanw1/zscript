package net.zscript.model.transformer.adapter;

import static java.util.stream.Collectors.toList;

import java.nio.file.Path;
import java.util.List;
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

    public <T> List<LoadedEntityContent<T>> loadEntities(Function<LoadableEntity, List<LoadedEntityContent<T>>> loader) {
        return relativePaths.stream()
                .map(LoadableEntity::new)
                .map(loader::apply)
                .flatMap(list -> list.stream())
                .collect(toList());
    }

    public class LoadableEntity {
        private final Path relativePath;

        public LoadableEntity(Path relativePath) {
            if (relativePath.isAbsolute()) {
                throw new IllegalArgumentException("relativePath is absolute: " + relativePath);
            }
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

        public Path getFullPath() {
            return rootPath.resolve(relativePath);
        }

        public <T> LoadedEntityContent<T> withContent(final T content, final Path relativeOutputFilename) {
            return new LoadedEntityContent<>(relativePath, content, relativeOutputFilename);
        }
    }

    public class LoadedEntityContent<T> extends LoadableEntity {
        private final T    content;
        private final Path relativeOutputPath;

        public LoadedEntityContent(Path relativePath, T content, Path relativeOutputPath) {
            super(relativePath);

            this.content = content;

            if (relativeOutputPath.isAbsolute()) {
                throw new IllegalArgumentException("relativeOutputPath is absolute: " + relativeOutputPath);
            }
            this.relativeOutputPath = relativeOutputPath;
        }

        public T getContent() {
            return content;
        }

        public Path getRelativeOutputPath() {
            return relativeOutputPath;
        }
    }
}
