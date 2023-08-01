package net.zscript.javareceiver_acceptancetests;

import org.junit.runner.RunWith;

import io.cucumber.junit.Cucumber;
import io.cucumber.junit.CucumberOptions;

@RunWith(Cucumber.class)
@CucumberOptions(features = "classpath:/features", glue = { "net.zscript.zscript_acceptance_tests", "net.zscript.javareceiver_acceptancetests" })
public class CucumberRunTests {
}
