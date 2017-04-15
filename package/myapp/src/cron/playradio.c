#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <json-c/json.h>


#define RADIO_LIST	"/etc/radio.list"


static char* json_to_str(const char *str)
{
	char *ptr = strstr(str, "\\/");
	if(ptr == NULL)
	{
		printf("url ok!\n");
		return NULL;
	}

	int size = strlen(str);
	char *buf = (char *)malloc(size);
	if(buf == NULL)
	{
		printf("malloc error!\n");
		return NULL;
	}

	memset(buf, 0, size);
	memcpy(buf, str, ptr-str);

	char *dst = buf+(ptr-str);
	while(*ptr)
	{
		if((*ptr == '\\') && (*(ptr+1) == '/'))
			ptr++;

		*dst++ = *ptr++;
	}

	return buf;
}

static int play_radio(void)
{
	struct json_object *root_object = NULL;

	// read radio list
	root_object = json_object_from_file(RADIO_LIST);
	if(root_object == NULL)
	{
		printf("read root_object error!\n");
		return -1;
	}

	// find index
	struct json_object *index_object = NULL;
	index_object = json_object_object_get(root_object, "index");
	if(index_object == NULL)
	{
		printf("read index_object error!\n");
		json_object_put(root_object);
		return -1;
	}
	const char *str_index = NULL;
	str_index = json_object_to_json_string(index_object);

	// parse index
	char index[16];
	memset(index, 0, sizeof(index));
	memcpy(index, str_index+1, strlen(str_index)-2);

	// find target
	struct json_object *target_object = NULL;
	target_object = json_object_object_get(root_object, index);
	if(target_object == NULL)
	{
		printf("read target_object error!\n");
		json_object_put(root_object);
		return -1;
	}

	// find url
	struct json_object *url_object = NULL;
	url_object = json_object_object_get(target_object, "url");
	if(url_object == NULL)
	{
		printf("read url_object error!\n");
		json_object_put(root_object);
		return -1;
	}
	const char *str_url = NULL;
	str_url = json_object_to_json_string(url_object);

	// parse url
	char buf[1024];
	int len;
	char *new_url = json_to_str(str_url);
	if(new_url)
	{
		len = snprintf(buf, sizeof(buf), 
			"echo \"loadfile %s", new_url+1);
		free(new_url);
	}
	else
		len = snprintf(buf, sizeof(buf), 
			"echo \"loadfile %s", str_url+1);

	snprintf(buf+len-1, sizeof(buf)-len+1, 
			"\" > /tmp/mplayer-infifo");

	// play
	printf("cmd=%s\n", buf);
	system(buf);

	json_object_put(root_object);
	return 0;
}

int main(int argc, char *argv[])
{
	return play_radio();
}
