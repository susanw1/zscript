package net.zscript.model.datamodel;

import java.util.List;

import com.fasterxml.jackson.annotation.JsonIdentityInfo;
import com.fasterxml.jackson.annotation.ObjectIdGenerators;

import net.zscript.model.datamodel.ZscriptDataModel.RequestFieldModel;
import net.zscript.model.datamodel.ZscriptDataModel.ResponseFieldModel;

public interface IntrinsicsDataModel {
    Intrinsics getIntrinsics();

    interface Intrinsics {
        List<RequestFieldModel> getRequestFields();

        List<ResponseFieldModel> getResponseFields();

        List<ZcharDefs> getZchars();

        List<StatusModel> getStatus();

        List<StatusCategoryModel> getStatusCategories();
    }

    interface ZcharDefs extends ZscriptDataModel.ModelComponent {
        char getCh();

        default String getChEscapedAsC() {
            char ch = getCh();
            switch (ch) {
            case '\0':
                return "\\0";
            case '\n':
                return "\\n";
            case '\t':
                return "\\t";
            case '\r':
                return "\\r";
            }
            return String.valueOf(ch);
        }

        boolean isSeparator();

        boolean isMustEscape();

        boolean isNonNumeric();

        boolean isIgnoreAlways();

        boolean isIgnoreInCode();

        boolean isBigField();

        boolean isAddressing();
    }

    @JsonIdentityInfo(generator = ObjectIdGenerators.StringIdGenerator.class)
    interface StatusCategoryModel extends ZscriptDataModel.ModelComponent {
        int getBaseId();
    }

    interface StatusModel extends ZscriptDataModel.ModelComponent {
        int getId();

        StatusCategoryModel getCategory();
    }
}
