#ifndef CSCALE_H
#define CSCALE_H

#include <QObject>
#include <QDebug>
#include <math.h>
#include <regex>

//调弦
typedef enum _S_TURNING_ {
	TURNING_STANDARD = 0,
	TURNING_DROP_D,
	TURNING_LOW_C,

	TURNING_CNT
}turning_s;

//调式
typedef enum _S_TONALITY_ {
	TONAL_NATURAL_MAJOR = 0,
	TONAL_NATURAL_MINOR,

	TONAL_CNT
}tonal_s;

//音名
typedef enum _S_TONE_ {
	TONE_C = 0,
	TONE_Cs,   //C-Sharp
	TONE_D,
	TONE_Ds,
	TONE_E,
	TONE_F,
	TONE_Fs,
	TONE_G,
	TONE_Gs,
	TONE_A,
	TONE_As,
	TONE_B,

	TONE_CNT
} tone_s;

//和弦
typedef enum _S_CHORD_ {
	CHORD_NONE = 0,
	CHORD_MAJOR,
	CHORD_MINOR,
	CHORD_5,
	CHORD_SUS2,
	CHORD_SUS4,
	CHORD_AUG,
	CHORD_DIM,
	CHORD_MAJ7,
	CHORD_MIN7,
	CHORD_D7,
	CHORD_MIN7_5,
	CHORD_D7SUS4,
	CHORD_DIM7,
	CHORD_MAJ6,
	CHORD_MIN6,
	CHORD_ADD9,

	CHORD_CNT
} chord_s;

//变化音表示方式
typedef enum _S_MARK_ {
	MARK_SHARP = 0, //变化音以音名+'#'表示
	MARK_FLAT,		//变化音以音名+'b'表示
	MARK_NUM_SHARP, //变化音以数字+'#'表示
	MARK_NUM_FLAT,	//变化音以数字+'b'表示
}mark_s;

//common defines
#define INNER_TONE_CNT 7		//调内音
#define STRING_CNT 6			//弦数
#define CHORD_TONE_CNT_MAX 5	//最大和弦组成音个数
#define FREQUENCY_A0 440		//国际基准音A0
#define TBL_WIDTH 400			//表宽
#define TBL_HEIGHT 650			//表高
#define HALF_SCALE_CNT 12		//12平均律音个数

using namespace std;

typedef struct ToneInfo {
	int group;	//音所在组
	int tone;	//该组第几个音
} s_ToneInfo;

typedef struct _TONAL_INFO_ {
	QString name;
    array<int, INNER_TONE_CNT> order;	//元素代表每个音之间的距离, 下同
}tonalInfo;

typedef struct _CHORD_INFO_ {
	QString name;
	array<int, CHORD_TONE_CNT_MAX> order;
}chordInfo;

typedef struct _TURNING_INFO_ {
	QString name;
	array<int, STRING_CNT> order;
}turningInfo;

class CScale
{
public:
	CScale(int key, int tonality, int turning);

    void RefreshData(int key, int tonality);                    //改调或调式后刷新数据
    bool IsInnerTone(int tone);                                 //是否为调内音
    bool IsBaseTone(int tone);                                  //是否是非变化音
    bool IsChordTone(int tone);                                 //是否是和弦内音
    int GetLevelByTone(int tone);                               //查询某个音是调内几级音
    int GetChordList(array<int, CHORD_TONE_CNT_MAX> &dest);     //获取和弦组成音 return chord tone count
    int GetToneByName(QString name);                            //根据音名获取音

    vector<s_ToneInfo> AnalyStaff(const QString str);                                                   //解析曲谱
    vector<s_ToneInfo> Modulation(const vector<s_ToneInfo> before, int curKey, int destKey);            //转调
    vector<s_ToneInfo> TonalityConvert(const vector<s_ToneInfo> before, int curTonal, int destTonal);   //调式转换
    QString ToneInfoToStr(const vector<s_ToneInfo> vTone);                                              //音数据转为字符串信息
    void RefreshInnerToneList(int key, int tonality);                                                   //刷新调内音列表
	//void GetTonalityRelationList(int key1, int key2, int tonal1, int tonal2, int* list);

//inline:
	//Getter
	inline QString GetTitle() {return m_Name;}
	inline int GetCurKey() {return m_Key;}
	inline int GetCurTonality() {return m_Tonality;}
	inline int GetCurChord() {return m_Chord;}
    inline int GetCurMark() {return m_Mark;}

    inline QString GetToneName(int tone) {return s_CHalfScale[m_Mark][tone];}
	inline QString GetKeyName(int tone) {return s_CHalfScale[m_Mark%2][tone];}
	inline QString GetTonalityName(int tonal) {return s_TonalityInfo[tonal].name;}
	inline QString GetTurningName(int turning) {return s_TurningInfo[turning].name;}
	inline QString GetChordName(int chord) {return s_ChordInfo[chord].name;}
	inline QString GetRomaName(int level) {return s_RomaList[level];}
    inline int GetToneByLevel(int level) {return m_Member[level];}                                                  //根据级数获取音高
    inline int GetToneByPos(int fret, int gtString) {return (s_TurningInfo[m_Turning].order[gtString]+fret) % 12;}  //根据吉他指板位置获取音高
    inline int CalTonesDistance(int toneA, int toneB) {return abs(toneA-toneB);}                                    //获取音程
    inline int GetFrequency(int group, int tone) {return FREQUENCY_A0 * pow(2, ((tone-9)/12)+group);}               //获取音频率

	//Setter
	inline void SetKey(int key) {m_Key = key;}
	inline void SetTonality(int tonality) {m_Tonality = tonality;}
	inline void SetTurning(int turning) {m_Turning = turning;}
	inline void SetChord(int chord) {m_Chord = chord;}
	inline void SetAccidentalMark(int mark) {m_Mark = mark;}
	inline void SetChordLevel(int level) {m_ChordLevel = level;}

public:
	static const vector<array<QString, HALF_SCALE_CNT>> s_CHalfScale;
	static const vector<tonalInfo> s_TonalityInfo;
	static const vector<chordInfo> s_ChordInfo;
	static const vector<turningInfo> s_TurningInfo;
	static const array<QString, INNER_TONE_CNT> s_RomaList;
	static const array<char, INNER_TONE_CNT> s_CMajorScale;

private:
	QString m_Name;		//音阶名
	int m_Key;			//调
	int m_Tonality;		//调性
	int m_Turning;		//调弦
	int m_Chord;		//和弦
	int m_ChordLevel;	//和弦级数
	int m_Mark;			//变音记法 b/#
	array<int, INNER_TONE_CNT> m_Member;	//调内音级
};

#endif // CSCALE_H
