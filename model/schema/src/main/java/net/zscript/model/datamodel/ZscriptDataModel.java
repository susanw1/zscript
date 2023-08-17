package net.zscript.model.datamodel;

import java.util.List;

import com.fasterxml.jackson.annotation.JsonBackReference;
import com.fasterxml.jackson.annotation.JsonManagedReference;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonSubTypes;
import com.fasterxml.jackson.annotation.JsonSubTypes.Type;
import com.fasterxml.jackson.annotation.JsonTypeInfo;

import net.zscript.model.loader.ModuleBank;

public interface ZscriptDataModel {

    interface ModuleModel {
        ModuleBank getModuleBank();

        void setModuleBank(ModuleBank moduleBank);

        String getName();

        /**
         * Facilitates disambiguating the name in templating engines. Identical to calling {@link #getName()}.
         */
        default String getModuleName() {
            return getName();
        }

        short getId();

        String getVersion();

        String getDescription();

        List<String> getPackage();

        @JsonManagedReference
        List<CommandModel> getCommands();
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
        CHANNEL,
        ADDRESSING
    }

    interface CommandModel {
        @JsonBackReference
        ModuleModel getModule();

        String getName();

        /**
         * Facilitates disambiguating the name in templating engines. Identical to calling {@link #getName()}.
         */
        default String getCommandName() {
            return getName();
        }

        byte getCommand();

        String getDescription();

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
            return getModule().getModuleBank().getId() << 8 | getModule().getId() << 4 | getCommand();
        }
    }

    @JsonTypeInfo(use = JsonTypeInfo.Id.NAME, include = JsonTypeInfo.As.PROPERTY, defaultImpl = TypeDefinition.class)
    @JsonSubTypes({
            @Type(value = EnumTypeDefinition.class, name = "enum"),
            @Type(value = NumberTypeDefinition.class, name = "number"),
            @Type(value = BitsetTypeDefinition.class, name = "bitset"),
            @Type(value = FlagTypeDefinition.class, name = "flag"),
            @Type(value = TextTypeDefinition.class, name = "text"),
            @Type(value = BytesTypeDefinition.class, name = "bytes"),
            @Type(value = CommandsTypeDefinition.class, name = "commands"),
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

        interface Bit {
            String getName();

            String getDescription();
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
    }

    interface BytesTypeDefinition extends TypeDefinition {
        default boolean bytesType() {
            return true;
        }
    }

    interface TextTypeDefinition extends TypeDefinition {
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

    interface CustomTypeDefinition extends TypeDefinition {
        default boolean customType() {
            return true;
        }
    }

    interface GenericField {
        @JsonBackReference
        CommandModel getCommand();

        char getKey();

        String getName();

        String getDescription();

        TypeDefinition getTypeDefinition();

        boolean isRequired();
    }

    interface RequestFieldModel extends GenericField {
    }

    interface ResponseFieldModel extends GenericField {
    }

    interface StatusModel {
        @JsonBackReference
        CommandModel getCommand();

        String getCode();

        int getId();

        String getMeaning();
    }
}
