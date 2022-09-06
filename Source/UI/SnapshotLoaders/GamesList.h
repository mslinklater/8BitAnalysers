#pragma once

#include <string>
#include <vector>

class FSpectrumEmu;

enum class ESnapshotType
{
	Z80,
	SNA,
	RZX
};

struct FGameSnapshot
{
	ESnapshotType	Type;
	std::string		DisplayName;
	std::string		FileName;
};

class FGamesList
{
public:
	void	Init(FSpectrumEmu* pEmu) { pSpectrumEmu = pEmu; }
	bool	EnumerateGames();
	bool	LoadGame(int index);
	bool	LoadGame(const char* pFileName);

	int		GetNoGames() const { return (int)GamesList.size(); }
	const FGameSnapshot& GetGame(int index) const { return GamesList[index]; }
	const std::string& GetGameName(int index) const { return GamesList[index].DisplayName; }
	//const std::vector<FGameSnapshot>& GetList() const { return GamesList; }

private:
	FSpectrumEmu* pSpectrumEmu = nullptr;
	std::vector< FGameSnapshot>	GamesList;
};