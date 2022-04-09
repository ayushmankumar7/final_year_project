#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "ops.h"
#include "utils.h"
#include "model.h"

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

__declspec(dllexport)int main(int,char **);
__declspec(dllexport)void run(char *,char *);

#endif