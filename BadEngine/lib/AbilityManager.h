#ifndef AbilityManager_H
#define AbilityManager_H

class AbilityManager {
public:
	TimeManager* tm;

	struct AbalityCoolDown
	{
		std::string name;
		float coolDownTime;
		float timeToRefresh;
	}; 

	//struct AbilityCount
	//{
	//	std::string name;
	//	int count;
	//};

    int bananaCount = 1;
    int limitBanana = 4;
	std::vector<AbalityCoolDown> abilityCoolDown;
	//std::vector<AbilityCount> AbilityCount;

	AbilityManager(TimeManager* tm) {
		this->tm = tm;
	};

    void AddBanana() {
        if ((bananaCount < limitBanana)) {
            bananaCount++;
        }
    }

    void UseBanana() {
        if (bananaCount > 0) {
            bananaCount--;
        }
    }
    bool CanUseBanana() {
        if (bananaCount > 0) {
            return true;
        }
        return false;
    }

    bool TryGetBanana() {
        if (bananaCount > 0) {
            bananaCount--;
            return true;
        }
        return false;
    }

	//void AddAbility(std::string name, int count) {
	//	AbilityCount Ac;
	//	Ac.name = name;
	//	Ac.count = count;
	//	abilityCounts.push_back(ac);
	//}

	void AddAbility(std::string name,float coolDownTime) {
		AbalityCoolDown Acd;
		Acd.name = name;
		Acd.coolDownTime = coolDownTime;
		Acd.timeToRefresh = 0.f;
        abilityCoolDown.push_back(Acd);
	}

    //void AddCount(const std::string& name, int count) {
    //    for (auto& ability : abilityCounts) {
    //        if (ability.name == name) {
    //            ability.count += count;
    //            return;
    //        }
    //    }
    //}

    void UseCoolDownAbility(const std::string& name, float time) {
        for (auto& ability : abilityCoolDown) {
            if (ability.name == name) {
                if (ability.timeToRefresh <= 0) {
                    ability.timeToRefresh = ability.coolDownTime;
                }
                return;
            }
        }
    }

    bool CheckUseCoolDown(std::string name) {
        for (auto& ability : abilityCoolDown) {
            if (ability.name == name) {
                if (ability.timeToRefresh <= 0) {
                    ability.timeToRefresh = ability.coolDownTime;
                    return true;
                }
                return false;
            }
        }
        return false;
    }

    bool CheckUseCoolDown(std::string name, bool isUse) {
        for (auto& ability : abilityCoolDown) {
            if (ability.name == name) {
                if (ability.timeToRefresh <= 0 || isUse) {
                    ability.timeToRefresh = ability.coolDownTime;
                    return true;
                }
                return false;
            }
        }
        return false;
    }

    //int GetCount(const std::string& name) {
    //    for (const auto& ability : abilityCounts) {
    //        if (ability.name == name) {
    //            return ability.count;
    //        }
    //    }
    //    return 0; 
    //}

    int getTimeToRefresh(std::string name) {
        for (auto& ability : abilityCoolDown) {
            if (ability.name == name) {
                return static_cast<int>(ability.timeToRefresh);
            }
        }
        return -1;
    }

    void ClearAllAbilities() {
        abilityCoolDown.clear();
    }

    void UpdateTime(float deltaTime) {
        deltaTime *= 2;
        for (auto& ability : abilityCoolDown) {
            if (ability.timeToRefresh > 0) {
                ability.timeToRefresh -= deltaTime;
            }
            else if (ability.timeToRefresh < 0) {
                ability.timeToRefresh = 0.f;
            }
        }
    }
	~AbilityManager();
	
};

#endif