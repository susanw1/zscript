package net.zscript.model.datamodel;

import java.util.List;

import net.zscript.model.datamodel.ZcodeDataModel.RequestParamModel;
import net.zscript.model.datamodel.ZcodeDataModel.ResponseParamModel;
import net.zscript.model.datamodel.ZcodeDataModel.StatusModel;

public interface IntrinsicsDataModel {
    Intrinsics getIntrinsics();

    interface Intrinsics {
        List<RequestParamModel> getRequestParams();

        List<ResponseParamModel> getResponseParams();

        List<StatusModel> getStatus();
    }
}
