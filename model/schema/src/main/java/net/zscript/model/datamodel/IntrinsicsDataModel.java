package net.zscript.model.datamodel;

import java.util.List;

import net.zscript.model.datamodel.ZscriptDataModel.RequestFieldModel;
import net.zscript.model.datamodel.ZscriptDataModel.ResponseFieldModel;
import net.zscript.model.datamodel.ZscriptDataModel.StatusModel;

public interface IntrinsicsDataModel {
    Intrinsics getIntrinsics();

    interface Intrinsics {
        List<RequestFieldModel> getRequestFields();

        List<ResponseFieldModel> getResponseFields();

        List<ZcharDefs> getZchars();

        List<StatusModel> getStatus();
    }

    interface ZcharDefs {
        String getName();

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

        String getDescription();

        boolean isSeparator();

        boolean isMustEscape();

        boolean isNonNumeric();

        boolean isIgnoreAlways();

        boolean isIgnoreInCode();

        boolean isBigField();
    }
}
