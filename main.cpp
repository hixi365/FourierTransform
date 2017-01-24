
#include <vector>

#include <DxLib.h>
#include <xaudio2.h>

#include "Key.h"

#pragma comment(lib,"xaudio2.lib")

#define WINDOW_W 1280
#define WINDOW_H 960

#define WIDE 1000

#define AC_W WIDE
#define AC_H 900
#define BACE_X 550
#define BACE_Y 480

#define F_W 30
#define F_P 5

#define FF_X 800
#define FF_Y 800

#define FF_W 200
#define FF_H 200

#define FNT_W 500
#define FT_W 20
#define ET_W 20

#define E_W	201
#define E_MAX 8.0f
#define E_T (20.0f / 1000.0f)

#define VOLUME (0.1f)

int mouse_x;
int mouse_y;
int mouse_move_x;
int mouse_move_y;
int obj_x = WINDOW_W / 2;
int obj_y = WINDOW_H / 2;
int obj_xs = obj_x;
int obj_ys = obj_y;

int mouse_left = 0;
int mouse_right = 0;
int mouse_center = 0;

bool flg_draw[7];

IXAudio2 *xaudio;
IXAudio2MasteringVoice *mastering_voice;

IXAudio2SourceVoice * foc;

int vav[WIDE];
float main_vav[WIDE];

int ac = 5;
float hz = 440;

float in_val[E_W];

float fsin[F_W];
float fcos[F_W];
float fvav[WIDE];

float fnt_v[FNT_W * 2 + 1];
float ft_v[FT_W * 2 + 1];
float et_v[ET_W * 2 + 1];

void InitArray()
{

	ZeroMemory(in_val, sizeof(in_val));
	ZeroMemory(fsin, sizeof(fsin));
	ZeroMemory(fcos, sizeof(fcos));
	ZeroMemory(fvav, sizeof(fvav));
	ZeroMemory(vav, sizeof(vav));
	ZeroMemory(main_vav, sizeof(vav));
	ZeroMemory(fnt_v, sizeof(fnt_v));
	ZeroMemory(ft_v, sizeof(ft_v));
	ZeroMemory(et_v, sizeof(et_v));

}

void init()
{
	HRESULT hr;

//	hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );
	hr = XAudio2Create( &xaudio, 0 );
	hr = xaudio->CreateMasteringVoice( &mastering_voice );
}

void cleanup()
{
	if (mastering_voice != 0){
		mastering_voice->DestroyVoice();
		mastering_voice = 0;
	}
	if (xaudio != 0){
		xaudio->Release();
		xaudio = 0;
	}
	CoUninitialize();
}

void DrawFrame()
{
	DrawLine(BACE_X - AC_W / 2, BACE_Y - AC_H / 2, BACE_X + AC_W / 2, BACE_Y - AC_H / 2, GetColor(200, 200, 200));
	DrawLine(BACE_X - AC_W / 2, BACE_Y + AC_H / 2, BACE_X + AC_W / 2, BACE_Y + AC_H / 2, GetColor(200, 200, 200));
	DrawLine(BACE_X - AC_W / 2, BACE_Y - AC_H / 2, BACE_X - AC_W / 2, BACE_Y + AC_H / 2, GetColor(200, 200, 200));
	DrawLine(BACE_X + AC_W / 2, BACE_Y - AC_H / 2, BACE_X + AC_W / 2, BACE_Y + AC_H / 2, GetColor(200, 200, 200));

	DrawLine(BACE_X - AC_W / 2, BACE_Y, BACE_X + AC_W / 2, BACE_Y, GetColor(200, 200, 200));

}

void DrawWave()
{
	for (int i = 0; i < WIDE; i++){
		vav[i] = main_vav[i] / E_MAX * AC_H / 2;
		DrawPixel(BACE_X - AC_W / 2 + i, BACE_Y - vav[i], GetColor(0, 255, 0));
	}
}

void DrawWaveF()
{
	for (int i = 0; i < WIDE; i++)
		DrawPixel(BACE_X - AC_W / 2 + i, BACE_Y - fvav[i] / E_MAX * AC_H / 2, GetColor(255, 255, 0));
}

void DrawWaveFNT()
{
	for (int i = 0; i < WIDE; i++){
		int f = (int)((float)i / (WIDE - 1) * (FNT_W * 2 + 1 - 1));
		DrawPixel(BACE_X - AC_W / 2 + i, BACE_Y - fnt_v[f], GetColor(255, 0, 0));
	}
}

void DrawWaveFT()
{
	for (int i = 0; i < WIDE; i++){
		float p = (float)i / (WIDE - 1) * (FT_W * 2 + 1 - 1);
		if (p - (int)p > 0.1f && 1 - (p - (int)p) > 0.1f)
			continue;
		if (p - (int)p > 0.5f)
			p += 0.5f;
		int f = (int)p;
		DrawLine(BACE_X - AC_W / 2 + i, BACE_Y, BACE_X - AC_W / 2 + i, BACE_Y - ft_v[f], GetColor(0, 0, 255));
	}
}

void DrawWaveET()
{
	for (int i = 0; i < WIDE; i++){
		float p = (float)i / (WIDE - 1) * (ET_W * 2 + 1 - 1);
		if (p - (int)p > 0.03f && 1 - (p - (int)p) > 0.03f)
			continue;
		if (p - (int)p > 0.5f)
			p += 0.5f;
		int f = (int)p;
		DrawLine(BACE_X - AC_W / 2 + i, BACE_Y, BACE_X - AC_W / 2 + i, BACE_Y - et_v[f], GetColor(255, 200, 50));
	}
}

void DrawWaveFF()
{
	for (int i = 0; i < FF_W; i++){
		DrawLine(FF_X + i, FF_Y, FF_X + i, FF_Y + fcos[(int)((float)i / F_W * FF_W)] * FF_H / 1000, GetColor(255, 0, 0));
		DrawLine(FF_X + i, FF_Y, FF_X + i, FF_Y - fsin[(int)((float)i / F_W * FF_W)] * FF_H / 1000, GetColor(0, 0, 255));
	}
}

void SetWavep()
{
	int a, b, c, d;
	a = obj_x<obj_xs ? obj_x : obj_xs;
	b = obj_x>obj_xs ? obj_x : obj_xs;
	c = obj_x<obj_xs ? obj_y : obj_ys;
	d = obj_x>obj_xs ? obj_y : obj_ys;
	a -= (BACE_X - AC_W / 2);
	b -= (BACE_X - AC_W / 2);
	c = -c + BACE_Y;
	d = -d + BACE_Y;
	for (int i = a; i <= b; i++){
		vav[i] = c + (double)(d - c) / (b - a)*(i - a);
		main_vav[i] = vav[i] * E_MAX / (AC_H / 2);
	}
}

void SetPattern(int p)
{

	switch (p)
	{
	case 0: // 初期値
		for (int i = 0; i < WIDE; i++)
			main_vav[i] = 0;
		break;

	case 1: // sin
		for (int i = 0; i < WIDE; i++)
			main_vav[i] = sin((float)i / WIDE * 3.1415 * 2) * E_MAX;
		break;

	case 2: // rand
		for (int i = 0; i < WIDE; i++)
			main_vav[i] = (rand() % 2000 - 1000) * 0.001f * E_MAX;
		break;

	default: // 初期値
		for (int i = 0; i < WIDE; i++)
			main_vav[i] = 0;
		break;

	}

}

void SetVoice(int f = 0)
{

	HRESULT hr;
	WAVEFORMATEX oformat = { 0 };

	if (foc != nullptr){
		foc->Stop();
		foc->DestroyVoice();
	}

	oformat.wFormatTag = WAVE_FORMAT_PCM;
	oformat.nChannels = 1;                   //チャンネル数
	oformat.wBitsPerSample = 16;             //１サンプルあたりのビット数
	oformat.nSamplesPerSec = 44100;          //サンプリング周波数
	oformat.nBlockAlign = oformat.wBitsPerSample / 8 * oformat.nChannels;
	oformat.nAvgBytesPerSec = oformat.nSamplesPerSec * oformat.nBlockAlign;

	XAUDIO2_SEND_DESCRIPTOR SFXSend[1];
	SFXSend[0].Flags = 1;
	SFXSend[0].pOutputVoice = mastering_voice;
	XAUDIO2_VOICE_SENDS SFXSendList;
	SFXSendList.pSends = SFXSend;
	SFXSendList.SendCount = 1;

	hr = xaudio->CreateSourceVoice(&foc, &oformat, 0, 2.0f, nullptr, NULL, NULL);

	std::vector< BYTE > data(oformat.nAvgBytesPerSec * 1);     //バッファ
	short* p = (short*)&data[0];
	
	if (f == 0){
		for (size_t i = 0; i < data.size() / 2; i++){
			*p = (short)(32767 * main_vav[(int)((float)i / data.size() / 2 * hz * WIDE) % WIDE] / E_MAX * VOLUME);
			p++;
		}
	}
	else if (f == 1){
		for (size_t i = 0; i < data.size() / 2; i++){
			*p = (short)(32767 * fvav[(int)((float)i / data.size() / 2 * hz * WIDE) % WIDE] / E_MAX * VOLUME);
			p++;
		}
	}
	
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = data.size();    
	buffer.pAudioData = &data[0];        
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	foc->SubmitSourceBuffer(&buffer);

	foc->Start();
	Sleep(1000);
}

void MakeFourier()
{

	// 台形公式
	for (int j = 0; j < F_W; j++){
		fcos[j] = 0;
		fsin[j] = 0;
		float pitch = (float)F_P;
		for (float p = 0; p + pitch <= WIDE; p += pitch){

			float p0 = p;
			int i0_0 = (int)p0;
			int i0_1 = (int)(p0 + 0.5f);
			float e0 = p0 - (int)p0;
			float v0 = main_vav[i0_0] * (1 - e0) + main_vav[i0_1] * e0;
			float c0 = cos((float)p0 / WIDE * 3.1415 * 2 * j);
			float s0 = sin((float)p0 / WIDE * 3.1415 * 2 * j);
			float cv0 = c0 * v0;
			float sv0 = s0 * v0;

			float p1 = p + pitch;
			int i1_0 = (int)p1;
			int i1_1 = (int)(p1 + 0.5f);
			float e1 = p1 - (int)p1;
			float v1 = main_vav[i1_0] * (1 - e1) + main_vav[i1_1] * e1;
			float c1 = cos((float)p1 / WIDE * 3.1415 * 2 * j);
			float s1 = sin((float)p1 / WIDE * 3.1415 * 2 * j);
			float cv1 = c1 * v1;
			float sv1 = s1 * v1;

			float cs = (cv0 + cv1) / 2 * (pitch / WIDE);
			float ss = (sv0 + sv1) / 2 * (pitch / WIDE);

			fcos[j] += cs;
			fsin[j] += ss;
		}
	}

	fcos[0] /= 2;

	//// 台形公式 面積
	//float coss[F_W] = {};
	//float sins[F_W] = {};
	//for (int j = 0; j < F_W; j++){
	//	fcos[j] = 0;
	//	fsin[j] = 0;
	//	float pitch = (float)F_P;
	//	for (float p = 0; p + pitch < WIDE; p += pitch){

	//		float p0 = p;
	//		int i0_0 = (int)p0;
	//		int i0_1 = (int)(p0 + 0.5f);
	//		float e0 = p0 - (int)p0;
	//		float v0 = main_vav[i0_0] * (1 - e0) + main_vav[i0_1] * e0;
	//		float c0 = cos((float)p0 / WIDE * 3.1415 * 2 * j);
	//		float s0 = sin((float)p0 / WIDE * 3.1415 * 2 * j);
	//		float cv0 = c0 * v0;
	//		float sv0 = s0 * v0;

	//		float p1 = p + pitch;
	//		int i1_0 = (int)p1;
	//		int i1_1 = (int)(p1 + 0.5f);
	//		float e1 = p1 - (int)p1;
	//		float v1 = main_vav[i1_0] * (1 - e1) + main_vav[i1_1] * e1;
	//		float c1 = cos((float)p1 / WIDE * 3.1415 * 2 * j);
	//		float s1 = sin((float)p1 / WIDE * 3.1415 * 2 * j);
	//		float cv1 = c1 * v1;
	//		float sv1 = s1 * v1;

	//		float cs = (cv0 + cv1) * (pitch / WIDE);
	//		float ss = (sv0 + sv1) * (pitch / WIDE);

	//		float css = (c0 * c0 + c1 * c1) * (pitch / WIDE);
	//		float sss = (s0 * s0 + s1 * s1) * (pitch / WIDE);

	//		fcos[j] += cs;
	//		fsin[j] += ss;

	//		coss[j] += css;
	//		sins[j] += sss;

	//	}

	//	fcos[j] /= coss[j];
	//	fsin[j] /= sins[j];

	//}

	// 非周期信号のフーリエ変換
	for (int k = 0; k < 2 * FNT_W + 1; k++){
		float n = k - FNT_W;
		fnt_v[k] = 0;
		float fnt_c = 0;
		float fnt_s = 0;
		float pitch = (float)F_P;
		for (float p = 0; p + pitch < WIDE; p += pitch){
	
			float j = n / FNT_W * 20;
	
			float p0 = p;
			int i0_0 = (int)p0;
			int i0_1 = (int)(p0 + 0.5f);
			float e0 = p0 - (int)p0;
			float v0 = main_vav[i0_0] * (1 - e0) + main_vav[i0_1] * e0;
			float c0 = cos((float)p0 / WIDE * 3.1415 * 2 * j);
			float s0 = sin((float)p0 / WIDE * 3.1415 * 2 * j);
			float cv0 = c0 * v0;
			float sv0 = s0 * v0;
	
			float p1 = p + pitch;
			int i1_0 = (int)p1;
			int i1_1 = (int)(p1 + 0.5f);
			float e1 = p1 - (int)p1;
			float v1 = main_vav[i1_0] * (1 - e1) + main_vav[i1_1] * e1;
			float c1 = cos((float)p1 / WIDE * 3.1415 * 2 * j);
			float s1 = sin((float)p1 / WIDE * 3.1415 * 2 * j);
			float cv1 = c1 * v1;
			float sv1 = s1 * v1;
	
			float cs = (cv0 + cv1) / 2 * (pitch / WIDE);
			float ss = (sv0 + sv1) / 2 * (pitch / WIDE);
	
			fnt_c += cs;
			fnt_s += ss;
	
		}
	
		fnt_v[k] += sqrt(fnt_c * fnt_c + fnt_s * fnt_s);
	
	}
	
	// 周期信号のフーリエ変換
	for (int k = 0; k < 2 * FT_W + 1; k++){
		float n = k - FT_W;
		ft_v[k] = 0;
		float ft_c = 0;
		float ft_s = 0;
		float pitch = (float)F_P;
		for (float p = 0; p + pitch < WIDE; p += pitch){
	
			float j = n / FT_W * 20;
	
			float p0 = p;
			int i0_0 = (int)p0;
			int i0_1 = (int)(p0 + 0.5f);
			float e0 = p0 - (int)p0;
			float v0 = main_vav[i0_0] * (1 - e0) + main_vav[i0_1] * e0;
			float c0 = cos((float)p0 / WIDE * 3.1415 * 2 * j);
			float s0 = sin((float)p0 / WIDE * 3.1415 * 2 * j);
			float cv0 = c0 * v0;
			float sv0 = s0 * v0;
	
			float p1 = p + pitch;
			int i1_0 = (int)p1;
			int i1_1 = (int)(p1 + 0.5f);
			float e1 = p1 - (int)p1;
			float v1 = main_vav[i1_0] * (1 - e1) + main_vav[i1_1] * e1;
			float c1 = cos((float)p1 / WIDE * 3.1415 * 2 * j);
			float s1 = sin((float)p1 / WIDE * 3.1415 * 2 * j);
			float cv1 = c1 * v1;
			float sv1 = s1 * v1;
	
			float cs = (cv0 + cv1) / 2 * (pitch / WIDE);
			float ss = (sv0 + sv1) / 2 * (pitch / WIDE);
	
			ft_c += cs;
			ft_s += ss;
	
		}
	
		ft_v[k] += sqrt(ft_c * ft_c + ft_s * ft_s) / E_T;	// 周期で割るべき？
	
	}

	// 電力スペクトル密度
	for (int k = 0; k < 2 * ET_W + 1; k++){
		et_v[k] = ft_v[k] * ft_v[k] / E_T;
	}

	// 書き込み
	for (int i = 0; i < WIDE; i++){
		fvav[i] = 0;
		for (int j = 0; j < F_W; j++){
			fvav[i] += fcos[j] * cos((float)i / WIDE * 3.1415 * 2 * j) * 2;
			fvav[i] += fsin[j] * sin((float)i / WIDE * 3.1415 * 2 * j) * 2;
		}
	}

}

void LoadInput()
{

	FILE * fp;
	errno_t err = fopen_s(&fp, "input.csv", "r");

	if (err)
		return;

	ZeroMemory(in_val, sizeof(in_val));

	float data;
	for (int i = 0; i < E_W; i++){
		if (fscanf_s(fp, "%f", &data) == EOF)
			break;
		in_val[i] = data;
	}

	for (int i = 0; i < WIDE; i++){
		float p = (float)i / (WIDE - 1) * (E_W - 1);
		int i0 = (int)p;
		int i1 = (int)(p + 0.5f);
		float e = p - (int)p;
		float v = in_val[i0] * (1 - e) + in_val[i1] * e;
		main_vav[i] = v;
	}

	fclose(fp);

}

void OutCsv()
{

	//{
	//	FILE * fp = nullptr;
	//	errno_t err = fopen_s(&fp, "inin.csv", "w");

	//	if (err)
	//		return;

	//	for (int i = 0; i < 20; i++){
	//		for (int j = 0; j < 10; j++){
	//			fprintf(fp, "%f,", in_val[i*10+j]);
	//		}
	//		fprintf(fp, "\n");
	//	}

	//	fclose(fp);
	//}

	{
		FILE * fp = nullptr;
		errno_t err = fopen_s(&fp, "output_k1.csv", "w");

		if (err)
			return;

		for (int i = 0; i < F_W; i++){
			fprintf(fp, "%f,%f\n", fcos[i], fsin[i]);
		}

		fclose(fp);
	}

	{
		FILE * fp = nullptr;
		errno_t err = fopen_s(&fp, "output_k2.csv", "w");

		if (err)
			return;

		for (int i = 0; i < FNT_W * 2 + 1; i++){
			fprintf(fp, "%d,%f\n", i - FNT_W, fnt_v[i]);
		}

		fclose(fp);
	}

	{
		FILE * fp = nullptr;
		errno_t err = fopen_s(&fp, "output_k3.csv", "w");

		if (err)
			return;

		for (int i = 0; i < FT_W * 2 + 1; i++){
			fprintf(fp, "%d,%f\n", i - FT_W, ft_v[i]);
		}

		fclose(fp);
	}

	{
		FILE * fp = nullptr;
		errno_t err = fopen_s(&fp, "output_k4.csv", "w");

		if (err)
			return;

		for (int i = 0; i < ET_W * 2 + 1; i++){
			fprintf(fp, "%d,%f\n", i - ET_W, et_v[i]);
		}

		fclose(fp);
	}

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	SetOutApplicationLogValidFlag(false);		//log false
	SetMainWindowText("Remilia_is_sleepeng_alone");			//ウインドウのタイトル
	ChangeWindowMode(TRUE);						//ウインドウモード
	SetGraphMode(WINDOW_W, WINDOW_H, 32);					//画面のサイズ
	SetWindowSizeChangeEnableFlag(true);		//サイズ変更	
	//	SetAlwaysRunFlag(true);						//フォーカス
	if (DxLib_Init() == -1) return -1;			//初期化式 
	SetDrawScreen(DX_SCREEN_BACK);			//裏描画設定

	
	SetMouseDispFlag(false);

	init();


	while (ScreenFlip() == 0 && ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0){

		ClearDrawScreen();

		ReGetKey();

		/*マウスの座標習得*/
		GetMousePoint(&mouse_x, &mouse_y);
		mouse_move_x = mouse_x - AC_W / 2;
		mouse_move_y = mouse_y - AC_H / 2;

		SetMousePoint(AC_W / 2, AC_H / 2);

		obj_xs = obj_x;
		obj_ys = obj_y;

		obj_x += mouse_move_x;
		obj_y += mouse_move_y;

		if (obj_x < BACE_X - AC_W/2)
			obj_x = BACE_X - AC_W / 2;
		if (obj_x >= BACE_X + AC_W / 2)
			obj_x = BACE_X + AC_W / 2 - 1;
		if (obj_y < BACE_Y - AC_H/2)
			obj_y = BACE_Y - AC_H / 2;
		if (obj_y >= BACE_Y + AC_H / 2)
			obj_y = BACE_Y + AC_H / 2 - 1;

		if (GetMouseInput() & MOUSE_INPUT_LEFT)
			mouse_left++;
		else
			mouse_left = 0;

		if (GetMouseInput() & MOUSE_INPUT_RIGHT)
			mouse_right++;
		else
			mouse_right = 0;

		if (GetMouseInput() & MOUSE_INPUT_MIDDLE)
			mouse_center++;
		else
			mouse_center = 0;

		if (mouse_left)
			SetWavep();

		if (GetKey(1) == 1)
			InitArray();

		if (GetKey(2) == 1)
			MakeFourier();

		if (GetKey(20) == 1)
			flg_draw[0] = !flg_draw[0];

		if (GetKey(21) == 1)
			flg_draw[1] = !flg_draw[1];

		if (GetKey(22) == 1)
			flg_draw[2] = !flg_draw[2];

		if (GetKey(23) == 1)
			flg_draw[3] = !flg_draw[3];

		if (GetKey(24) == 1)
			flg_draw[4] = !flg_draw[4];

		if (GetKey(25) == 1)
			flg_draw[5] = !flg_draw[5];

		if (GetKey(26) == 1)
			flg_draw[6] = !flg_draw[6];

		if (GetKey(11) == 1)
			hz /= 1.059463094;

		if (GetKey(13) == 1)
			hz *= 1.059463094;

		if (GetKey(3) == 1)
			SetVoice(0);

		if (GetKey(30) == 1)
			SetVoice(1);

		if (GetKey(31) == 1)
			SetPattern(1);

		if (GetKey(32) == 1)
			SetPattern(2);



		if (GetKey(44) == 1)
			OutCsv();

		if (GetKey(55) == 1)
			LoadInput();

		DrawFrame();
		
		if(!flg_draw[0]) DrawWaveFF();
		if(!flg_draw[1]) DrawWave();
		if(!flg_draw[2]) DrawWaveF();
		if(!flg_draw[3]) DrawWaveFNT();
		if(!flg_draw[4]) DrawWaveET();
		if(!flg_draw[5]) DrawWaveFT();

		if(!flg_draw[6]) DrawCircle(obj_x, obj_y, 5, GetColor(255, 0, 0), 1);		


	}

	cleanup();

	DxLib_End();

	return 0;
}