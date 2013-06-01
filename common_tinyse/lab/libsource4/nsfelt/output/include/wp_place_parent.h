/*
 * �������¼�������ϵģ�鶨��
 */

#ifndef PLACE_PARENT_H_
#define PLACE_PARENT_H_

#include <ul_dict.h>

#define PLACE_MAP_FILE "PlaceMap"

//������: ������5857, ���������������, ��Ҫ�������SIZE
const unsigned int PARENTS_BUFFER_SIZE = 7000;	//�������Ĵ�С


Sdict_search* loadPlaceMap(const char* mapPath, const char* mapFile, char* parentZone);

void freePlaceMap(Sdict_search* pDictSearch);

bool isPlaceParent(const char* firstPlace, char* secondPlace, 
		Sdict_search* pDictSearch, const char* parentZone);

#endif //PLACE_PARENT_H_
