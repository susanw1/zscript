package net.zscript.demo01.client;

import net.zscript.javaclient.commandbuilder.ZscriptCommandNode;
import net.zscript.model.modules.base.CoreModule;

public class Main {
    public static void main(String[] args) {
        ZscriptCommandNode a = CoreModule.activateBuilder()
                .build();

        //        CoreModule.ActivateCommand.builder()
        //                .build();
        //        CoreModule.CoreCommands.MakeCode.builder();
    }
}
