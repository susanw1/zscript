package com.wittsfamily.rcode_acceptance_tests;

import org.junit.runner.RunWith;

import io.cucumber.junit.Cucumber;
import io.cucumber.junit.CucumberOptions;

@RunWith(Cucumber.class)
@CucumberOptions(features = { "classpath:/features/core_rcode.feature", "classpath:/features" })
public class CucumberRunTests {
}
