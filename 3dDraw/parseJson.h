#pragma once
#include "json/json.h"

class parseJson {
public:
	void parsing_json();
	double wpointX[4] = {};
	double wpointY[4] = {};
	double pointX[19][4] = { {},{},{},{} };		// dsc ������ŭ �迭 ����
	double pointY[19][4] = { {},{},{},{} };
};