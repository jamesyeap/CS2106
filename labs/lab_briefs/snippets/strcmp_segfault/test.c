#include <stdio.h>
#include <string.h>

int main()
{
	// char *string_arr[3] = { "hello", "there", "people" };
	char *string_arr[3] = { "hello", "there", NULL };

	int is_hello = strcmp(string_arr[0], "hello");
	printf("is first element hello: %d\n", is_hello);

	int is_world = strcmp(string_arr[1], "world");
	printf("is first element world: %d\n", is_world);

	if (string_arr[2] != NULL && strcmp(string_arr[2], "people")) {
		int is_people = strcmp(string_arr[2], "people");
		printf("is third element people: %d\n", is_people);
	} else {
		if (string_arr[2] == NULL) {
			printf("nice one almost segfault wow\n");
		} else {
			int is_people = strcmp(string_arr[2], "people");
			printf("is third element people: %d\n", is_people);
		}
		
	}

}