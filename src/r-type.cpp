#include "r-type.h"

#include <cstring>
#include <limits.h>
#include <stdlib.h>


// Serialization functions using NetBuffer

void SerializeMissile(Roar::NetBuffer &buffer, const Missile &missile) {
    buffer.WriteFloat(missile.pos.x);
    buffer.WriteFloat(missile.pos.y);
    buffer.WriteFloat(missile.rect.x);
    buffer.WriteFloat(missile.rect.y);
    buffer.WriteFloat(missile.rect.width);
    buffer.WriteFloat(missile.rect.height);
    buffer.WriteUInt32(missile.currentFrame);
    buffer.WriteUInt32(missile.framesSpeed);
    buffer.WriteUInt32(missile.framesCounter);
}

Missile DeserializeMissile(Roar::NetBuffer &buffer) {
    Missile missile;
    missile.pos.x = buffer.ReadFloat();
    missile.pos.y = buffer.ReadFloat();
    missile.rect.x = buffer.ReadFloat();
    missile.rect.y = buffer.ReadFloat();
    missile.rect.width = buffer.ReadFloat();
    missile.rect.height = buffer.ReadFloat();
    missile.currentFrame = buffer.ReadUInt32();
    missile.framesSpeed = buffer.ReadUInt32();
    missile.framesCounter = buffer.ReadUInt32();
    return missile;
}

void SerializeUpdateStateMessage(Roar::NetBuffer &buffer, const UpdateStateMessage &msg) {
    buffer.WriteInt32(msg.x);
    buffer.WriteInt32(msg.y);
    buffer.WriteUInt32(msg.missile_count);

    for (unsigned int i = 0; i < msg.missile_count; i++) {
        SerializeMissile(buffer, msg.missiles[i]);
    }
}

UpdateStateMessage DeserializeUpdateStateMessage(Roar::NetBuffer &buffer) {
    UpdateStateMessage msg;
    msg.x = buffer.ReadInt32();
    msg.y = buffer.ReadInt32();
    msg.missile_count = buffer.ReadUInt32();

    if (msg.missile_count > MAX_MISSILES_CLIENT)
        msg.missile_count = MAX_MISSILES_CLIENT;

    for (unsigned int i = 0; i < msg.missile_count; i++) {
        msg.missiles[i] = DeserializeMissile(buffer);
    }

    return msg;
}

void SerializeClientState(Roar::NetBuffer & buffer, const ClientState &state) {
    buffer.WriteUInt32(state.client_id);
    buffer.WriteInt32(state.x);
    buffer.WriteInt32(state.y);
    buffer.WriteUInt32(state.missile_count);

    for (unsigned int i = 0; i < state.missile_count; i++) {
        SerializeMissile(buffer, state.missiles[i]);
    }
}

ClientState DeserializeClientState(Roar::NetBuffer &buffer) {
    ClientState state;
    state.client_id = buffer.ReadUInt32();
    state.x = buffer.ReadInt32();
    state.y = buffer.ReadInt32();
    state.missile_count = buffer.ReadUInt32();

    if (state.missile_count > MAX_MISSILES_CLIENT)
        state.missile_count = MAX_MISSILES_CLIENT;

    for (unsigned int i = 0; i < state.missile_count; i++) {
        state.missiles[i] = DeserializeMissile(buffer);
    }

    return state;
}

void SerializeMobState(Roar::NetBuffer &buffer, const MobState &mob) {
    buffer.WriteUInt32(mob.mob_id);
    buffer.WriteFloat(mob.x);
    buffer.WriteFloat(mob.y);
    buffer.WriteUInt8(mob.active ? 1 : 0);
}

MobState DeserializeMobState(Roar::NetBuffer &buffer) {
    MobState mob;
    mob.mob_id = buffer.ReadUInt32();
    mob.x = buffer.ReadFloat();
    mob.y = buffer.ReadFloat();
    mob.active = buffer.ReadUInt8() != 0;
    return mob;
}

void SerializeGameStateMessage(Roar::NetBuffer &buffer, const GameStateMessage &msg) {
    buffer.WriteUInt32(msg.client_count);

    for (unsigned int i = 0; i < msg.client_count; i++) {
        SerializeClientState(buffer, msg.client_states[i]);
    }

    // Wave system info
    buffer.WriteFloat(msg.countdown_timer);
    buffer.WriteUInt32(msg.current_wave);
    buffer.WriteUInt8(msg.wave_active ? 1 : 0);
}

GameStateMessage DeserializeGameStateMessage(Roar::NetBuffer &buffer) {
    GameStateMessage msg;
    msg.client_count = buffer.ReadUInt32();

    if (msg.client_count > MAX_CLIENTS)
        msg.client_count = MAX_CLIENTS;

    for (unsigned int i = 0; i < msg.client_count; i++) {
        msg.client_states[i] = DeserializeClientState(buffer);
    }

    // Wave system info
    msg.countdown_timer = buffer.ReadFloat();
    msg.current_wave = buffer.ReadUInt32();
    msg.wave_active = buffer.ReadUInt8() != 0;

    return msg;
}

void SerializeConnectAcceptData(Roar::NetBuffer &buffer, const ConnectAcceptData &data) {
    buffer.WriteUInt32(data.client_id);
    buffer.WriteInt32(data.spawn_x);
    buffer.WriteInt32(data.spawn_y);
}

ConnectAcceptData DeserializeConnectAcceptData(Roar::NetBuffer &buffer) {
    ConnectAcceptData data;
    data.client_id = buffer.ReadUInt32();
    data.spawn_x = buffer.ReadInt32();
    data.spawn_y = buffer.ReadInt32();
    return data;
}
