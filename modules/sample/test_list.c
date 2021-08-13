#include "list.h"
#include <stdio.h>
#include <stdlib.h>

struct lib_person {
	struct list_head list;
	int age;
};

int main(void)
{

	struct lib_person *tmp;
	struct list_head *tmp_list;
	LIST_HEAD(p);
	int people_num = 10;
	for (int i = 0; i < people_num; i++) {
		tmp = (struct lib_person *)malloc(sizeof(struct lib_person));
		tmp->age = i;
		add_list(&tmp->list, &p);
	}

	list_for_each(tmp_list, &p) {
		printf("age:%d\n", ((struct lib_person *)tmp_list)->age);
	}
}
