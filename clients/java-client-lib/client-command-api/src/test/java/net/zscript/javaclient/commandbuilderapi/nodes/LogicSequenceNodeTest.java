package net.zscript.javaclient.commandbuilderapi.nodes;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.commandbuilderapi.DemoActivateCommand;
import net.zscript.javaclient.commandbuilderapi.defaultcommands.BlankCommandNode;

class LogicSequenceNodeTest {
    @Test
    public void shouldCreateOrSequence() {
        ZscriptCommandNode<?> node   = DemoCapabilities.builder().build();
        OrSequenceNode        orNode = new OrSequenceNode(node);

        assertThat(orNode.isCommand()).isFalse();
        assertThat(orNode.getParent()).isNull();
    }

    @Test
    public void shouldCreateAndSequence() {
        ZscriptCommandNode<?> node    = DemoCapabilities.builder().build();
        AndSequenceNode       andNode = new AndSequenceNode(node);

        assertThat(andNode.isCommand()).isFalse();
        assertThat(andNode.getParent()).isNull();
    }

    @Test
    public void shouldRejectEmptyNodeList() {
        assertThatThrownBy(OrSequenceNode::new).isInstanceOf(IllegalArgumentException.class).hasMessageContaining("sequence empty");
        assertThatThrownBy(AndSequenceNode::new).isInstanceOf(IllegalArgumentException.class).hasMessageContaining("sequence empty");
    }

    @Test
    public void shouldOptimizeSingleMemberWithOrSequence() {
        ZscriptCommandNode<?> node1  = DemoCapabilities.builder().build(); // can't fail
        ZscriptCommandNode<?> node2  = DemoCapabilities.builder().build(); // can't fail
        OrSequenceNode        orNode = new OrSequenceNode(node1, node2);
        assertThat(orNode.optimize()).isSameAs(node1);
    }

    @Test
    public void shouldOptimizeMultiMembersWithOrSequence() {
        ZscriptCommandNode<?> node1 = DemoActivateCommand.builder().build(); // can fail
        ZscriptCommandNode<?> node2 = DemoCapabilities.builder().build(); // can't fail

        OrSequenceNode orNode1 = new OrSequenceNode(node1, node2);
        assertThat(orNode1.optimize()).containsExactly(node1, node2);
        OrSequenceNode orNode2 = new OrSequenceNode(node2, node1);
        assertThat(orNode2.optimize()).isSameAs(node2);
    }

    @Test
    public void shouldOptimizeEmptyMemberWithAndSequence() {
        ZscriptCommandNode<?> node1 = new BlankCommandNode(); // can't fail
        ZscriptCommandNode<?> node2 = new BlankCommandNode(); // can't fail

        AndSequenceNode andNode1 = new AndSequenceNode(node1, node2);
        assertThat(andNode1.optimize()).doesNotContain(node1, node2).isInstanceOf(BlankCommandNode.class);
    }

    @Test
    public void shouldOptimizeSingleMemberWithAndSequence() {
        ZscriptCommandNode<?> node1 = DemoCapabilities.builder().build(); // can't fail
        ZscriptCommandNode<?> node2 = DemoCapabilities.builder().build(); // can't fail

        AndSequenceNode andNode1 = new AndSequenceNode(node1, node2);
        assertThat(andNode1.optimize()).containsExactly(node1, node2);

        AndSequenceNode andNode2 = new AndSequenceNode(node2, node1);
        assertThat(andNode2.optimize()).containsExactly(node2, node1);
    }

    @Test
    public void shouldOptimizeMultiMembersWithAndSequence() {
        ZscriptCommandNode<?> node1 = DemoActivateCommand.builder().build(); // can fail
        ZscriptCommandNode<?> node2 = DemoCapabilities.builder().build(); // can't fail

        AndSequenceNode andNode1 = new AndSequenceNode(node1, node2);
        assertThat(andNode1.optimize()).containsExactly(node1, node2);

        AndSequenceNode andNode2 = new AndSequenceNode(node2, node1);
        assertThat(andNode2.optimize()).containsExactly(node2, node1);
    }

}
