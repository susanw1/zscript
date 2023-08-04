package net.zscript.model.transformer.adapter;

import java.nio.file.Path;

public class LoadedEntityContent<T> extends LoadableEntity {
    T content;

    LoadedEntityContent(String name, Path fullPath, Path relativePath, T content) {
        super(name, fullPath, relativePath);
        this.content = content;
    }
}
