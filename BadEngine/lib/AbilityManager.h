#ifndef AbilityManager_H
#define AbilityManager_H

class AbilityManager {
public:
	TimeManager* tm;

	struct AbalityCoolDown
	{
		std::string name;
		std::float coolDownTime;
		std::float timeToRefresh;
	}; 

	struct AbilityCount
	{
		std::string name;
		std::int count;
	};

	std::vector<AbalityCoolDown> abilityCollDown;
	std::vector<AbilityCount> AbilityCount;

	AbilityManager(TimeManeger* tm) {
		this->tm = tm;
	};

	void AddAbility(std::name, int count) {
		AbilityCount Ac;
		Ac.name = name;
		Ac.count = count;
		abilityCounts.push_back(ac);
	}

	void AddAbility(std::name,float coolDownTime) {
		AbalityCoolDown Acd;
		Acd.name = name;
		Acd.coolDownTime = coolDownTime;
		Acd.timeToRefresh = 0.f;
		abilityCoolDowns.push_back(acd);
	}

    void AddCount(const std::string& name, int count) {
        for (auto& ability : abilityCounts) {
            if (ability.name == name) {
                ability.count += count;
                return;
            }
        }
    }

    void UseCoolDownAbility(const std::string& name, float time) {
        for (auto& ability : abilityCoolDowns) {
            if (ability.name == name) {
                if (ability.timeToRefresh <= 0) {
                    ability.timeToRefresh = ability.coolDownTime;
                }
                return;
            }
        }
    }

    int GetCount(const std::string& name) {
        for (const auto& ability : abilityCounts) {
            if (ability.name == name) {
                return ability.count;
            }
        }
        return 0; 
    }

    void UpdateTime() {
        for (auto& ability : abilityCoolDowns) {
            if (ability.timeToRefresh > 0) {
                ability.timeToRefresh -= tm->getFramePerSeconds();
            }
        }
    }
	~AbilityManager();
	
};

#endif