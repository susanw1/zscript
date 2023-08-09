package net.zscript.model.transformer.adapter;

import java.nio.file.Path;
import java.util.List;

import net.zscript.model.transformer.adapter.LoadableEntities.LoadedEntityContent;

/**
 * User-defined mapper to convert a requested file/URI content into the form expected by the transform template.
 */
public interface TransformerPluginMapper {

    /**
     * Performs the required mapping.
     *
     * @param entities the entities to map
     * @return a list of mapped contexts to be transformed
     */
    <T> List<LoadedEntityContent<T>> loadAndMap(final LoadableEntities entities);

    /**
     * Utility method for converting a relative path with a dot-suffixed filename into the same path with a different suffix.
     *
     * @param relativePathToSource the path to convert
     * @param suffix               the suffix to add
     * @return the output path
     */
    default Path findRelativePathToOutput(final Path relativePathToSource, String suffix) {
        final String lastPart    = relativePathToSource.getFileName().toString();
        final int    index       = lastPart.lastIndexOf('.');
        final String newLastPart = (index != -1 ? lastPart.substring(0, index) : lastPart) + "." + suffix;
        return relativePathToSource.resolveSibling(newLastPart);
    }
}
