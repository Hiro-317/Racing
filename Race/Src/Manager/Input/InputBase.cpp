#include "InputBase.h"


InputBase::InputBase(void)
{
}

void InputBase::Update(std::pair<const int, Info>& p)
{
	p.second.keyTrgDown = p.second.keyNew && !p.second.keyOld;
	p.second.keyTrgUp = !p.second.keyNew && p.second.keyOld;

	if (!anyoneTrgDown) {
		anyoneTrgDown = p.second.keyTrgDown;
		anyoneTrg = p.second.keyTrgDown;
		anyone = p.second.keyTrgDown;
	}
	if (!anyoneTrg) {
		anyoneTrg = p.second.keyNew;
		anyone = p.second.keyNew;
	}
}

void InputBase::Add(int key)
{
	Info info = Info();
	info.key = key;
	info.keyOld = false;
	info.keyNew = false;
	info.keyTrgDown = false;
	info.keyTrgUp = false;
	infos.emplace(key, info);
}

bool InputBase::Added(int key)
{
	return infos.find(key) != infos.end();
}
