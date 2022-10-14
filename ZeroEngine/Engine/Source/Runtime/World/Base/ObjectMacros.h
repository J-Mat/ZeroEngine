#pragma once


#define GENERATED_BODY_INNER(A,B,C,D) A##B##C##D

#define UCLASS(...)
#define UPROPERTY(...)
#define UFUNCTION(...)
#define USTRUCT(...)

#define GENERATED_BODY(...) //\
//GENERATED_BODY_INNER(REFLECTION_FILE_NAME, _, TAG_LINE, _BODY)
