package org.zcode.javaclient.components;

public abstract class ZcodeException extends RuntimeException {
    public ZcodeException(String message, Object... args) {
        super(makeMessage(message, args));
    }

    public ZcodeException(String message, Throwable cause, Object... args) {
        super(makeMessage(message, args), cause);
    }

    protected static String makeMessage(String message, Object... args) {
        if (args.length == 0) {
            return message;
        }
        StringBuilder sb = new StringBuilder(message).append(" [");
        for (int i = 0, len = args.length; i < len; i += 2) {
            if (i > 0) {
                sb.append(", ");
            }
            sb.append(args[i]).append("=").append(String.valueOf(args[i + 1]));
        }
        return sb.append("]").toString();
    }

}
