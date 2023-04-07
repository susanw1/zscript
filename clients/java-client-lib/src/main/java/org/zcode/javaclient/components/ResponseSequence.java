package org.zcode.javaclient.components;

import org.zcode.javaclient.parsing.Address;
import org.zcode.javaclient.parsing.Comment;

public class ResponseSequence extends ZcodeSequence<Response> {
    protected ResponseSequence(Address address, Comment comment, Response... responses) {
        super(address, comment, responses);
    }

}
