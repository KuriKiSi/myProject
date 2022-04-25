#include "cscale.h"

const vector<array<QString, HALF_SCALE_CNT>> CScale::s_CHalfScale = {			 //半音阶 12平均律, 四种表示方式
	{"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"},
	{"C","Db","D","Eb","E","F","Gb","G","Ab","A","Bb","B"},
	{"1","#1","2","#2","3","4","#4","5","#5","6","#6","7"},
	{"1","b2","2","b3","3","4","b5","5","b6","6","b7","7"}
};

const vector<tonalInfo> CScale::s_TonalityInfo = {		//调式信息
	{"Natural Major", {2,2,1,2,2,2,1}},
	{"Natural Minor", {2,1,2,2,1,2,2}}
};

const vector<turningInfo> CScale::s_TurningInfo = {		//各调弦的空弦音
	{"Standard", {TONE_E, TONE_A, TONE_D, TONE_G, TONE_B, TONE_E}},
	{"Drop D",	 {TONE_D, TONE_A, TONE_D, TONE_G, TONE_B, TONE_E}},
	{"Low C",	 {TONE_C, TONE_G, TONE_D, TONE_G, TONE_A, TONE_D}}
};

const vector<chordInfo> CScale::s_ChordInfo = {			//和弦信息
	{"None",		{-1,-1,-1,-1}},
	{"Major",		{4,3,-1,-1}},	 //X Major
	{"Minor",		{3,4,-1,-1}},	 //X Minor
	{"Five",		{7,-1,-1,-1}},	 //X 5
	{"Sus2",		{2,5,-1,-1}},	 //X Sus2
	{"Sus4",		{5,2,-1,-1}},	 //X Sus4
	{"Aug",			{4,4,-1,-1}},	 //X Aug
	{"Dim",			{3,3,-1,-1}},	 //X Dim
	{"Major7",		{4,3,4,-1}},	 //X M7
	{"Minor7",		{3,4,3,-1}},	 //X m7
	{"D7",			{4,3,3,-1}},	 //X 7
	{"Half Dim7",	{3,3,3,-1}},	 //X m7-5
	{"D7Sus4",		{5,2,3,-1}},	 //X 7Sus4
	{"Dim7",		{3,3,2,-1}},	 //X Dim7
	{"Major6",		{4,3,2,-1}},	 //X Maj6
	{"Minor6",		{3,4,2,-1}},	 //X Min6
	{"Add9",		{2,2,3,-1}}		 //X Add9
};

const array<QString, INNER_TONE_CNT> CScale::s_RomaList = {"I", "II", "III", "IV", "V", "VI", "VII"};  //罗马数字
const array<char, INNER_TONE_CNT> CScale::s_CMajorScale = {'C', 'D', 'E', 'F', 'G', 'A', 'B'};		   //C大调音阶

CScale::CScale(int key, int tonality, int turning) :
	m_Turning(turning),
	m_Chord(CHORD_NONE),
	m_ChordLevel(1),
	m_Mark(MARK_SHARP)
{
	RefreshData(key, tonality);
}

void CScale::RefreshData(int key, int tonality)
{
	this->m_Name = QString("%1 %2 Scale").arg(s_CHalfScale[m_Mark][key]).arg(s_TonalityInfo[tonality].name);
	this->m_Key = key;
	this->m_Tonality = tonality;
    RefreshInnerToneList(key, tonality);
}

void CScale::RefreshInnerToneList(int key, int tonality)
{
    this->m_Member[0] = key;
	for (int i = 1; i < 8; i++) {
        this->m_Member[i] = (this->m_Member[i-1] + s_TonalityInfo[tonality].order[i-1]) % 12;
	}
}

bool CScale::IsBaseTone(int index)
{
	QString name = s_CHalfScale[m_Mark][index];
	if (name.indexOf("#") != -1 || name.indexOf("b") != -1){
		return false;
	}
	return true;
}

bool CScale::IsInnerTone(int tone)
{
   return (GetLevelByTone(tone) == -1) ? false : true;
}

bool CScale::IsChordTone(int tone)
{
	array<int, CHORD_TONE_CNT_MAX> tonelist = {0};
	int count = GetChordList(tonelist);
	if (count == 0) {
		return false;
	}
	for (int i = 0; i < count; i++) {
		if (tone == tonelist[i]) {
			return true;
		}
	}
	return false;
}

int CScale::GetChordList(array<int, CHORD_TONE_CNT_MAX> &tonelist)
{
	if (m_Chord == 0) {
		return 0;
	}
	int count = 1;
	int tone = GetToneByLevel(m_ChordLevel-1);
	tonelist[0] = (m_Key+abs(tone-m_Key)) % 12;
	for (int i = 1; i < CHORD_TONE_CNT_MAX; i++) {
		int iDistance = s_ChordInfo[m_Chord].order[i-1];
		if (iDistance < 0) {
			break;
		}
		tonelist[i] = (tonelist[i-1]+iDistance)%12;
		count++;
	}
	return count;
}

int CScale::GetLevelByTone(int tone)
{
	for (int i = 0; i < 8; i++) {
		if (tone == m_Member[i]) {
			return i;
		}
	}
	return -1;
}


int CScale::GetToneByName(QString name)
{
	for (unsigned int i = 0; i < s_CHalfScale.size(); i++) {
		for (int j = 0; j < s_CHalfScale[i].size(); j++) {
			if (name == s_CHalfScale[i][j]) {
				return j;
			}
		}
	}
	return -1;
}

vector<s_ToneInfo> CScale::AnalyStaff(const QString str)
{
	vector<s_ToneInfo> vTone;
	string stdString = str.toStdString();
	QString qstr;

	//1. "|" -> ",|,"
	regex reg1("\\|");
	stdString = std::regex_replace(stdString, reg1, ",|,");

	//delete space
	regex reg_space("\\s");
	stdString = std::regex_replace(stdString, reg_space, "");

	//2.split with ','
	vector<string> vsTone;
	regex reg2(",");
	sregex_token_iterator pos(stdString.begin(), stdString.end(), reg2, -1);
	decltype(pos) end;
	for (; pos != end; ++pos) {
		vsTone.push_back(pos->str());
	}

	//3.match each tone
	regex reg3("([1-7][#b]?|\\|)(?:\\(([+-]?\\d+)\\))?");
	for (unsigned int i = 0; i < vsTone.size(); i++) {
		smatch m;
		string s = vsTone[i];
		auto ret = regex_match(s, m, reg3);
		if (ret) {
			s_ToneInfo info;
			string toneStr = m.str(1);
			if (toneStr == "|") {
				info.tone = -1;
			}
			else {
				for (unsigned int i = 0; i < toneStr.size(); i++) {
					if (toneStr[i] >= '1' && toneStr[i] <= '7') {
						toneStr[i] = s_CMajorScale[toneStr[i] - '0' -1];
					}
				}
				//qDebug() << qstr.fromStdString(toneStr);
				info.tone = GetToneByName(qstr.fromStdString(toneStr));
			}
			info.group = (m.str(2) == "") ? 0 : atoi(m.str(2).c_str());
			vTone.push_back(info);
			//qDebug() << info.tone << info.group << qstr.fromStdString(m.str(1)) << qstr.fromStdString(m.str(2));
		}
	}
	return vTone;
}

vector<s_ToneInfo> CScale::Modulation(const vector<s_ToneInfo> before, int curKey, int destKey)
{
	vector<s_ToneInfo> vDestTone;
	int distance = destKey - curKey;

	for(auto it = before.begin(); it != before.end(); ++it) {
		int group = (*it).group;
		int tone = (*it).tone;
		s_ToneInfo destToneInfo;
		if (tone < 0) {
			destToneInfo.tone = tone;
			destToneInfo.group = 0;;
		}
		else {
			destToneInfo.tone = tone + distance;
			destToneInfo.group = group;
			if (destToneInfo.tone < 0) {
				destToneInfo.tone += 12;
				destToneInfo.group--;
			}
			else if (destToneInfo.tone >= 12) {
				destToneInfo.tone -= 12;
				destToneInfo.group++;
			}
		}
		vDestTone.push_back(destToneInfo);
	}
	return vDestTone;
}

vector<s_ToneInfo> CScale::TonalityConvert(const vector<s_ToneInfo> before, int curTonal, int destTonal)
{
	vector<s_ToneInfo> vDestTone;
	int relationlist[INNER_TONE_CNT] = {0};
	relationlist[0] = 0;

	for (int i = 0; i < INNER_TONE_CNT; i++) {
		relationlist[i+1] = s_TonalityInfo[destTonal].order[i] - s_TonalityInfo[curTonal].order[i] + relationlist[i];
	}

	for(auto it = before.begin(); it != before.end(); ++it) {
		int group = (*it).group;
		int tone = (*it).tone;
		s_ToneInfo destToneInfo;
		if (tone < 0) {
			destToneInfo.tone = tone;
			destToneInfo.group = 0;;
		}
		else if (IsInnerTone(tone)) {
			destToneInfo.tone = tone + relationlist[GetLevelByTone(tone)];
			destToneInfo.group = group;
			if (destToneInfo.tone < 0) {
				destToneInfo.tone += 12;
				destToneInfo.group--;
			}
			else if (destToneInfo.tone >= 12) {
				destToneInfo.tone -= 12;
				destToneInfo.group++;
			}
		}
		vDestTone.push_back(destToneInfo);
	}
	return vDestTone;
}

QString CScale::ToneInfoToStr(const vector<s_ToneInfo> vTone)
{
	QString str = "";
	unsigned int i = 0;
	for(auto it = vTone.begin(); it != vTone.end(); ++it)
	{
		int group = (*it).group;
		int tone = (*it).tone;

		if (tone < 0) {
			str += " |\n";
		} else {
			if (i != 0 && vTone[i-1].tone != -1) {
				str += ",";
			}
			if (group == 0) {
				str += QString("%1").arg(GetToneName(tone));
			} else {
				str += QString("%1(%2)").arg(GetToneName(tone)).arg(group);
			}
		}
		i++;
	}
	return str;
}
