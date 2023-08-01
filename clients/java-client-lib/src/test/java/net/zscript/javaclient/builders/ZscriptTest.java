package net.zscript.javaclient.builders;

import static org.assertj.core.api.Assertions.assertThat;

import java.util.Optional;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;

import net.zscript.javaclient.builders.Command;
import net.zscript.javaclient.builders.Response;
import net.zscript.javaclient.builders.Zscript;
import net.zscript.model.loader.ModelLoader;

class ZscriptTest {

    @BeforeEach
    void setUp() throws Exception {
    }

    @Disabled
    @Test
    void shouldMakeCommandUsingNames() {
        final ModelLoader modelLoader = ModelLoader.standardModel();
        final Zscript       zscript       = Zscript.forModel(modelLoader);

        final Command c = zscript.forModule("Base", "Core")
                .makeCommand("activate")
                .setField("key", 123)
                .build();

        final Response r = zscript.parseResponse(c, "SA1");

        assertThat(r.getResponseParamByKey('A', Optional.class)).hasValue(1);
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
