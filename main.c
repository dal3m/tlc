/*
	TLC - tiny librus (synergia) client.
	Author: dalem
	License: ISC
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

#include "urls.h"
#include "auth.h"
#include "map.h"

void
lessons(CURL *curl)
{
	FILE *ltmp = fopen("/tmp/subjects.tlc", "w+");
	CURLcode res = fetch(curl, SUBJSURL, ltmp);

	if(res != CURLE_OK)
		fprintf(stderr, "lessons error: %s\n", curl_easy_strerror(res));
		
	fclose(ltmp);
	
	json_t *arr  = jsoninit("/tmp/subjects.tlc", "Subjects");

	size_t sz = (size_t)json_array_size(arr), i;
	
	for(i = 0; i < sz; i++)
	{
		json_t *cur = json_array_get(arr, i);
		
		json_t *id =  json_object_get(cur, "Id");
		json_t *nme = json_object_get(cur, "Name");

		add(json_integer_value(id), json_string_value(nme), &subjects);
	}
}

void
cats(CURL *curl)
{
	FILE *ctmp = fopen("/tmp/categories.tlc", "w+");
	CURLcode res = fetch(curl, CATEGURL, ctmp);

	if(res != CURLE_OK)
		fprintf(stderr, "categories error: %s\n", curl_easy_strerror(res));
		
	fclose(ctmp);
	
	json_t *arr  = jsoninit("/tmp/categories.tlc", "Categories");

	size_t sz = (size_t)json_array_size(arr), i;
	
	for(i = 0; i < sz; i++)
	{
		json_t *cur = json_array_get(arr, i);
		
		json_t *id =  json_object_get(cur, "Id");
		json_t *nme = json_object_get(cur, "Name");

		add(json_integer_value(id), json_string_value(nme), &categories);
	}
}

void
grades(CURL *curl)
{
	FILE *gtmp = fopen("/tmp/grades.tlc", "w+");

	CURLcode res = fetch(curl, GRADEURL, gtmp);
	
	if(res != CURLE_OK)
		fprintf(stderr, "grades error: %s\n", curl_easy_strerror(res));

	lessons(curl);
	
	fclose(gtmp);

	json_t *arr  = jsoninit("/tmp/grades.tlc", "Grades");
	size_t sz = (size_t)json_array_size(arr), i;
	
	for(i = 0; i < sz; i++)
	{
		json_t *cur = json_array_get(arr, i);
		
		json_t *grd = json_object_get(cur, "Grade");
		json_t *sem = json_object_get(cur, "Semester");
		json_t *dat = json_object_get(cur, "Date");
		
		json_t *sub = json_object_get(cur, "Subject");
			json_t *id =  json_object_get(sub, "Id");
		
		printf("%lld\t%s\t%s:\t%s\n", json_integer_value(sem), json_string_value(dat),
			look(json_integer_value(id), subjects), json_string_value(grd));
	}

	freemap(subjects);
}

const char* 
lesson2name(CURL *curl, const char *url)
{
	FILE *ltmp = fopen("/tmp/lessons.tlc", "w+");
        
	CURLcode res = fetch(curl, url, ltmp);
	fclose(ltmp);

	if(res != CURLE_OK) 
		fprintf(stderr, "lesson error: %s\n", curl_easy_strerror(res));
	
	json_t *lesson = jsoninit("/tmp/lessons.tlc", "Lesson");
	
	json_t *sub = json_object_get(lesson, "Subject");
		json_t *id = json_object_get(sub, "Id");

	return look(json_integer_value(id), subjects);
}

void
attendance(CURL *curl)
{
	FILE *atmp = fopen("/tmp/attendance.tlc", "w+");

	CURLcode res = fetch(curl, ATTENURL, atmp);
	
	if(res != CURLE_OK)
		fprintf(stderr, "attendance error: %s\n", curl_easy_strerror(res));

	lessons(curl);
	
	fclose(atmp);

	json_t *arr  = jsoninit("/tmp/attendance.tlc", "Attendances");
	size_t sz = (size_t)json_array_size(arr), i;
	
	for(i = 0; i < sz; i++)
	{
		json_t *cur = json_array_get(arr, i);
		
		json_t *sem = json_object_get(cur, "Semester");
		json_t *dat = json_object_get(cur, "Date");
		
		json_t *typ = json_object_get(cur, "Type");
			json_t *id =  json_object_get(typ, "Id");

		json_t *les = json_object_get(cur, "Lesson");
			json_t *uid = json_object_get(les, "Id");
		
		char buf[1024];
		snprintf(buf, sizeof(buf), "https://synergia.librus.pl/gateway/api/2.0/Lessons/%lld", 
		json_integer_value(uid));
		
		printf("%lld\t%s\t%s:\t", json_integer_value(sem), json_string_value(dat), lesson2name(curl,buf));

		switch(json_integer_value(id))
		{
			case 1:
				printf("nb\n");
				break;
			
			case 2:
				printf("sp\n");
				break;
			
			case 3:
				printf("u\n");
				break;
			
			case 4:
				printf("zw\n");
				break;

			case 100:
				printf("ob\n");
				break;

			default:
				printf("?\n");
				break;
		}
	}
}

void
notices(CURL *curl)
{
	FILE *ntmp = fopen("/tmp/notices.tlc", "w+");

	CURLcode res = fetch(curl, NOTICURL, ntmp);
	
	if(res != CURLE_OK)
		fprintf(stderr, "notices error: %s\n", curl_easy_strerror(res));
		
	fclose(ntmp);

	json_t *arr  = jsoninit("/tmp/notices.tlc", "SchoolNotices");
	size_t sz = (size_t)json_array_size(arr), i;
	
	for(i = 0; i < sz; i++)
	{
		json_t *cur = json_array_get(arr, i);
		
		json_t *dat = json_object_get(cur, "StartDate");
		json_t *top = json_object_get(cur, "Subject");
		json_t *con = json_object_get(cur, "Content");
		
		printf("%s\n%s\n%s\n", json_string_value(dat), json_string_value(top),
		json_string_value(con));
	}
}

void
warns(CURL *curl)
{
	FILE *wtmp = fopen("/tmp/warns.tlc", "w+");

	CURLcode res = fetch(curl, WARNSURL, wtmp);
	
	if(res != CURLE_OK)
		fprintf(stderr, "warnings error: %s\n", curl_easy_strerror(res));
		
	fclose(wtmp);

	json_t *arr  = jsoninit("/tmp/warns.tlc", "Notes");
	size_t sz = (size_t)json_array_size(arr), i;
	
	for(i = 0; i < sz; i++)
	{
		json_t *cur = json_array_get(arr, i);
		
		json_t *dat = json_object_get(cur, "Date");
		json_t *pos = json_object_get(cur, "Positive");
		json_t *con = json_object_get(cur, "Text");
		
		char stat[6];
		
		if(json_integer_value(pos) == 0)
			strcpy(stat, "false");

		else
			strcpy(stat, "true");

		printf("%s\n\npositive?: %s\n\n%s\n\n", json_string_value(dat), stat,
		json_string_value(con));
	}
}

void
tests(CURL *curl)
{
	FILE *ttmp = fopen("/tmp/tests.tlc", "w+");

	CURLcode res = fetch(curl, HOMEWURL, ttmp);
	
	if(res != CURLE_OK)
		fprintf(stderr, "warnings error: %s\n", curl_easy_strerror(res));

	lessons(curl);
	cats(curl);
	
	fclose(ttmp);

	json_t *arr  = jsoninit("/tmp/tests.tlc", "HomeWorks");
	size_t sz = (size_t)json_array_size(arr), i;
	
	for(i = 0; i < sz; i++)
	{
		json_t *cur = json_array_get(arr, i);
		
		json_t *con = json_object_get(cur, "Content");
		json_t *dat = json_object_get(cur, "Date");

		json_t *cat = json_object_get(cur, "Category");
			json_t *id = json_object_get(cat, "Id");

		json_t *sub = json_object_get(cur, "Subject");
			json_t *sid = json_object_get(sub, "Id");
		
		printf("%s\t%s\t%s\t%s\t\n", json_string_value(dat), look(json_integer_value(id), categories),
		look(json_integer_value(sid), subjects), json_string_value(con));
	}

	freemap(subjects);
	freemap(categories);
}

void calendar(CURL *curl, const char* date)
{
	FILE *ctmp = fopen("/tmp/calendar.tlc", "w+");
	CURLcode res;

	if(date[0] == '\0')
		res = fetch(curl, CALENURL, ctmp);

	else
	{
		char url[128];
		snprintf(url, 128, "%s?weekStart=%s", CALENURL, date);
		res = fetch(curl, url, ctmp);
	}
		

	if (res != CURLE_OK) 
		fprintf(stderr, "calendar error: %s\n", curl_easy_strerror(res));
		
	fclose(ctmp);

	json_t *root = jsoninit("/tmp/calendar.tlc", "Timetable");
	if(!root) return;

	const char *key;
	json_t *arr;

	json_object_foreach(root, key, arr)
	{
		if(!json_is_array(arr)) continue;

		size_t sz = json_array_size(arr), i;
		for(i = 0; i < sz; ++i)
		{
			json_t *cur = json_array_get(arr, i);
			if (!json_is_array(cur) || json_array_size(cur) == 0) continue;

			size_t lsz = json_array_size(cur), j;
			for(j = 0; j < lsz; ++j)
			{
				json_t *obj = json_array_get(cur, j);

					json_t *hf  = json_object_get(obj, "HourFrom");
					json_t *ht  = json_object_get(obj, "HourTo");
					json_t *can = json_object_get(obj, "IsCanceled");
					json_t *sbs = json_object_get(obj, "IsSubstitutionClass");

					json_t *sub = json_object_get(obj, "Subject");
						json_t *name = sub ? json_object_get(sub, "Name") : NULL;

				int c = json_is_true(can);
				int s = json_is_true(sbs);

				printf("%s\t%s-%s %s\t%s\t%s\n",
					key,
					json_string_value(hf),
					json_string_value(ht),
					s ? "Substituted" : " ",
					c ? "Canceled" : " ",
					json_string_value(name));
			}
		}
	}
}
void 
lucky(CURL *curl) 
{
    CURLcode res;

    FILE *ntmp = fopen("/tmp/lucky.tlc", "w+");

    curl_easy_setopt(curl, CURLOPT_URL, LUCKYURL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, ntmp);
    res = curl_easy_perform(curl);

    fclose(ntmp);

    json_error_t err;
    json_t *root = json_load_file("/tmp/lucky.tlc", 0, &err);

    json_t *num = json_object_get(root, "LuckyNumber");
    json_t *lck = json_object_get(num, "LuckyNumber");

    if(lck)
        printf("%lld\n", json_integer_value(lck));
	else
        fprintf(stderr, "LuckyNumber not found in JSON\n");

	if(res != CURLE_OK)
		fprintf(stderr, "grades error: %s\n", curl_easy_strerror(res));

    json_decref(root);
}

void
help(void)
{
		printf("Usage: tlc OPTIONS\n"
		"\t -g\t fetch grades in a format of SEMESTER DATE SUBJECT: GRADE\n"
		"\t -a\t fetch attendance in a format of SEMESTER DATE SUBJECT: STATUS\n"
		"\t -l\t fetch the lucky number\n"
		"\t -n\t fetch school annoucments\n"
		"\t -w\t fetch your warnings\n"
		"\t -t\t fetch your tests\n"
		"\t -c\t fetch calendar with an optional date optionz\n"
		"\t -h\t display this help message\n");

		exit(0);
}

int
main(int argc, char *argv[])
{
	CURL *curl;	
	curl = curl_easy_init();
	
	if(argc != 2 && strcmp(argv[1], "-c") != 0)
		help();

	if(curl)
	{
		init(curl);
		post(curl);
		login(curl);

		if(strcmp(argv[1], "-l") == 0)
			lucky(curl);

		else if(strcmp(argv[1], "-g") == 0)
			grades(curl);
		
		else if(strcmp(argv[1], "-a") == 0)
			attendance(curl);

		else if(strcmp(argv[1], "-n") == 0)
			notices(curl);

		else if(strcmp(argv[1], "-w") == 0)
			warns(curl);

		else if(strcmp(argv[1], "-t") == 0)
			tests(curl);

		else if (strcmp(argv[1], "-c") == 0) 
		{
			const char *date = (argc > 2) ? argv[2] : "";  // Check if date is supplied
			calendar(curl, date);
		}

		else
			help();

		curl_easy_cleanup(curl);
	}
	 
	curl_global_cleanup();
	return 0;
}
