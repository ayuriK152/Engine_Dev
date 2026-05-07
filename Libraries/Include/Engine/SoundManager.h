#pragma once

class BULB_API SoundManager
{
	friend class BulbApplication;

private:
	SoundManager() = default;
	~SoundManager();

public:
	SoundManager(const SoundManager& rhs) = delete;
	SoundManager& operator=(const SoundManager& rhs) = delete;

	static SoundManager* GetInstance();
	static Bulb::ProcessResult Delete();

	void Init();

	void StopAllSounds();

	// 사운드 로드 기능
	void LoadSound(const string& path, bool loop);
	// 사운드 재생 기능
	void PlaySound(const string& name, const string& group = "");

	void AddGroup(const string& name, const string& parentGroup = "");

	void SetMasterVolume(float value);

	void SetVolume(float value, const string& group);

private:
	static SoundManager* s_instance;

	FMOD::System* _system = nullptr;
	// <Path, Sound>
	unordered_map<string, FMOD::Sound*> _sounds;

	FMOD::ChannelGroup* _masterGroup = nullptr;

	unordered_map<string, FMOD::ChannelGroup*> _channelGroups;
};
