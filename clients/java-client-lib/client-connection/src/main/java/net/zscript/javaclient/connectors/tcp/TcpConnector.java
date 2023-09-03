package net.zscript.javaclient.connectors.tcp;

import java.io.IOException;

import net.zscript.javaclient.connection.ZscriptConnection;
import net.zscript.javaclient.connectors.ZscriptConnectors;
import net.zscript.javaclient.connectors.ZscriptConnectors.Connector;
import net.zscript.javaclient.connectors.tcp.TcpConnection.TcpConnectionParams;

public class TcpConnector implements Connector<TcpConnectionParams> {
    public TcpConnector() {
        ZscriptConnectors.register(this);
    }

    public enum TcpType implements ZscriptConnectors.ConnectorType {
        TCP
    }

    @Override
    public String getType() {
        return "TCP";
    }

    @Override
    public Class<TcpConnectionParams> getConnectorParamsClass() {
        return TcpConnectionParams.class;
    }

    @Override
    public ZscriptConnection makeConnection(TcpConnectionParams params) throws IOException {
        return new TcpConnection(params);
    }
}
