package org.zcode.model.datamodel;

import java.util.List;

import com.fasterxml.jackson.annotation.JsonSubTypes;
import com.fasterxml.jackson.annotation.JsonSubTypes.Type;
import com.fasterxml.jackson.annotation.JsonTypeInfo;

public interface ZcodeDataModel {
    List<ModuleModel> getModules();

    interface ModuleModel {
        String getName();

        short getId();

        String getVersion();

        String getDescription();

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
        String getName();

        byte getCommand();

        String getDescription();

        OperationType getOperation();

        Extension getExtension();

        List<RequestParamModel> getRequestParams();

        List<ResponseParamModel> getResponseParams();

        List<StatusModel> getStatus();
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
        List<String> getValues();
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
        String getCode();

        int getId();

        String getMeaning();
    }
}
