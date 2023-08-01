package net.zscript.model.datamodel;

import java.util.List;

import com.fasterxml.jackson.annotation.JsonBackReference;
import com.fasterxml.jackson.annotation.JsonManagedReference;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonSubTypes;
import com.fasterxml.jackson.annotation.JsonSubTypes.Type;

import net.zscript.model.loader.ModuleBank;

import com.fasterxml.jackson.annotation.JsonTypeInfo;

public interface ZscriptDataModel {
    List<ModuleModel> getModules();

    interface ModuleModel {
        ModuleBank getModuleBank();

        void setModuleBank(ModuleBank moduleBank);

        String getName();

        short getId();

        String getVersion();

        String getDescription();

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

        byte getCommand();

        String getDescription();

        OperationType getOperation();

        Extension getExtension();

        @JsonManagedReference
        @JsonProperty(required = true)
        List<RequestParamModel> getRequestParams();

        @JsonManagedReference
        @JsonProperty(required = true)
        List<ResponseParamModel> getResponseParams();

        @JsonManagedReference
        List<StatusModel> getStatus();

        default int getFullCommand() {
            return getModule().getModuleBank().getId() << 8 | getModule().getId() << 4 | getCommand();
        }
    }

    enum ParamType {
        bitset,
        number,
        flag,
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
    }

    interface EnumTypeDefinition extends TypeDefinition {
        List<String> getAllowedValues();
    }

    interface BitsetTypeDefinition extends TypeDefinition {
        List<String> getBitFields();
    }

    interface CommandsTypeDefinition extends TypeDefinition {
        List<String> getBitFields();
    }

    interface FlagTypeDefinition extends TypeDefinition {
    }

    interface NumberTypeDefinition extends TypeDefinition {
    }

    interface BytesTypeDefinition extends TypeDefinition {
    }

    interface TextTypeDefinition extends TypeDefinition {
    }

    interface CustomTypeDefinition extends TypeDefinition {
    }

    interface GenericParam {
        @JsonBackReference
        CommandModel getCommand();

        char getLabel();

        String getName();

        String getDescription();

        TypeDefinition getTypeDefinition();

        boolean isRequired();
    }

    interface RequestParamModel extends GenericParam {
    }

    interface ResponseParamModel extends GenericParam {
    }

    interface StatusModel {
        @JsonBackReference
        CommandModel getCommand();

        String getCode();

        int getId();

        String getMeaning();
    }
}
