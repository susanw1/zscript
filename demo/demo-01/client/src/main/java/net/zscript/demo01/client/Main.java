package net.zscript.demo01.client;

import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandNode;
import net.zscript.model.modules.base.CoreModule;

public class Main {
    public static void main(String[] args) {
        ZscriptCommandNode a = CoreModule.activateBuilder()
                .build();
        CoreModule.ActivateCommand.Builder b = CoreModule.activateBuilder();//whatever I always need to do
        //        if(condition) {
        //            ZscriptCommandNode cmd1 = b.setChallenge(3).build();
        //        }
        //        ZscriptCommandNode cmd2 = b.build();
        //        CoreModule.ActivateCommand.builder()
        //                .build();
        //        CoreModule.CoreCommands.MakeCode.builder();
    }
}
