package com.wittsfamily.rcode.javareceiver_acceptancetests;

import org.junit.runner.RunWith;

import io.cucumber.junit.Cucumber;
import io.cucumber.junit.CucumberOptions;

@RunWith(Cucumber.class)
@CucumberOptions(features = "classpath:/features", glue = { "com.wittsfamily.rcode_acceptance_tests", "com.wittsfamily.rcode.javareceiver_acceptancetests" })
public class CucumberRunTests {
}
