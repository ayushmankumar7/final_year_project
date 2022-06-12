#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "ops.h"
#include "utils.h"
#include "model.h"

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#ifdef __linux__
int main(int,char **);
void run(char *,char *);

#elif _WIN32 

__declspec(dllexport)int main(int,char **);
__declspec(dllexport)void run(char *,char *);

#endif

#endif