package net.zscript.experiment;

import static org.assertj.core.api.Assertions.assertThat;

import io.cucumber.datatable.DataTable;
import io.cucumber.java.Before;
import io.cucumber.java.BeforeStep;
import io.cucumber.java.en.And;
import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;
import org.junit.platform.suite.api.AfterSuite;

public class MyStepdefs {
    private DayMgr dayMgr;
    private String result;

    @Before
    public void xxx() {
        System.out.println("Before...");
        //        org.assertj.core.api.Assumptions.setPreferredAssumptionException(PreferredAssumptionException.JUNIT5);
    }

    @BeforeStep
    public void xxy() {
        System.out.println("BeforeStep...");
    }

    @AfterSuite
    public void xxz1() {
        System.out.println("AfterStep...");
    }

    @Given("today is {word}")
    public void todayIsX(String day) {
        dayMgr = new DayMgr(day);
        System.out.println("Initializing: " + day);
    }

    @Given("today is {word} afternoon")
    public void todayIsAfternoon(String day) {
        todayIsX(day);
        //        org.junit.Assume.assumeFalse(true);
        try {
            //            org.junit.jupiter.api.Assumptions.assumeFalse(true); //org.opentest4j.TestAbortedException
        } catch (Throwable e) {
            e.printStackTrace();
        }
        org.assertj.core.api.Assumptions.assumeThat(true).isFalse(); // org.junit.AssumptionViolatedException

        //        Assume.assumeFalse(true);
        //
        //        Assumptions.assumeThat(12).isEqualTo(13);
        //        AssumptionViolatedException e;
    }

    @When("I ask whether it's {word} yet")
    public void iAskWhetherItsSpecifiedDayYet(String day) {
        result = dayMgr.isItThisDay(day);
    }

    @Then("I should be told {word}")
    public void iShouldBeTold(String day) {
        assertThat(result).isEqualTo(day);
    }

    @Given("a(n) {string}")
    public void aLetter(String letter) {
        System.out.println("Step " + letter);
    }

    @Given("all this")
    public void allThis(String text) {
        System.out.println("All this:  " + text);
    }

    @And("a big table")
    public void aBigTable(DataTable table) {
        System.out.println("Table:  " + table);
        System.out.println("Table.asList:  " + table.asLists());
        //        System.out.println("Table.asMap:  " + table.asMap());
        //        throw new PendingException();
    }

}

class DayMgr {
    String day;

    DayMgr(String day) {
        this.day = day;
    }

    String isItThisDay(String candidate) {
        return day.equals(candidate) ? "Yay" : "Nope";
    }
}
