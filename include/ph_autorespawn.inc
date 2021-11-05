/**
 * Very useful if you need to spawnkill a player
 * if he doesn't need to be respawned from other plugins action.
 *
 * @param value	 Sets a spawnkill to a player while autorespawn is ON.
 * true - The player is spawnkilled and won't be respawned.
 * false - The opposite of true.
 */
native void SpawnKilled(int client, bool value);

/**
 * Very useful if you want a plugin to check,
 * if autorespawn is enabled.
 *
 * @param Returns if respawn is enabled.
 */
native bool RespawnOn();    