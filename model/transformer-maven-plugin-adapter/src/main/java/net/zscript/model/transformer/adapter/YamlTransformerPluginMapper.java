package net.zscript.model.transformer.adapter;

public class YamlTransformerPluginMapper implements TransformerPluginMapper {

    @Override
    public MapperResponse loadAndMap(LoadableEntity entity) {

        return new MapperResponse() {
            @Override
            public Object getMappedContent() {
                return null;
            }
        };
    }
}
