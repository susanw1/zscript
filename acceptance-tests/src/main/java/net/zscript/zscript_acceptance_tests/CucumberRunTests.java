package net.zscript.zscript_acceptance_tests;

import org.junit.runner.RunWith;

import io.cucumber.junit.Cucumber;
import io.cucumber.junit.CucumberOptions;

@RunWith(Cucumber.class)
@CucumberOptions(features = { "classpath:/features/core_zscript.feature", "classpath:/features" })
public class CucumberRunTests {
}
