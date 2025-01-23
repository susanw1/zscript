package net.zscript.javaclient.devicenodes;

import net.zscript.javaclient.addressing.AddressedCommand;

public interface ResponseMatchedListener {
    /**
     * Notification that the supplied command has been matched to a response.
     *
     * @param foundCommand the command that has been found
     */
    void onResponseMatched(AddressedCommand foundCommand);
}
