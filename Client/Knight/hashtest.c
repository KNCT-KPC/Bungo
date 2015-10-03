// Base: http://amiq11.tumblr.com/post/66345910772/cc-%E3%81%A7%E4%BD%BF%E3%81%88%E3%82%8B-hashtable-booleanlabel
// gcc hashtest.c `pkg-config --cflags glib-2.0`-lglib-2.0 -std=c99

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <glib-2.0/glib.h>
#include <string.h>

static char *keys[] = {
	"aaaa",
	"bbbb",
	"cccc",
	"dddd",
	"eeee",
	"ffff",
	"gggg",
	"hhhh",
	"iiii",
	"jjjj",
	"kkkk",
	"llll",
	"mmmm",
	"nnnn",
	"oooo",
	"pppp",
	"qqqq",
	"rrrr",
	"ssss",
	"tttt",
	"uuuu",
	"vvvv",
	"wwww",
	"xxxx",
	"yyyy",
	"zzzz"
};

void print_hash( gpointer key, gpointer val, __attribute__((unused)) gpointer user_data )
{
	g_print( "%s: %d\n", (char *)key, *(int*)val );
}

int main(int argc, char *argv[])
{
	GHashTable *htab = g_hash_table_new(g_str_hash, g_str_equal);

	char *key;
	int *val;
	for (int i=0; i<26; i++) {
		key = strdup(keys[i]);
		val = g_new(int, 1);
		*val = i;
		g_hash_table_insert(htab, key, val);
	}

	// Dump
	printf("----\n");
	g_hash_table_foreach(htab, print_hash, NULL);

	// Remove
	printf("----\n");
	g_hash_table_remove(htab, keys[0]);
	g_hash_table_foreach(htab, print_hash, NULL);

	// Modify
	printf("----\n");
	val = g_hash_table_lookup(htab, keys[1]);
	*val = 114514;
	g_hash_table_foreach(htab, print_hash, NULL);
	
	// Contain?
	printf("----\n");
	for (int i=0; i<3; i++) {
		printf("Keys[0]: %d\n", g_hash_table_contains(htab, keys[i]));
	}
	
	// Owari
	printf("----\n");
	g_hash_table_destroy(htab);
	
	return 0;
}
