package net.zscript.javaclient.connection;

import java.net.Socket;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.connectors.ZscriptConnectors;
import net.zscript.javaclient.connectors.ZscriptConnectors.ZscriptConnector;
import net.zscript.javaclient.nodes.DirectConnection;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.javaclient.tokens.ExtendingTokenBuffer;
import net.zscript.model.modules.base.CoreModule;
import net.zscript.util.ByteString;

class TcpMain {
    public static void main(String[] args) throws Exception {
        ZscriptConnector<Socket> connector = ZscriptConnectors.TCP;

        Socket s = new Socket("192.168.23.144", 8888);

        try (DirectConnection conn = connector.connect(s)) {
            conn.onReceive((response) -> {
                System.out.println("Response: " + response.toString());
            });

            final ByteString     command = CoreModule.echoBuilder().setAny('A', 35).build().toByteString();
            ExtendingTokenBuffer buffer  = ExtendingTokenBuffer.tokenize(command);
            conn.send(new AddressedCommand(CommandSequence.from(CommandExecutionPath.parse(buffer.getTokenReader().getFirstReadToken()), -1)));
        }
    }
}
