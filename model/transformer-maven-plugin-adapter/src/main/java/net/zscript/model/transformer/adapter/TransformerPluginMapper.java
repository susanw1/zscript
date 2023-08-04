package net.zscript.model.transformer.adapter;

/**
 * User-defined mapper to convert a requested file/URI content into the form expected by the transform template.
 */
public interface TransformerPluginMapper {

    interface MapperResponse {
        Object getMappedContent();
    }

    /**
     * Performs the required mapping.
     *
     * @param entity
     * @return
     */
    MapperResponse loadAndMap(LoadableEntity entity);
}
