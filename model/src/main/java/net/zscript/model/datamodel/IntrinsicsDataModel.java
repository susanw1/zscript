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

        List<StatusModel> getStatus();
    }
}
