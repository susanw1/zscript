package org.zcode.javareceiver_acceptancetests;

import org.junit.runner.RunWith;

import io.cucumber.junit.Cucumber;
import io.cucumber.junit.CucumberOptions;

@RunWith(Cucumber.class)
@CucumberOptions(features = "classpath:/features", glue = { "org.zcode.zcode_acceptance_tests", "org.zcode.javareceiver_acceptancetests" })
public class CucumberRunTests {
}
