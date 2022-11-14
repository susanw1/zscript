package org.zcode.model.datamodel;

import java.util.List;

import org.zcode.model.datamodel.ZcodeDataModel.RequestParamModel;
import org.zcode.model.datamodel.ZcodeDataModel.ResponseParamModel;
import org.zcode.model.datamodel.ZcodeDataModel.StatusModel;

public interface IntrinsicsDataModel {
    Intrinsics getIntrinsics();

    interface Intrinsics {
        List<RequestParamModel> getRequestParams();

        List<ResponseParamModel> getResponseParams();

        List<StatusModel> getStatus();
    }
}
