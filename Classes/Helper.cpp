#include "Helper.h"
#include <stdio.h>
#include <string>

std::string Helper::format2(int x) {
	std::string res;
	if (x < 10) {
		res = "0" + std::to_string(x);
	}
	else {
		res = std::to_string(x);
	}
	return res;
}
