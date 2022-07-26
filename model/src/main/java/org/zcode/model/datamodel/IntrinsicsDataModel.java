package org.zcode.model.datamodel;

import java.util.List;

import org.zcode.model.datamodel.ZcodeDataModel.RequestParam;
import org.zcode.model.datamodel.ZcodeDataModel.ResponseParam;
import org.zcode.model.datamodel.ZcodeDataModel.Status;

public interface IntrinsicsDataModel {
    Intrinsics getIntrinsics();

    interface Intrinsics {
        List<RequestParam> getRequestParams();

        List<ResponseParam> getResponseParams();

        List<Status> getStatus();
    }
}
