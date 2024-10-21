package net.zscript.acceptance;

import static io.cucumber.core.options.Constants.GLUE_PROPERTY_NAME;

import org.junit.platform.suite.api.ConfigurationParameter;
import org.junit.platform.suite.api.IncludeEngines;
import org.junit.platform.suite.api.SelectClasspathResource;
import org.junit.platform.suite.api.Suite;

@Suite
@IncludeEngines("cucumber")
@SelectClasspathResource("features")
@SelectClasspathResource("experiment")
@ConfigurationParameter(key = GLUE_PROPERTY_NAME, value = "net.zscript.acceptance, net.zscript.experiment")
public class CucumberRunIT {
}
