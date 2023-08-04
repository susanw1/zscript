package net.zscript.model.transformer.adapter;

import java.nio.file.Path;

public class LoadableEntity {
    private final String description;
    private final Path   fullPath;
    private final Path   relativePath;

    public LoadableEntity(String description, Path fullPath, Path relativePath) {
        this.description = description;
        this.fullPath = fullPath;
        this.relativePath = relativePath;
    }

    public <T> LoadedEntityContent<T> withContent(T content) {
        return new LoadedEntityContent<T>(description, fullPath, relativePath, content);
    }

    public Path getFullPath() {
        return fullPath;
    }

    public String getDescription() {
        return description;
    }

    public Path getRelativePath() {
        return relativePath;
    }
}
