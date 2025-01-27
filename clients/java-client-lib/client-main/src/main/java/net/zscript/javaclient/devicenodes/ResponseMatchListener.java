package net.zscript.javaclient.devicenodes;

public interface ResponseMatchListener {
    /**
     * Notification that the supplied command has been matched to a response.
     *
     * @param foundCommand the command that has been found
     */
    void responseHasMatched(AddressedCommand foundCommand);
}
