package net.zscript.javaclient.commandbuilderapi.nodes;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.util.List;

import static java.util.Collections.emptyList;

import net.zscript.javaclient.commandbuilderapi.Respondable;
import net.zscript.javaclient.commandbuilderapi.ZscriptResponse;
import net.zscript.javaclient.commandbuilderapi.defaultcommands.AbortCommandNode;
import net.zscript.javaclient.commandbuilderapi.defaultcommands.FailureCommandNode;
import net.zscript.javaclient.commandpaths.FieldElement;
import net.zscript.javaclient.commandpaths.ZscriptFieldSet;
import net.zscript.tokenizer.ZscriptExpression;
import net.zscript.util.ByteString.ByteStringBuilder;

/**
 * Defines a command sequence node representing a single command, optionally with a captor. The captor will be given the constructed response object when this command is executed
 * by a Zscript device and matched with a response.
 * <p>
 * A ZscriptCommandNode instance is usually created by calling  {@link ZscriptCommandBuilder#build()} on a suitable implementation.
 * <p>
 * Implementations of this class are either:
 * <ul>
 *     <li> generic "default" commands, used for prettifying the sequence definition process
 *     (see {@link FailureCommandNode}, {@link AbortCommandNode}, etc).</li>
 *     <li> generated commands using the command-builder system, representing (YAML-defined) declarative command/response definitions</li>
 * </ul>
 */
public abstract class ZscriptCommandNode<T extends ZscriptResponse> extends CommandSequenceNode implements Respondable<T> {

    private final ResponseCaptor<T> captor;
    private final ZscriptFieldSet   fieldSet;

    protected ZscriptCommandNode(ZscriptCommandBuilder<T> builder) {
        this(builder.captor, List.copyOf(builder.fields.values()));
    }

    protected ZscriptCommandNode() {
        this(null, emptyList());
    }

    /**
     * Defines a command sequence node representing a single command, optionally with a captor. The captor will be given the constructed response object when this command is
     * executed by a Zscript device and matched with a response.
     *
     * @param captor the captor, for keeping a link to the matching response
     * @param fields the numeric fields associated with this response
     */
    protected ZscriptCommandNode(@Nullable ResponseCaptor<T> captor, List<FieldElement> fields) {
        this.captor = captor;
        this.fieldSet = ZscriptFieldSet.fromList(fields);
        if (captor != null) {
            captor.setSource(this);
        }
    }

    @Nullable
    public final ResponseCaptor<T> getCaptor() {
        return captor;
    }

    @Nonnull
    public abstract T parseResponse(ZscriptExpression response);

    @Nonnull
    public abstract Class<T> getResponseType();

    @Nonnull
    public final List<CommandSequenceNode> getChildren() {
        return emptyList();
    }

    @Nonnull
    public final ZscriptFieldSet asFieldSet() {
        return fieldSet;
    }

    @Override
    public final void appendTo(ByteStringBuilder b) {
        asFieldSet().appendTo(b);
    }
}
