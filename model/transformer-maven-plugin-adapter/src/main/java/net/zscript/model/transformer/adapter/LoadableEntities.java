package net.zscript.model.transformer.adapter;

import static java.util.stream.Collectors.toList;

import java.util.List;
import java.util.Objects;
import java.util.function.Function;

public class LoadableEntities {
    private final String       entityDescription;
    private final String       rootPathName;
    private final List<String> relativePaths;
    private final String       fileTypeSuffix;

    public LoadableEntities(String entityDescription, String rootPathName, String[] relativePaths, String fileTypeSuffix) {
        this.entityDescription = entityDescription;
        this.rootPathName = rootPathName;
        this.relativePaths = List.of(relativePaths);
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
        private final String relativePath;

        public LoadableEntity(String relativePath) {
            this.relativePath = relativePath;
        }

        public String getRelativePath() {
            return relativePath;
        }

        public String getRootPathName() {
            return rootPathName;
        }

        public String getDescription() {
            return entityDescription;
        }

        public String getFileTypeSuffix() {
            return fileTypeSuffix;
        }

        public <T> LoadedEntityContent<T> withContent(T content, String relativeOutputFilename) {
            return new LoadedEntityContent<>(relativePath, content, relativeOutputFilename);
        }
    }

    public class LoadedEntityContent<T> extends LoadableEntity {
        private final T      content;
        private final String relativeOutputFilename;

        public LoadedEntityContent(String relativePath, T content, String relativeOutputPath) {
            super(relativePath);
            this.content = content;
            this.relativeOutputFilename = relativeOutputPath;
        }

        public T getContent() {
            return content;
        }

        public String getRelativeOutputFilename() {
            return relativeOutputFilename;
        }
    }
}
