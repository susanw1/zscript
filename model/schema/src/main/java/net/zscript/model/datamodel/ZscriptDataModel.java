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

    enum FieldType {
        bitset,
        number,
        flag,
        bool,
        tristate,
        enums,
        bytes,
        text
    }

    @JsonTypeInfo(use = JsonTypeInfo.Id.NAME, include = JsonTypeInfo.As.PROPERTY, defaultImpl = TypeDefinition.class)
    @JsonSubTypes({
            @Type(value = EnumTypeDefinition.class, name = "enum"),
            @Type(value = BitsetTypeDefinition.class, name = "bitset"),
            @Type(value = CommandsTypeDefinition.class, name = "commands"),
            @Type(value = FlagTypeDefinition.class, name = "flag"),
            @Type(value = NumberTypeDefinition.class, name = "number"),
            @Type(value = BytesTypeDefinition.class, name = "bytes"),
            @Type(value = TextTypeDefinition.class, name = "text"),
            @Type(value = CustomTypeDefinition.class, name = "custom"),
    })
    interface TypeDefinition {
        default boolean isType(String typeName) {
            System.out.println(getClass().getSimpleName());
            System.out.println(typeName + "TypeDefinition");
            return getClass().getSimpleName().toLowerCase().endsWith("$" + typeName + "typedefinition");
        }
    }

    interface EnumTypeDefinition extends TypeDefinition {
        default boolean enumType() {
            return true;
        }

        List<String> getAllowedValues();
    }

    interface BitsetTypeDefinition extends TypeDefinition {
        default boolean bitsetType() {
            return true;
        }

        List<String> getBitFields();
    }

    interface CommandsTypeDefinition extends TypeDefinition {
        List<String> getBitFields();
    }

    interface FlagTypeDefinition extends TypeDefinition {
    }

    interface NumberTypeDefinition extends TypeDefinition {
        default boolean numberType() {
            return true;
        }
    }

    interface BytesTypeDefinition extends TypeDefinition {
    }

    interface TextTypeDefinition extends TypeDefinition {
    }

    interface CustomTypeDefinition extends TypeDefinition {
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
