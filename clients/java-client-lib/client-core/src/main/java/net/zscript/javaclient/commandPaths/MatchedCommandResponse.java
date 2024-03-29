package net.zscript.javaclient.commandPaths;

public class MatchedCommandResponse {
    private final Command  command;
    private final Response response;

    public MatchedCommandResponse(Command command, Response response) {
        this.command = command;
        this.response = response;
    }

    public Command getCommand() {
        return command;
    }

    public Response getResponse() {
        return response;
    }
}
