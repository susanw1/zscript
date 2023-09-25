package net.zscript.model.datamodel;

import java.util.List;

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

    /**  */
    interface ModelComponent {
        @JsonProperty(required = true)
        String getName();

        @JsonProperty(required = true)
        String getDescription();

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

        @JsonProperty(required = true)
        short getId();

        @JsonProperty(required = true)
        String getVersion();

        List<String> getPackage();

        @JsonManagedReference
        List<CommandModel> getCommands();

        @JsonManagedReference
        List<NotificationModel> getNotifications();
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
        List<StatusModel> getStatus();

        default int getFullCommand() {
            return getModule().getModuleBank().getId() << 8 | getModule().getId() << 4 | (getCommand() & 0xf);
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
            return getModule().getModuleBank().getId() << 8 | getModule().getId() << 4 | (getNotification() & 0xF);
        }
    }

    enum LogicalTermination {
        END,
        ANDTHEN,
        ORELSE
    }

    @JsonIdentityInfo(generator = ObjectIdGenerators.StringIdGenerator.class)
    interface NotificationSectionNodeModel {
        @JsonBackReference
        NotificationModel getNotification();

        @JsonProperty(required = true)
        NotificationSectionModel getSection();

        @JsonProperty(required = true)
        LogicalTermination getLogicalTermination();
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
            @Type(value = NumberTypeDefinition.class, name = "number"),
            @Type(value = BitsetTypeDefinition.class, name = "bitset"),
            @Type(value = FlagTypeDefinition.class, name = "flag"),
            @Type(value = TextTypeDefinition.class, name = "text"),
            @Type(value = BytesTypeDefinition.class, name = "bytes"),
            @Type(value = CommandsTypeDefinition.class, name = "commands"),
            @Type(value = ModulesTypeDefinition.class, name = "modules"),
            @Type(value = AnyTypeDefinition.class, name = "any"),
            @Type(value = CustomTypeDefinition.class, name = "custom"),
    })
    interface TypeDefinition {
    }

    interface EnumTypeDefinition extends TypeDefinition {
        default boolean enumType() {
            return true;
        }

        List<String> getValues();
    }

    interface BitsetTypeDefinition extends TypeDefinition {
        default boolean bitsetType() {
            return true;
        }

        List<Bit> getBits();

        abstract class Bit implements ModelComponent {
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
    }

    interface NumberTypeDefinition extends TypeDefinition {
        default boolean numberType() {
            return true;
        }

        Integer getMin();

        Integer getMax();
    }

    /**
     * "abstract" type to represent the common behaviour in the big-field related types.
     */
    interface BigfieldTypeDefinition extends TypeDefinition {
        default boolean bigfieldType() {
            return true;
        }

        Integer getMinLength();

        Integer getMaxLength();
    }

    interface BytesTypeDefinition extends BigfieldTypeDefinition {
        default boolean bytesType() {
            return true;
        }
    }

    interface TextTypeDefinition extends BigfieldTypeDefinition {
        default boolean textType() {
            return true;
        }
    }

    interface AnyTypeDefinition extends TypeDefinition {
        default boolean anyType() {
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

    interface StatusModel {
        @JsonBackReference
        CommandModel getCommand();

        String getCode();

        int getId();

        String getMeaning();
    }
}
