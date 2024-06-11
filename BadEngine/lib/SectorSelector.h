#ifndef SECTORSELECTOR_H
#define SECTORSELECTOR_H

class SectorSelector {
public:
	SectorSelector(int* sectorCount) {
		this->sectorCount = sectorCount;
	}
	~SectorSelector() {}

	int* sectorCount;
	int selectedSector = 0;
	int selectedSector2 = 0;

	void selectSector(int which) {
		if (*sectorCount == 0) {
			selectedSector = 0;
			selectedSector2 = 0;
		}
		else {
			int sector = rand() % (*sectorCount * *sectorCount) + 1;
			if (*sectorCount == 1) {
				selectedSector = sector;
				selectedSector2 = sector;
				return;
			}
			if (which == 1) {
				selectedSector = sector;
			}
			else if (which == 2) {
				selectedSector2 = sector;
			}
			else {
				return;
			}
		}
	}
};
#endif
