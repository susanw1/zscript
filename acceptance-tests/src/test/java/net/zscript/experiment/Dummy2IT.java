package net.zscript.experiment;

import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;

@Disabled
public class Dummy2IT {
    @Test
    public void shouldAssumeDefaultAssertJFail() {
        org.assertj.core.api.Assumptions.assumeThat(false).isTrue();
    }

    @Test
    public void shouldAssumeJunit5Fail() {
        org.junit.jupiter.api.Assumptions.assumeTrue(false);
    }

}
