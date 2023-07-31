package org.zcode.javasimulator.connections.pin;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import org.zcode.javasimulator.CommunicationPacket;
import org.zcode.javasimulator.CommunicationResponse;
import org.zcode.javasimulator.Connection;
import org.zcode.javasimulator.Entity;
import org.zcode.javasimulator.ProtocolConnection;

public final class PinConnection extends ProtocolConnection<PinProtocolCategory, PinConnection> {
	private class PinPull {
		public final double level;
		public final double strength;

		public PinPull(double level, double strength) {
			this.level = level;
			this.strength = strength;
		}
	}

	public PinConnection(Connection<PinProtocolCategory> connection) {
		super(PinProtocolCategory.class, connection);
	}

	private final Map<Object, PinPull> pulls = new HashMap<>();
	private double state;
	private final Set<Entity> children = new HashSet<>();

	private double calculateNewState() {
		double numerator = 0;
		double denominator = 0;
		for (PinPull pull : pulls.values()) {
			denominator += pull.strength;
			numerator += pull.level * pull.strength;
		}
		double newState = numerator / denominator;
		if (denominator == 0) {
			newState = 0;
		}
		double currentFlow = 0;
		for (PinPull pull : pulls.values()) {
			currentFlow += Math.abs(pull.level - newState) * pull.strength;
		}
		if (currentFlow > 1) {
			System.out.println("WARNING: current flow exceeds 1A");
		}
		return newState;
	}

	private PinStatePacket notifyNewState(Entity source) {
		double newState = calculateNewState();
		PinStatePacket statePacket = new PinStatePacket(newState);
		if (state != newState) {
			state = newState;
			for (Entity entity : children) {
				if (entity != source) {
					entity.receive(connection, statePacket);
				}
			}
		}
		return statePacket;
	}

	@Override
	public CommunicationResponse<PinConnection> sendMessage(Entity source, CommunicationPacket<PinConnection> packet) {
		if (packet.getClass() == PinWritePacket.class) {
			PinWritePacket write = (PinWritePacket) packet;
			if (write.getPullStrength() != 0) {
				pulls.put(source, new PinPull(write.getLevel(), write.getPullStrength()));
			} else {
				pulls.remove(source);
			}
			return notifyNewState(source);
		} else if (packet.getClass() == PinReadPacket.class) {
			return new PinStatePacket(state);
		} else {
			throw new IllegalArgumentException("Unknown packet type for I2C connection: " + packet.getClass().getName());
		}

	}

	@Override
	public void mergeFrom(PinConnection other) {
		children.addAll(other.children);
		pulls.putAll(other.pulls);
		notifyNewState(null);
	}

	public void addChild(Entity e) {
		children.add(e);
	}
}
