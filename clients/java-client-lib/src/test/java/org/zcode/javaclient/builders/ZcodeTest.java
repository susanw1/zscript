package org.zcode.javaclient.builders;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;
import org.zcode.javaclient.components.Command;
import org.zcode.javaclient.components.Response;
import org.zcode.model.loader.ModelLoader;

class ZcodeTest {

    @BeforeEach
    void setUp() throws Exception {
    }

    @Disabled
    @Test
    void shouldMakeCommandUsingNames() {
        final ModelLoader modelLoader = ModelLoader.standardModel();
        final Zcode       zcode       = Zcode.forModel(modelLoader);

        final Command c = zcode.forModule("Base", "Core")
                .makeCommand("activate")
                .setField("key", 123)
                .build();

        final Response r = zcode.parseResponse(c, "SA1");

//        assertThat(r.getResponseParamByKey('A', Optional.class)).hasValue(1);
    }

//    // TODO
//    @Test
//    void shouldMakeCommandUsingReflection() {
//        final ModelLoader modelLoader = ModelLoader.standardModel();
//        final Zcode       zcode       = Zcode.forModel(modelLoader);
//
//        final Command c = zcode.forModule(Core.class)
//                .activate()
//                .key(123)
//                .build();
//
//        final Response r = zcode.parseResponse(c, "SA1");
//
//        assertThat(r.getResponseParam("previousActivationState", Optional.class)).hasValue(1);
//    }

}
