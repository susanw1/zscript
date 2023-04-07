package org.zcode.javaclient.components;

public class ZcodeValidationException extends RuntimeException {
    public ZcodeValidationException(String message, Object... args) {
        super(message);
    }

    protected static String makeMessage(String message, Object... args) {
        if (args.length == 0) {
            return message;
        }
        StringBuilder sb = new StringBuilder(message).append(" [");
        for (int i = 0, len = args.length; i < len; sb.append(", "), i += 2) {
            sb.append(args[i]).append("=").append(String.valueOf(args[i + 1]));
        }
        return sb.append("]").toString();
    }

}
