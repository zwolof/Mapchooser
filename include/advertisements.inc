#if defined _advertisements_included
  #endinput
#endif
#define _advertisements_included

/**
 * Gets a client's color as a hexadecimal integer.
 * 
 * @param strType		Type of advertisement
 * @param strText		Text to display
 * @param strFlags		Required admin flag(s) to view the advertisement
 * @noreturn
 */
native RegisterAdvertisement(String:strType[], String:strText[], String:strFlags[] = "none");

/**
 * Called when the advertisement configuration file is loaded
 * 
 * @noreturn
 */
forward OnAdvertisementsLoaded(client);

public SharedPlugin:__pl_ccc = 
{
	name = "advertisements",
	file = "advertisements.smx",
#if defined REQUIRE_PLUGIN
	required = 1
#else
	required = 0
#endif
};

#if !defined REQUIRE_PLUGIN
public __pl_ccc_SetNTVOptional() 
{
	MarkNativeAsOptional("RegisterAdvertisement");
}
#endif