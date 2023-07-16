package org.zcode.javasimulator;

public class Utility {
	public static String toFixedLengthHex(int i, int length) {
		String s = Integer.toUnsignedString(i, 16);
		if (s.length() >= length) {
			return s;
		} else {
			StringBuilder sb = new StringBuilder(length);

			for (int j = s.length(); j < length; j++) {
				sb.append('0');
			}
			sb.append(s);
			return sb.toString();
		}
	}
}
