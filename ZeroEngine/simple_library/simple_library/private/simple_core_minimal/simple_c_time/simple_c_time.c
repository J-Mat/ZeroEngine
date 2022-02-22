// Copyright (C) RenZhai.2022.All Rights Reserved.
#include "../../../public/simple_core_minimal/simple_c_time/simple_c_time.h"
#include "../../../public/simple_core_minimal/simple_c_time/simple_c_locale_list.h"

char * get_locale(char *in_buffer,
	const char *in_locale,
	const char *in_encoding)
{
	memset(in_buffer, 0, strlen(in_buffer));
	strcat(in_buffer, in_locale);
	strcat(in_buffer, ".");
	strcat(in_buffer, in_encoding);

	return in_buffer;
}

struct tm *get_local_time_tm()
{
	time_t ntime;
	time(&ntime);

	return localtime(&ntime);
}

time_t get_time_t()
{
	time_t ntime;
	time(&ntime);

	return ntime;
}

void get_local_time_string(char *t_buffer)
{
	struct tm * t = get_local_time_tm();
	char * p = asctime(t);

	strcpy(t_buffer, p);
}

struct tm * get_g_m_time_tm()
{
	time_t t_time = time(NULL);

	return gmtime(&t_time);
}

struct tm * time_t_to_tm(etime_type type, time_t *const t_a)
{
	switch (type)
	{
	case LOCAL_TIME:
		return localtime(t_a);
		
	case GM:
		return gmtime(t_a);
	}

	return NULL;
}

time_t tm_to_time_t(struct tm * const t_b)
{
	return mktime(t_b);
}

char * tm_to_string(const struct tm *t)
{
	return asctime(t);
}

char * time_t_to_string(etime_type type, const time_t *t)
{
	struct tm *p = time_t_to_tm(type, t);

	return tm_to_string(p);
}

clock_t get_clock()
{
	return clock();
}

double get_clock_diff(clock_t start, clock_t end)
{
	return (double)(end - start) / CLOCKS_PER_SEC;
}
