// Copyright (C) RenZhai.2022.All Rights Reserved.
#include "../../../public/simple_core_minimal/simple_c_guid/simple_guid.h"
#include "../../../public/simple_core_minimal/simple_c_core/simple_c_string_algorithm/string_algorithm.h"

int hex_digit(char c)
{
	int Result = 0;

	if (c >= '0' && c <= '9')
	{
		Result = c - '0';
	}
	else if (c >= 'a' && c <= 'f')
	{
		Result = c + 10 - 'a';
	}
	else if (c >= 'A' && c <= 'F')
	{
		Result = c + 10 - 'A';
	}
	else
	{
		Result = 0;
	}

	return Result;
}

unsigned int hex_number(const char *hex_str)
{
	unsigned int Ret = 0;

	while (*hex_str)
	{
		Ret *= 16;
		Ret += hex_digit(*hex_str++);
	}

	return Ret;
}
void guid_parse(const char* buf, simple_c_guid* c_guid)
{
	if (strlen(buf) == 32)
	{
		string_to_guid(buf, c_guid);
	}
}

bool is_guid_valid_str(const char* c_guid)
{
	simple_c_guid n_guid;

	normalization_guid(&n_guid);
	string_to_guid(c_guid, &n_guid);

	return is_guid_valid(&n_guid);
}

bool is_guid_valid(simple_c_guid* c_guid)
{
	simple_c_guid z_guid;
	normalization_guid(&z_guid);

	return !guid_equal(c_guid, &z_guid);
}

void normalization_guid(simple_c_guid* c_guid)
{
	c_guid->a = 0;
	c_guid->b = 0;
	c_guid->c = 0;
	c_guid->d = 0;
}

void create_guid_str(char* c_guid)
{
	simple_c_guid n_guid;
	normalization_guid(&n_guid);

	create_guid(&n_guid);

	guid_to_string(c_guid, &n_guid);
}

void create_guid(simple_c_guid* c_guid)
{
	normalization_guid(c_guid);
	assert(CoCreateGuid((GUID*)c_guid) == S_OK);
}

void guid_to_string(char* buf,const simple_c_guid* c_guid)
{
	get_printf_s(buf, "%08X%08X%08X%08X",c_guid->a, c_guid->b, c_guid->c, c_guid->d);
}

void string_to_guid(const char* buf, simple_c_guid* c_guid)
{
	if (strlen(buf) + 1 >= 32)
	{
		char byte_buf[MAX_PATH] = { 0 };
		c_guid->a = hex_number(string_mid(buf, byte_buf, 0, 8));
		c_guid->b = hex_number(string_mid(buf, byte_buf, 8, 8));
		c_guid->c = hex_number(string_mid(buf, byte_buf, 16, 8));
		c_guid->d = hex_number(string_mid(buf, byte_buf, 24, 8));
	}
}

bool guid_equal(const simple_c_guid * c_guid_a, const simple_c_guid * c_guid_b)
{
	return	c_guid_a->a == c_guid_b->a	&&
			c_guid_a->b == c_guid_b->b  &&
			c_guid_a->c == c_guid_b->c  &&
			c_guid_a->d == c_guid_b->d;
}

bool guid_equal_str(const char *guid_string, const simple_c_guid * c_guid)
{
	simple_c_guid id;
	string_to_guid(guid_string, &id);

	return guid_equal(&id, c_guid);
}
