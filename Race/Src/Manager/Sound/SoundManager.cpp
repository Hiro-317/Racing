#include <DxLib.h>

#include "SoundManager.h"

#include <charconv>
#include <cmath>
#include <filesystem>
#include<fstream>
#include<iostream>

#include "../../Common/Utility.h"
#include "../../Common/Vector3.h"

namespace {
	int LoadSoundFile(const std::string& path, int bufferCount = 3)
	{
#ifdef UNICODE
		return LoadSoundMem(std::filesystem::path(path).c_str(), bufferCount);
#else
		return LoadSoundMem(path.c_str(), bufferCount);
#endif
	}
}

SoundManager* SoundManager::ins = nullptr;

SoundManager::SoundManager() :
	masterVolume(255),
	bgmVolume(1.0f),
	seVolume(1.0f)
{
}

void SoundManager::Init(void)
{
#pragma region サウンドテーブルを読み込む
	// CSVファイルを開く
	std::ifstream ifs = std::ifstream("Data/Sound/SoundTable.csv");

	// 例外処理：ファイルが開けなかった場合
	if (!ifs) { std::cerr << "サウンドデータの読み込みに失敗しました。" << '\n'; }

	std::string line;
	size_t lineNumber = 0;
	while (std::getline(ifs, line)) {
		++lineNumber;
		if (lineNumber == 1 && line.starts_with("\xEF\xBB\xBF")) { line.erase(0, 3); }
		if (!line.empty() && line.back() == '\r') { line.pop_back(); }
		if (line.empty()) { continue; }

		const auto fields = Split(line, ',');
		if (!fields.empty() && fields[TABLE_ID] == "ID") { continue; }

		SoundTable data;
		int maxDupli = 0;
		if (fields.size() == TABLE_COLUMN_COUNT &&
			!fields[TABLE_ID].empty() && !fields[TABLE_PATH].empty() && !fields[TABLE_SCENE].empty() &&
			(fields[TABLE_TYPE] == "SE" || fields[TABLE_TYPE] == "BGM") &&
			(fields[TABLE_LOOP] == "TRUE" || fields[TABLE_LOOP] == "FALSE" ||
			 fields[TABLE_LOOP] == "True" || fields[TABLE_LOOP] == "False")) {
			const auto& volume = fields[TABLE_VOLUME];
			const auto& copies = fields[TABLE_DUPLI];
			const auto volumeResult = std::from_chars(volume.data(), volume.data() + volume.size(), data.volume);
			const auto copiesResult = std::from_chars(copies.data(), copies.data() + copies.size(), maxDupli);
			if (volumeResult.ec == std::errc{} && volumeResult.ptr == volume.data() + volume.size() &&
				std::isfinite(data.volume) && data.volume >= 0.0f && data.volume <= 1.0f &&
				copiesResult.ec == std::errc{} && copiesResult.ptr == copies.data() + copies.size() &&
				maxDupli >= 1 && maxDupli <= 100) {
				data.path = fields[TABLE_PATH];
				data.scene = fields[TABLE_SCENE];
				data.type = fields[TABLE_TYPE];
				data.loop = fields[TABLE_LOOP] == "TRUE" || fields[TABLE_LOOP] == "True";
				data.maxDupli = static_cast<unsigned char>(maxDupli);
				if (SOUND_TABLE.emplace(fields[TABLE_ID], data).second) { continue; }
			}
		}
		std::cerr << "サウンドテーブルの " << lineNumber << " 行目を読み込めませんでした。" << '\n';
	}
#pragma endregion

#pragma region 読み込みシーンAllのものは最初に読み込んでおく

	for (std::pair<const std::string, SoundTable>& data : SOUND_TABLE) {
		// 「All」以外はスキップ
		if (data.second.scene != "All") { continue; }

		// 音声を追加する
		SoundInfoAdd(data);
	}

#pragma endregion
}

void SoundManager::Release(void)
{
	// 現状抱えているデータを全て破棄する
	for (std::pair<const std::string, std::vector<SoundData>>& sound : sounds) {
		for (SoundData& s : sound.second) { DeleteSoundMem(s.id); }
		sound.second.clear();
	}
	sounds.clear();
	soundInfos.clear();
	for (const auto& [handle, sound] : spatialSounds) { DeleteSoundMem(handle); }
	spatialSounds.clear();
	SOUND_TABLE.clear();
}

void SoundManager::Update(void)
{
	for (auto& info : soundInfos) {
		if (info.second.playInterval > 0) { info.second.playInterval--; }
		else if (info.second.playInterval < 0) { info.second.playInterval = 0; }
	}
}

void SoundManager::ChangeScene(const std::string& scene)
{
#pragma region All以外を破棄する
	// 削除した配列要素数の添え字を記憶しておくための一時変数
	std::vector<std::string> deleteSubscrs = {};
	deleteSubscrs.reserve(sounds.size());

	// まず現状抱えているサウンドデータのなかの「All」以外のもののリソースを解放する
	for (std::pair<const std::string, std::vector<SoundData>>& sound : sounds) {
		if (SOUND_TABLE.at(sound.first).scene == "All") { continue; }

		for (SoundData& info : sound.second) { DeleteSoundMem(info.id); info.id = -1; }

		sound.second.clear();

		// 何を消したか記憶する
		deleteSubscrs.emplace_back(sound.first);
	}
	// 消したものを完全に配列から削除する
	for (std::string& subscr : deleteSubscrs) {
		soundInfos.erase(subscr);
		sounds.erase(subscr);
	}

	// 一時変数をクリア
	deleteSubscrs.clear();
#pragma endregion

	AddScene(scene);
}

void SoundManager::AddScene(const std::string& scene)
{

#pragma region 指定されたシーンに対応するサウンドを読み込む
	// テーブルを探索する
	for (std::pair<const std::string, SoundTable>& data : SOUND_TABLE) {
		// 指定のもの以外はスキップ
		if (data.second.scene != scene || data.second.scene == "Engine") { continue; }

		// 読み込んでデータ追加
		SoundInfoAdd(data);
	}
#pragma endregion
}

void SoundManager::Play(const std::string& id)
{
	// テーブルにIDがなかったらエラー通知して終了
	if (!SOUND_TABLE.contains(id)) {
		std::cerr << "指定のIDが見つからない為、音声を再生できませんでした" << '\n';
		return;
	}
	// サウンドデータがあるか確認する
	if (!soundInfos.contains(id) || !sounds.contains(id)) {
		std::cerr << "指定のIDの音声が正常に読み込まれていなかった為、音声を再生できませんでした" << '\n';
		return;
	}

	// インターバル
	if (soundInfos.at(id).playInterval > 0) { return; }

	// 配列を探索する
	for (SoundData& info : sounds.at(id)) {
		// 再生中だったら次のを確認しに行く
		if (info.play()) { continue; }

		// 音量設定
		ChangeVolumeSoundMem(VolumeValue(id), info.id);

		// ループ再生するかどうか
		auto loop = (SOUND_TABLE.at(id).loop) ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK;

		// 再生
		PlaySoundMem(info.id, loop, true);

		// 再生したのでインターバルをセット
		soundInfos.at(id).playInterval = PLAY_INTERVAL_TIME;

		break;
	}
}

void SoundManager::Stop(const std::string& id)
{
	// テーブルにIDがなかったらエラー通知して終了
	if (!SOUND_TABLE.contains(id)) {
		std::cerr << "指定のIDが見つからない為、音声を停止できませんでした" << '\n';
		return;
	}
	// サウンドデータがあるか確認する
	if (!soundInfos.contains(id) || !sounds.contains(id)) {
		std::cerr << "指定のIDの音声が正常に読み込まれていなかった為、音声を停止できませんでした" << '\n';
		return;
	}

	// 配列を探索する
	for (SoundData& info : sounds.at(id)) {
		// 再生中じゃなかったら次のを確認しに行く
		if (!info.play()) { continue; }

		// 停止
		StopSoundMem(info.id);

		info.paused = false;
	}
}

void SoundManager::AllStop(void)
{
	for (const auto& [handle, sound] : spatialSounds) { StopSpatial(handle); }

	// 配列を探索する
	for (std::pair<const std::string, std::vector<SoundData>>& sound : sounds) {
		for (SoundData& info : sound.second) {
			// 再生中じゃなかったら次のを確認しに行く
			if (!info.play()) { continue; }

			// 停止
			StopSoundMem(info.id);

			info.paused = false;
		}
	}
}

void SoundManager::Pause(void)
{
	for (auto& [handle, sound] : spatialSounds) {
		if (CheckSoundMem(handle) != 1) { continue; }
		StopSoundMem(handle);
		sound.paused = true;
	}

	// 配列を探索する
	for (std::pair<const std::string, std::vector<SoundData>>& sound : sounds) {
		for (SoundData& info : sound.second) {
			// 再生中じゃなかったら次のを確認しに行く
			if (!info.play()) { continue; }

			// 停止
			StopSoundMem(info.id);

			info.paused = true;
		}
	}
}

void SoundManager::PausePlay(void)
{
	for (auto& [handle, sound] : spatialSounds) {
		if (!sound.paused) { continue; }
		const int playType = sound.table->loop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK;
		PlaySoundMem(handle, playType, false);
		sound.paused = false;
	}

	// 配列を探索する
	for (std::pair<const std::string, std::vector<SoundData>>& sound : sounds) {
		for (SoundData& info : sound.second) {
			if (!info.paused) { continue; }

			// ループ再生するかどうか
			auto loop = (SOUND_TABLE.at(sound.first).loop) ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK;

			// 途中から再生
			PlaySoundMem(info.id, loop, false);
		}
	}
}

int SoundManager::LoadSpatial(const std::string& id)
{
	const auto entry = SOUND_TABLE.find(id);
	if (entry == SOUND_TABLE.end()) {
		std::cerr << "３Ｄ音源のIDが見つかりませんでした。 " << id << '\n';
		return -1;
	}

	const SoundTable* table = &entry->second;
	const auto copies = std::count_if(spatialSounds.begin(), spatialSounds.end(), [table](const auto& sound) {
		return sound.second.table == table;
	});
	if (copies >= table->maxDupli) { return -1; }

	SetCreate3DSoundFlag(true);
	const int handle = LoadSoundFile(table->path, 1);
	SetCreate3DSoundFlag(false);
	if (handle == -1) {
		std::cerr << "３Ｄ音源を読み込めませんでした。 " << table->path << '\n';
		return -1;
	}

	spatialSounds.emplace(handle, SpatialSound{ table });
	return handle;
}

bool SoundManager::PlaySpatial(int handle, const Vector3& position, float radius, float gain)
{
	const auto sound = spatialSounds.find(handle);
	if (sound == spatialSounds.end()) { return false; }

	Set3DRadiusSoundMem(radius, handle);
	UpdateSpatial(handle, position, gain);
	const int playType = sound->second.table->loop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK;
	sound->second.paused = false;
	return PlaySoundMem(handle, playType, true) == 0;
}

void SoundManager::UpdateSpatial(int handle, const Vector3& position, float gain, int frequency)
{
	const auto sound = spatialSounds.find(handle);
	if (sound == spatialSounds.end()) { return; }

	// 位置の更新後に回転数に応じた再生周波数を適用する
	Set3DPositionSoundMem(position.ToVECTOR(), handle);
	if (frequency > 0) { SetFrequencySoundMem(frequency, handle); }

	const auto& table = *sound->second.table;
	const float categoryGain = table.type == "BGM" ? bgmVolume : seVolume;
	const float amplitude = MasterVolumeRatio() * categoryGain * table.volume * gain;
	int volume = 0;
	if (amplitude > 0.0f) {
		volume = std::clamp(10000 + static_cast<int>(std::lround(2000.0f * std::log10(amplitude))), 0, 10000);
	}
	SetVolumeSoundMem(volume, handle);
}

bool SoundManager::IsSpatialPlaying(int handle) const
{
	const auto sound = spatialSounds.find(handle);
	return sound != spatialSounds.end() && (sound->second.paused || CheckSoundMem(handle) == 1);
}

void SoundManager::StopSpatial(int handle)
{
	const auto sound = spatialSounds.find(handle);
	if (sound == spatialSounds.end()) { return; }

	StopSoundMem(handle);
	sound->second.paused = false;
}

void SoundManager::ReleaseSpatial(int handle)
{
	if (spatialSounds.erase(handle) != 0) { DeleteSoundMem(handle); }
}

void SoundManager::SetSpatialListener(const Vector3& position, const Vector3& frontPosition, const Vector3& up)
{
	Set3DSoundListenerPosAndFrontPosAndUpVec(position.ToVECTOR(), frontPosition.ToVECTOR(), up.ToVECTOR());
}

void SoundManager::SoundInfoAdd(const std::pair<const std::string, SoundTable>& data)
{
	// その場所にすでに要素数があれば何もしない
	if (soundInfos.contains(data.first) || sounds.contains(data.first)) { return; }

	// 明示的に配列を追加
	soundInfos[data.first] = {};
	sounds[data.first] = {};

	// 最大同時再生数の数だけ音声を読み込む と同時に初期化も行う
	for (unsigned char i = 0; i < data.second.maxDupli; i++) {

		// 一旦一時変数にデータを取りまとめる
		SoundData work = {};
		work.id = LoadSoundFile(data.second.path);
		if (work.id == -1) { continue; }
		work.paused = false;

		// 一時変数にまとめて生成した情報を配列に追加する
		sounds.at(data.first).emplace_back(work);
	}
}

int SoundManager::VolumeValue(std::string id)
{
	int ret = masterVolume;

	// BGM/SE どちらなのかテーブルから取ってくる
	std::string type = SOUND_TABLE.at(id).type;
	
	// タイプ別の音量の割合計算
	float volumeRatio = 1.0f;
	if (type == "BGM") { volumeRatio = bgmVolume; }
	else if (type == "SE") { volumeRatio = seVolume; }

	ret = (int)((float)ret * volumeRatio);

	// 個別の音量の割合計算
	volumeRatio = SOUND_TABLE.at(id).volume;

	ret = (int)((float)ret * volumeRatio);

	return ret;
}

bool SoundManager::SoundData::play(void) { return (CheckSoundMem(id) == 1); }