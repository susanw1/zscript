package net.zscript.javasimulator;

public abstract class EntityController {
	protected final Entity entity;

	public EntityController() {
		this.entity = new Entity(this);
	}

	public Entity getEntity() {
		return entity;
	}

	// U is the connection category, T is the specific connection type
	public abstract <U extends ProtocolCategory, T extends ProtocolConnection<U, T>> CommunicationResponse<T> acceptIncoming(Class<U> type,
			int index, CommunicationPacket<T> packet);
}
