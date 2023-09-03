package net.zscript.javaclient.connection;

import java.io.IOException;
import java.nio.charset.StandardCharsets;

import net.zscript.javaclient.connectors.ZscriptConnectors;
import net.zscript.javaclient.connectors.ZscriptConnectors.Connector;
import net.zscript.javaclient.connectors.tcp.TcpConnection.TcpConnectionParams;
import net.zscript.javaclient.connectors.tcp.TcpConnector;
import net.zscript.model.modules.base.CoreModule;

class TcpMain {
    public static void main(String[] args) throws IOException {
        Connector<TcpConnectionParams> tcp = ZscriptConnectors.getConnector(TcpConnectionParams.class)
                .orElseThrow();

        TcpConnectionParams params = new TcpConnectionParams("192.168.23.144", 8888);
        try (ZscriptConnection conn = tcp.makeConnection(params)) {
            conn.onReceive((response) -> {
                System.out.println("Response: " + new String(response, StandardCharsets.ISO_8859_1));
            });
            conn.send(CoreModule.echo().any('A', 1234).build().compile(false));
        }
    }
}
