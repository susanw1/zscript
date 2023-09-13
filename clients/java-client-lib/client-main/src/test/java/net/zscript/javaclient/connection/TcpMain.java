package net.zscript.javaclient.connection;

import java.io.IOException;
import java.net.Socket;
import java.nio.charset.StandardCharsets;

import net.zscript.javaclient.connectors.ZscriptConnectors;
import net.zscript.javaclient.connectors.ZscriptConnectors.ZscriptConnector;
import net.zscript.model.modules.base.CoreModule;

class TcpMain {
    public static void main(String[] args) throws IOException {
        ZscriptConnector<Socket> connector = ZscriptConnectors.TCP;

        Socket s = new Socket("192.168.23.144", 8888);

        try (ZscriptConnection conn = connector.connect(s)) {
            conn.onReceive((response) -> {
                System.out.println("Response: " + new String(response, StandardCharsets.ISO_8859_1));
            });
            conn.send(CoreModule.echo().any('A', 1234).build().compile());
        }
    }
}
