package net.zscript.model.datamodel;

import java.util.List;

import net.zscript.model.datamodel.ZscriptDataModel.RequestParamModel;
import net.zscript.model.datamodel.ZscriptDataModel.ResponseParamModel;
import net.zscript.model.datamodel.ZscriptDataModel.StatusModel;

public interface IntrinsicsDataModel {
    Intrinsics getIntrinsics();

    interface Intrinsics {
        List<RequestParamModel> getRequestParams();

        List<ResponseParamModel> getResponseParams();

        List<StatusModel> getStatus();
    }
}
