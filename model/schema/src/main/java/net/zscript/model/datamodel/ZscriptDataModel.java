package net.zscript.model.datamodel;

import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;

import static java.util.Arrays.stream;

import com.fasterxml.jackson.annotation.JsonBackReference;
import com.fasterxml.jackson.annotation.JsonIdentityInfo;
import com.fasterxml.jackson.annotation.JsonManagedReference;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonSubTypes;
import com.fasterxml.jackson.annotation.JsonSubTypes.Type;
import com.fasterxml.jackson.annotation.JsonTypeInfo;
import com.fasterxml.jackson.annotation.ObjectIdGenerators;

import net.zscript.model.loader.ModuleBank;

public interface ZscriptDataModel {

    /**
     * Most components of the Model have a name/description/longDescription - this defines them in one place.
     */
    interface ModelComponent {
        @JsonProperty(required = true)
        String getName();

        @JsonProperty(required = true)
        String getDescription();

        @JsonProperty
        String getLongDescription();
    }

    interface ModuleModel extends ModelComponent {
        @JsonBackReference
        ModuleBank getModuleBank();

        void setModuleBank(ModuleBank moduleBank);

        /**
         * Facilitates disambiguating the name in templating engines. Identical to calling {@link #getName()}.
         */
        default String getModuleName() {
            return getName();
        }

        /** The module ID of this Module, within this module's bank. Value 0x0-0xf */
        @JsonProperty(required = true)
        short getId();

        /** The full module ID of this Module, including the module's bank. Value 0x000-0xfff */
        default int getFullModuleId() {
            return (getModuleBank().getId() << 4) | getId();
        }

        /** Utility to calculate full ID of a member of this Module (eg command or notification), given its code. Value 0x0000-0xffff */
        default int getMemberId(int code) {
            if ((code & ~0xf) != 0) {
                throw new IllegalArgumentException("out of range 0-15: " + code);
            }
            return (getFullModuleId() << 4) | code;
        }

        @JsonProperty(required = true)
        String getVersion();

        List<String> getPackage();

        @JsonManagedReference
        List<CommandModel> getCommands();

        @JsonManagedReference
        List<NotificationModel> getNotifications();

        default Collection<NotificationSectionModel> getNotificationSections() {
            Map<String, NotificationSectionModel> sections = new HashMap<>();
            for (NotificationModel model : getNotifications()) {
                for (NotificationSectionNodeModel node : model.getSections()) {
                    sections.put(node.getSection().getSectionName(), node.getSection());
                }
            }
            return sections.values();
        }

        default Optional<? extends CommandModel> getCommandById(int id) {
            for (CommandModel c : getCommands()) {
                if (c.getCommand() == id) {
                    return Optional.of(c);
                }
            }
            return Optional.empty();
        }

        default Optional<? extends NotificationModel> getNotificationById(int id) {
            for (NotificationModel n : getNotifications()) {
                if (n.getNotification() == id) {
                    return Optional.of(n);
                }
            }
            return Optional.empty();
        }
    }

    /** Characterises commands in quasi-HTTP terms */
    enum OperationType {
        /** non-mutating, idempotent - eg fetching capabilities doesn't change anything and can be retried freely */
        GET,
        /** non-mutating, not idempotent - eg destructive read, like taking next byte from a channel */
        READ,
        /** mutating, idempotent - eg setting params, can be retried freely */
        PUT,
        /** mutating, not idempotent, eg writing data to a stream */
        POST,
        /** not known */
        UNKNOWN
    }

    enum Extension {
        /** Supports a command channel, so it can be addressed by an upstream device. */
        CHANNEL,
        /** Supports a downstream command connector to talk to sub-devices. */
        ADDRESSING
    }

    interface CommandModel extends ModelComponent {
        @JsonBackReference
        ModuleModel getModule();

        /**
         * Facilitates disambiguating the name in templating engines. Identical to calling {@link #getName()}.
         */
        default String getCommandName() {
            return getName();
        }

        @JsonProperty(required = true)
        byte getCommand();

        @JsonProperty(required = true)
        OperationType getOperation();

        Extension getExtension();

        @JsonManagedReference
        @JsonProperty(required = true)
        List<RequestFieldModel> getRequestFields();

        @JsonManagedReference
        @JsonProperty(required = true)
        List<ResponseFieldModel> getResponseFields();

        @JsonManagedReference
        List<CommandStatusModel> getStatus();

        default int getFullCommand() {
            return getModule().getMemberId(getCommand());
        }
    }

    interface NotificationModel extends ModelComponent {
        @JsonBackReference
        ModuleModel getModule();

        /**
         * Facilitates disambiguating the name in templating engines. Identical to calling {@link #getName()}.
         */
        default String getNotificationName() {
            return getName();
        }

        @JsonProperty(required = true)
        byte getNotification();

        @JsonProperty(required = true)
        String getCondition();

        @JsonManagedReference
        @JsonProperty(required = true)
        List<NotificationSectionNodeModel> getSections();

        default int getFullNotification() {
            return getModule().getMemberId(getNotification());
        }
    }

    @JsonIdentityInfo(generator = ObjectIdGenerators.StringIdGenerator.class)
    interface NotificationSectionNodeModel {
        @JsonBackReference
        NotificationModel getNotification();

        @JsonProperty(required = true)
        NotificationSectionModel getSection();
    }

    @JsonIdentityInfo(generator = ObjectIdGenerators.StringIdGenerator.class)
    interface NotificationSectionModel extends ModelComponent {
        /**
         * Facilitates disambiguating the name in templating engines. Identical to calling {@link #getName()}.
         */
        default String getSectionName() {
            return getName();
        }

        @JsonManagedReference
        @JsonProperty(required = true)
        List<NotificationFieldModel> getFields();
    }

    @JsonIdentityInfo(generator = ObjectIdGenerators.StringIdGenerator.class)
    @JsonTypeInfo(use = JsonTypeInfo.Id.NAME, defaultImpl = TypeDefinition.class)
    @JsonSubTypes({
            @Type(value = EnumTypeDefinition.class, name = "enum"),
            @Type(value = Uint16TypeDefinition.class, name = "uint16"),
            @Type(value = Uint32TypeDefinition.class, name = "uint32"),
            @Type(value = BitsetTypeDefinition.class, name = "bitset"),
            @Type(value = FlagTypeDefinition.class, name = "flag"),
            @Type(value = TextTypeDefinition.class, name = "text"),
            @Type(value = BytesTypeDefinition.class, name = "bytes"),
            @Type(value = CmdTypeDefinition.class, name = "cmd"),
            @Type(value = CommandsTypeDefinition.class, name = "commands"),
            @Type(value = ModulesTypeDefinition.class, name = "modules"),
            @Type(value = AnyTypeDefinition.class, name = "any"),
            @Type(value = CustomTypeDefinition.class, name = "custom"),
    })
    interface TypeDefinition {
        /**
         * A one-word description of the type of this field. It should be the same as the @Type field.
         *
         * @return a one-word description of the type of this field
         */
        default String getType() {
            // Inefficient linear search - alas the generated bean Class doesn't know which @Type matched. It's used in code generation, so ok for now.
            final Type[] types = TypeDefinition.class.getAnnotation(JsonSubTypes.class).value();
            return stream(types)
                    .filter(t -> t.value().isAssignableFrom(getClass())) // generated class implements respective interface
                    .findAny()
                    .map(Type::name)
                    .orElse("unknown");
        }

        /**
         * Determines whether the type code generation should include a 'hasXxxx()' field-check method (generally only for fields where 'required' is false). Overridden by specific
         * types. Default is false.
         *
         * @return true if check method should be included for non-required fields, false otherwise
         */
        default boolean includeFieldCheck() {
            return false;
        }
    }

    interface EnumTypeDefinition extends TypeDefinition {
        default boolean enumType() {
            return true;
        }

        default boolean includeFieldCheck() {
            return true;
        }

        List<String> getValues();
    }

    interface BitsetTypeDefinition extends TypeDefinition {
        default boolean bitsetType() {
            return true;
        }

        default boolean includeFieldCheck() {
            return true;
        }

        List<Bit> getBits();

        abstract class Bit implements ModelComponent {
            /**
             * Facilitates disambiguating the name in templating engines. Identical to calling {@link #getName()}.
             */
            public String getBitName() {
                return getName();
            }

            @Override
            public String toString() {
                return "Bit:[" + getName() + "]";
            }
        }
    }

    interface FlagTypeDefinition extends TypeDefinition {
        default boolean flagType() {
            return true;
        }

        default boolean includeFieldCheck() {
            return true;
        }
    }

    interface NumberTypeDefinition extends TypeDefinition {
        default boolean numberType() {
            return true;
        }

        default boolean includeFieldCheck() {
            return true;
        }
    }

    interface Uint16TypeDefinition extends NumberTypeDefinition {
        default boolean uint16Type() {
            return true;
        }

        /** Indicates the smallest acceptable value (default: 0, range: 0-0xffff, max >= min) */
        Integer getMin();

        /** Indicates the largest acceptable value (default: 0xffff, range: 0-0xffff, max >= min) */
        Integer getMax();
    }

    interface Uint32TypeDefinition extends NumberTypeDefinition {
        default boolean uint32Type() {
            return true;
        }

        /** Indicates the smallest acceptable value (default: 0, range: 0-0xffff_ffff, max >= min) */
        Long getMin();

        /** Indicates the largest acceptable value (default: 0xffff_ffff, range: 0-0xffff_ffff, max >= min) */
        Long getMax();
    }

    /**
     * "abstract" type to represent the common behaviour in the string-related types (bytes and text).
     */
    interface StringTypeDefinition extends TypeDefinition {
        default boolean stringType() {
            return true;
        }

        default boolean includeFieldCheck() {
            return true;
        }

        Integer getMinLength();

        Integer getMaxLength();
    }

    interface BytesTypeDefinition extends StringTypeDefinition {
        default boolean bytesType() {
            return true;
        }
    }

    interface TextTypeDefinition extends StringTypeDefinition {
        default boolean textType() {
            return true;
        }
    }

    interface AnyTypeDefinition extends TypeDefinition {
        default boolean anyType() {
            return true;
        }
    }

    /**
     * Special type for the 'Z' field - doesn't generate setter methods in code generation.
     */
    interface CmdTypeDefinition extends TypeDefinition {
        default boolean cmdType() {
            return true;
        }
    }

    interface CommandsTypeDefinition extends TypeDefinition {
        default boolean commandsType() {
            return true;
        }
    }

    interface ModulesTypeDefinition extends TypeDefinition {
        default boolean modulesType() {
            return true;
        }
    }

    interface CustomTypeDefinition extends TypeDefinition {
        default boolean customType() {
            return true;
        }
    }

    interface GenericField extends ModelComponent {
        /**
         * Facilitates disambiguating the name in templating engines. Identical to calling {@link #getName()}.
         */
        default String getFieldName() {
            return getName();
        }

        char getKey();

        TypeDefinition getTypeDefinition();

        boolean isRequired();
    }

    abstract class AbstractCommandFieldModel implements GenericField, Cloneable {
        @JsonBackReference
        public abstract CommandModel getCommand();

        public abstract void setCommand(CommandModel commandModel);

        public AbstractCommandFieldModel clone() {
            try {
                return (AbstractCommandFieldModel) super.clone();
            } catch (CloneNotSupportedException e) {
                throw new InternalError(e);
            }
        }
    }

    abstract class RequestFieldModel extends AbstractCommandFieldModel {
    }

    abstract class ResponseFieldModel extends AbstractCommandFieldModel {
    }

    interface NotificationFieldModel extends GenericField {
        @JsonBackReference
        NotificationSectionModel getNotificationSection();
    }

    /**
     * Specifically a Status object that is associated with a Command, as opposed to an intrinsic one
     */
    interface CommandStatusModel extends IntrinsicsDataModel.StatusModel {
        @JsonBackReference
        CommandModel getCommand();
    }
}
