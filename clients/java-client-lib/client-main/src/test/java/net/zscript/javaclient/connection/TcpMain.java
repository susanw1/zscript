package net.zscript.javaclient.connection;

import java.net.Socket;
import java.nio.charset.StandardCharsets;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.connectors.ZscriptConnectors;
import net.zscript.javaclient.connectors.ZscriptConnectors.ZscriptConnector;
import net.zscript.javaclient.nodes.DirectConnection;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.javaclient.tokens.ExtendingTokenBuffer;
import net.zscript.model.modules.base.CoreModule;
import net.zscript.tokenizer.Tokenizer;

class TcpMain {
    public static void main(String[] args) throws Exception {
        ZscriptConnector<Socket> connector = ZscriptConnectors.TCP;

        Socket s = new Socket("192.168.23.144", 8888);

        try (DirectConnection conn = connector.connect(s)) {
            conn.onReceive((response) -> {
                System.out.println("Response: " + response.toString());
            });
            ExtendingTokenBuffer buffer = new ExtendingTokenBuffer();
            Tokenizer            t      = new Tokenizer(buffer.getTokenWriter(), 2);
            for (byte b : CoreModule.echoBuilder().setAny('A', 1234).build().asString().getBytes(StandardCharsets.UTF_8)) {
                t.accept(b);
            }
            conn.send(new AddressedCommand(CommandSequence.from(CommandExecutionPath.parse(buffer.getTokenReader().getFirstReadToken()), -1)));
        }
    }
}
