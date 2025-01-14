package net.zscript.javaclient.commandprinting;

import static net.zscript.util.ByteString.byteStringUtf8;

import net.zscript.ascii.AnsiCharacterStylePrinter;
import net.zscript.javaclient.commandpaths.CommandExecutionPath;
import net.zscript.javaclient.commandpaths.ResponseExecutionPath;
import net.zscript.javaclient.tokens.ExtendingTokenBuffer;

public class Main {
    public static void main(String[] args) {
        CommandGraph.GraphPrintSettings basicSettings = new CommandGraph.GraphPrintSettings(
                new StandardCommandGrapher.CommandPrintSettings("  ", VerbositySetting.FIELDS), true, 2, 80);

        ExtendingTokenBuffer bufferCmd = ExtendingTokenBuffer.tokenize(byteStringUtf8("Z1 A1 & Z1 B2 | Z1 C3"), false);
        CommandExecutionPath cmdPath   = CommandExecutionPath.parse(bufferCmd.getTokenReader().getFirstReadToken());

        ExtendingTokenBuffer  bufferResp = ExtendingTokenBuffer.tokenize(byteStringUtf8("SA1 & S1 | SC3"), false);
        ResponseExecutionPath respPath   = ResponseExecutionPath.parse(bufferResp.getTokenReader().getFirstReadToken());

        String output = new StandardCommandGrapher().graph(cmdPath, null, basicSettings)
                .generateString(new AnsiCharacterStylePrinter());
        System.out.println(output);
    }
}
