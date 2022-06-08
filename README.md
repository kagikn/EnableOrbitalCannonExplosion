# Enable Orbital Cannon Explosion

This mod enables any scripts for GTA V to the spawn orbital cannon explosion with the native functions `ADD_EXPLOSION`, `ADD_OWNED_EXPLOSION`, and `ADD_EXPLOSION_WITH_USER_VFX`.

These natives will no longer deny spawning an explosion even if the explosion type value is set to `59` (`EXP_TAG_ORBITAL_CANNON`) and the native caller is not `am_mp_defunct_base` or `am_mp_orbital_cannon` (the joaat hashes are `0xCAB8A943` and `0x6C25DCCB` respectively).