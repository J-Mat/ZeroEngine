#pragma once


#define REFLECTION_BODY(A,B,C,D) A##B##C##D

#define UPROPERTY(...)
#define UFUNCTION(...)
#define USTRUCT(...)

#define REFLECTION_BODY(...) \
REFLECTION_BODY(REFLECTION_FILE_NAME, _, TAG_LINE, _BODY)
