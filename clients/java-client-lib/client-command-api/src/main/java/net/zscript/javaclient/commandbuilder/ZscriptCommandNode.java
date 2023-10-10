package net.zscript.javaclient.commandbuilder;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import net.zscript.javareceiver.tokenizer.ZscriptExpression;
import net.zscript.model.components.Zchars;

public abstract class ZscriptCommandNode extends CommandSequenceNode {

    /**
     * Performed on a command when no response was found in the received response sequence that corresponds to this command. Indicates the command was skipped due to ANDTHEN/ORELSE
     * logic.
     */
    public void onNotExecuted() {
    }

    /**
     * Performs a command's listener callbacks when given a command's response.
     *
     * @param response the response that we've received corresponding to this command
     */
    public void onResponse(ZscriptExpression response) {
    }

    public List<CommandSequenceNode> getChildren() {
        return Collections.emptyList();
    }

}
