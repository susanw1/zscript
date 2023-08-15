package net.zscript.client.modules;

import net.zscript.client.modules.CoreModule.ActivateCommandBuilder.ActivateCommandResponse;
import net.zscript.client.modules.CoreModule.ChannelInfoCommandBuilder.ChannelInfoCommandResponse;
import net.zscript.client.modules.CoreModule.EchoCommandBuilder.EchoCommandResponse;
import net.zscript.client.modules.CoreModule.MakeCodeCommandBuilder.MakeCodeCommandResponse;
import net.zscript.client.modules.CoreModule.MatchCodeCommandBuilder.MatchCodeCommandResponse;
import net.zscript.client.modules.CoreModule.ReadGuidCommandBuilder.ReadGuidCommandResponse;
import net.zscript.client.modules.CoreModule.ResetCommandBuilder.ResetCommandResponse;
import net.zscript.client.modules.CoreModule.UserDefinedCommandBuilder.UserDefinedCommandResponse;
import net.zscript.javaclient.commandbuilder.ZscriptCommandBuilder;
import net.zscript.javaclient.commandbuilder.ZscriptResponse;
import net.zscript.javaclient.commandbuilder.ZscriptUnparsedCommandResponse;
import net.zscript.javareceiver.tokenizer.Zchars;

/**
 * Core Module: Provides core system commands which can be executed without device activation.
 */
public class CoreModule {
    public static final int    MODULE_ID   = 0;
    public static final String MODULE_NAME = "Core";
    public static final String VERSION     = "0.0.1";
    public static final String DESCRIPTION = "Provides core system commands which can be executed without device activation";

    public static CapabilitiesCommandBuilder capabilities() {
        return new CapabilitiesCommandBuilder();
    }

    /**
     * Top level identification, for hardware, software, modules, versions
     */
    public static class CapabilitiesCommandBuilder extends ZscriptCommandBuilder<CapabilitiesCommandBuilder.CapabilitiesCommandResponse> {

        public static final byte CMD_ID = 0;

        private CapabilitiesCommandBuilder() {
            setField(Zchars.Z_CMD, CMD_ID);
        }

        @Override
        protected boolean commandCanFail() {
            return false;
        }

        @Override
        protected CapabilitiesCommandResponse parseResponse(ZscriptUnparsedCommandResponse resp) {
            return new CapabilitiesCommandResponse(resp);
        }

        public static class CapabilitiesCommandResponse implements ZscriptResponse {
            private final ZscriptUnparsedCommandResponse resp;

            public CapabilitiesCommandResponse(ZscriptUnparsedCommandResponse resp) {
                this.resp = resp;
            }

        }
    }

    public static EchoCommandBuilder echo() {
        return new EchoCommandBuilder();
    }

    /**
     * Echoes all request params to the response, for testing purposes
     */
    public static class EchoCommandBuilder extends ZscriptCommandBuilder<EchoCommandResponse> {

        public static final byte CMD_ID = 1;

        private EchoCommandBuilder() {
            setField(Zchars.Z_CMD, CMD_ID);
        }

        @Override
        protected boolean commandCanFail() {
            return false;
        }

        @Override
        protected EchoCommandResponse parseResponse(ZscriptUnparsedCommandResponse resp) {
            return new EchoCommandResponse(resp);
        }

        public static class EchoCommandResponse implements ZscriptResponse {
            private final ZscriptUnparsedCommandResponse resp;

            public EchoCommandResponse(ZscriptUnparsedCommandResponse resp) {
                this.resp = resp;
            }

        }
    }

    public static ActivateCommandBuilder activate() {
        return new ActivateCommandBuilder();
    }

    /**
     * Activates zscript channel, allowing commands 0x10 onwards to be executed
     */
    public static class ActivateCommandBuilder extends ZscriptCommandBuilder<ActivateCommandResponse> {

        public static final byte CMD_ID = 2;

        private ActivateCommandBuilder() {
            setField(Zchars.Z_CMD, CMD_ID);
        }

        @Override
        protected boolean commandCanFail() {
            return true;
        }

        @Override
        protected ActivateCommandResponse parseResponse(ZscriptUnparsedCommandResponse resp) {
            return new ActivateCommandResponse(resp);
        }

        public static class ActivateCommandResponse implements ZscriptResponse {
            private final ZscriptUnparsedCommandResponse resp;

            public ActivateCommandResponse(ZscriptUnparsedCommandResponse resp) {
                this.resp = resp;
            }

        }
    }

    public static ResetCommandBuilder reset() {
        return new ResetCommandBuilder();
    }

    /**
     * Resets the device(!) or channels. Device reset and *this channel* reset produces no response at all
     */
    public static class ResetCommandBuilder extends ZscriptCommandBuilder<ResetCommandResponse> {

        public static final byte CMD_ID = 3;

        private ResetCommandBuilder() {
            setField(Zchars.Z_CMD, CMD_ID);
        }

        @Override
        protected boolean commandCanFail() {
            return true;
        }

        @Override
        protected ResetCommandResponse parseResponse(ZscriptUnparsedCommandResponse resp) {
            return new ResetCommandResponse(resp);
        }

        public static class ResetCommandResponse implements ZscriptResponse {
            private final ZscriptUnparsedCommandResponse resp;

            public ResetCommandResponse(ZscriptUnparsedCommandResponse resp) {
                this.resp = resp;
            }

        }
    }

    public static ReadGuidCommandBuilder readGuid() {
        return new ReadGuidCommandBuilder();
    }

    /**
     * Retrieves the system GUID, if set
     */
    public static class ReadGuidCommandBuilder extends ZscriptCommandBuilder<ReadGuidCommandResponse> {

        public static final byte CMD_ID = 4;

        private ReadGuidCommandBuilder() {
            setField(Zchars.Z_CMD, CMD_ID);
        }

        @Override
        protected boolean commandCanFail() {
            return true;
        }

        @Override
        protected ReadGuidCommandResponse parseResponse(ZscriptUnparsedCommandResponse resp) {
            return new ReadGuidCommandResponse(resp);
        }

        public static class ReadGuidCommandResponse implements ZscriptResponse {
            private final ZscriptUnparsedCommandResponse resp;

            public ReadGuidCommandResponse(ZscriptUnparsedCommandResponse resp) {
                this.resp = resp;
            }

        }
    }

    public static ChannelInfoCommandBuilder channelInfo() {
        return new ChannelInfoCommandBuilder();
    }

    /**
     * Retrieves channel info
     */
    public static class ChannelInfoCommandBuilder extends ZscriptCommandBuilder<ChannelInfoCommandResponse> {

        public static final byte CMD_ID = 8;

        private ChannelInfoCommandBuilder() {
            setField(Zchars.Z_CMD, CMD_ID);
        }

        @Override
        protected boolean commandCanFail() {
            return false;
        }

        @Override
        protected ChannelInfoCommandResponse parseResponse(ZscriptUnparsedCommandResponse resp) {
            return new ChannelInfoCommandResponse(resp);
        }

        public static class ChannelInfoCommandResponse implements ZscriptResponse {
            private final ZscriptUnparsedCommandResponse resp;

            public ChannelInfoCommandResponse(ZscriptUnparsedCommandResponse resp) {
                this.resp = resp;
            }

        }
    }

    public static MakeCodeCommandBuilder makeCode() {
        return new MakeCodeCommandBuilder();
    }

    /**
     * Generates a random match-code
     */
    public static class MakeCodeCommandBuilder extends ZscriptCommandBuilder<MakeCodeCommandResponse> {

        public static final byte CMD_ID = 12;

        private MakeCodeCommandBuilder() {
            setField(Zchars.Z_CMD, CMD_ID);
        }

        @Override
        protected boolean commandCanFail() {
            return false;
        }

        @Override
        protected MakeCodeCommandResponse parseResponse(ZscriptUnparsedCommandResponse resp) {
            return new MakeCodeCommandResponse(resp);
        }

        public static class MakeCodeCommandResponse implements ZscriptResponse {
            private final ZscriptUnparsedCommandResponse resp;

            public MakeCodeCommandResponse(ZscriptUnparsedCommandResponse resp) {
                this.resp = resp;
            }

        }
    }

    public static MatchCodeCommandBuilder matchCode() {
        return new MatchCodeCommandBuilder();
    }

    /**
     * Returns OK only if supplied code matches the previously generated match-code
     */
    public static class MatchCodeCommandBuilder extends ZscriptCommandBuilder<MatchCodeCommandResponse> {

        public static final byte CMD_ID = 13;

        private MatchCodeCommandBuilder() {
            setField(Zchars.Z_CMD, CMD_ID);
        }

        @Override
        protected boolean commandCanFail() {
            return true;
        }

        @Override
        protected MatchCodeCommandResponse parseResponse(ZscriptUnparsedCommandResponse resp) {
            return new MatchCodeCommandResponse(resp);
        }

        public static class MatchCodeCommandResponse implements ZscriptResponse {
            private final ZscriptUnparsedCommandResponse resp;

            public MatchCodeCommandResponse(ZscriptUnparsedCommandResponse resp) {
                this.resp = resp;
            }

        }
    }

    public static UserDefinedCommandBuilder userDefined() {
        return new UserDefinedCommandBuilder();
    }

    /**
     * Command kept free for user-defined system command
     */
    public static class UserDefinedCommandBuilder extends ZscriptCommandBuilder<UserDefinedCommandResponse> {

        public static final byte CMD_ID = 15;

        private UserDefinedCommandBuilder() {
            setField(Zchars.Z_CMD, CMD_ID);
        }

        @Override
        protected boolean commandCanFail() {
            return false;
        }

        @Override
        protected UserDefinedCommandResponse parseResponse(ZscriptUnparsedCommandResponse resp) {
            return new UserDefinedCommandResponse(resp);
        }

        public static class UserDefinedCommandResponse implements ZscriptResponse {
            private final ZscriptUnparsedCommandResponse resp;

            public UserDefinedCommandResponse(ZscriptUnparsedCommandResponse resp) {
                this.resp = resp;
            }

        }
    }

}
