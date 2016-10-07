
#include <cmath>
#include<iostream>
//#define _OPENCL
#define GLOBALVIEW
#include "../Global.h"
#include "ProfTimer.h"

#include <vector>
#include <conio.h>
#include <string>

int tttt()
{
	
	//eeeeeeeeeeeeee
	
}


int nDataSaveNum(0);
int img_wid(1024);
int img_hig(800);
int img_bit(3);

const int nWidthParser(2048);;
const int nheightParser(1088);;

/*
퍼옴
http://stackoverflow.com/questions/5083465/fast-efficient-least-squares-fit-algorithm-in-c
*/

#define REAL float
//#define REAL double


inline static REAL sqr(REAL x) {
	return x*x;
}


int linreg(int n, const REAL x[], const REAL y[], REAL* m, REAL* b, REAL* r)
{
	REAL   sumx = 0.0;                        /* sum of x                      */
	REAL   sumx2 = 0.0;                       /* sum of x**2                   */
	REAL   sumxy = 0.0;                       /* sum of x * y                  */
	REAL   sumy = 0.0;                        /* sum of y                      */
	REAL   sumy2 = 0.0;                       /* sum of y**2                   */

	for (int i = 0; i<n; i++)
	{
		sumx += x[i];
		sumx2 += sqr(x[i]);
		sumxy += x[i] * y[i];;
		sumy += y[i];;
		sumy2 += sqr(y[i]);
	}

	REAL denom = (n * sumx2 - sqr(sumx));
	if (denom == 0) {
		// singular matrix. can't solve the problem.
		*m = 0;
		*b = 0;
		if (r) *r = 0;
		return 1;
	}

	*m = (n * sumxy - sumx * sumy) / denom;
	*b = (sumy * sumx2 - sumx * sumxy) / denom;
	if (r != NULL) {
		*r = (sumxy - sumx * sumy / n) /          /* compute correlation coeff     */
			sqrt((sumx2 - sqr(sumx) / n) *
			(sumy2 - sqr(sumy) / n));
	}

	return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////





unsigned char *TextData()
{
	Global::Image *pImageLoad = Global::DataLoad("../../../Resource/BmpFont/Consolas_32_32.bmp");
	Global::Image mImageLoad2(pImageLoad->width, pImageLoad->height, Global::TYPE::BYTE, 1);
	Global::RGB_GRAY(pImageLoad, &mImageLoad2);
	delete pImageLoad;
	return Gray_Bin(&mImageLoad2, 128);
	//Bin_GRAY(pImageLoad2, sTextData,255);
	//Text(sTextData, pImageLoad2,"TEST",40,40, RGB8(255,0,0));
	//Show("test...1", pImageLoad2);
}


class Parser_CSV
{
	float fY0;
	float fY1;
	Parser_CSV(float _fY0, float _fY1)
		: fY0(_fY0)
		, fY1(_fY1)
	{}
};

struct Sample
{
	int nX0;//Random Value X
	int nX1;//Random Value X
	int nSn;//Sample Number
	float m;//slope
	float b;//intersept
	float r;//correlation coefficient
};

void DataOpen(char* pFileName, float* pParser_CSV)
{
	FILE* f = fopen(pFileName, "rb");

	const int len(1000);
	char *pParser = " ,\t\n";
	char buff[len];

	while (fgets(buff, len, f))
	{
		char *token = strtok(buff, pParser);
		int nChoice(0);
		while (token != NULL)
		{
			float fData = (float)atof(token);

			switch (nChoice)
			{
			case 0:

				break;

			case 1:
				pParser_CSV++;
				*pParser_CSV = fData;
				//printf("token = %f\n", fData);
				break;

			case 2:

				break;

			}
			nChoice++;
			token = strtok(NULL, pParser);

		}
	}
	fclose(f);

}


void DotPoint(Global::Image *mImageParser, const int nPosX, const int nPosY, RGB8 &mRg)
{
	int nWidthLen = mImageParser->width * mImageParser->channel;
	if (mImageParser->width > nPosX && nPosX > 0)
	{
		if (mImageParser->height > nPosY && nPosY > 0)
		{
			mImageParser->data.b[nPosY * nWidthLen + (nPosX)] = mRg.B;
			mImageParser->data.b[nPosY * nWidthLen + (nPosX + 1)] = mRg.G;
			mImageParser->data.b[nPosY * nWidthLen + (nPosX + 2)] = mRg.R;
		}
	}
}



float fSurchDiff(10);
int Line(Global::Image *mImageParser, float* pParser_CSV, Sample &mSample, const bool bSw = false, const bool bVisible = false)
{
	//float fRandY0 = GlobalTool::Max(pParser_CSV[nRandX0], pParser_CSV[nRandX1]);
	//float fRandY1 = GlobalTool::Min(pParser_CSV[nRandX0], pParser_CSV[nRandX1]);


	float fLSX[2000] = { 0, };
	float fLSY[2000] = { 0, };
	int nCount = 0;
	mSample.nSn = 0;

	float fRandY0 = pParser_CSV[mSample.nX0];
	float fRandY1 = pParser_CSV[mSample.nX1];


	//int nMaxArray = GlobalTool::Max(mSample.nX0, mSample.nX1);
	//int nMinArray = GlobalTool::Min(mSample.nX0, mSample.nX1);

	float fU = mSample.nX1 - mSample.nX0;
	float fV = fRandY1 - fRandY0;
	float fM = (fV / fU);
	float nSurchDiff0;


	if (fM < 0)
	{
		nSurchDiff0 = fSurchDiff;
		//printf("slope : %f  %f \n",90 + degree, (atan(fM) * 180) / M_PI);
	}
	else
	{
		nSurchDiff0 = -fSurchDiff;
		//printf("slope : %f  %f \n", 90 - degree, (atan(fM) * 180) / M_PI);
	}

	float fDegree = (atan(fM) * 180) / M_PI;

	//double rad = atan2(fU, fV);
	//double degree = ((rad * 180) / M_PI);
	if (abs(fDegree) >= 10)
	{
		//printf("height : %f  %f  \n", fRandY0, fRandY1);
		//printf("slope : %f \n", (atan(fM) * 180) / M_PI);
		return 0;
	}


	if (mImageParser && bVisible)
	{
		Global::Circle(mImageParser, mSample.nX0, fRandY0, 15, RGB8(0, 255, 255));
		Global::Circle(mImageParser, mSample.nX1, fRandY1, 15, RGB8(0, 255, 255));
	}


	for (int i = 1; i < nWidthParser - 1; i++)
	{

		float nY0 = fM * ((i - 1) - mSample.nX0) + fRandY0;
		float nY1 = fM * ((i + 1) - mSample.nX0) + fRandY0;
		//printf("x: %d  y : %d  y : %d \n", i, nY0, nY1);
		if (nY0 < nheightParser || nY0 > 0)
		{
			if (mImageParser && bVisible)
			{
				Global::Line(mImageParser, (i - 1), nY0, (i + 1), nY1, RGB8(0, 255, 0));
				Global::Line(mImageParser, (i - 1) - nSurchDiff0, nY0 - fSurchDiff, (i + 1) - nSurchDiff0, nY1 - fSurchDiff, RGB8(0, 255, 0));
				Global::Line(mImageParser, (i - 1) + nSurchDiff0, nY0 + fSurchDiff, (i + 1) + nSurchDiff0, nY1 + fSurchDiff, RGB8(0, 255, 0));
			}


			if (pParser_CSV[i] != 0)
			{
				float fSurchPointMin = fM * ((i + 1) - mSample.nX0 + nSurchDiff0) + fRandY0;
				float fSurchPointMax = fM * ((i + 1) - mSample.nX0 - nSurchDiff0) + fRandY0;
				if (pParser_CSV[i + 1] > fSurchPointMin - fSurchDiff && pParser_CSV[i + 1] < fSurchPointMax + fSurchDiff)
				{
					fLSX[nCount] = i + 1;
					fLSY[nCount++] = pParser_CSV[i + 1];
					mSample.nSn++;
					if (mImageParser && bVisible)
					{
						Global::Circle(mImageParser, i + 1, pParser_CSV[i + 1], 2, RGB8(255, 0, 0));
					}
				}
				//Sleep(10);
				//Global::Show("Parser ", mImageParser);			
			}
		}
	}


	if (bSw)
	{
		linreg(nCount, fLSX, fLSY, &mSample.m, &mSample.b, &mSample.r);
		//Global::Line(mImageParser, 1, m + b, (mImageParser->width - nSurchDiff0), m*(mImageParser->width - nSurchDiff0) + b, RGB8(255, 0, 0));
		printf("m=%g b=%g r=%g  Ag=%f MeanAg=%f \n", mSample.m, mSample.b, mSample.r, fDegree, (atan(mSample.m) * 180) / M_PI);
	}
	if (mImageParser && bVisible)
	{
		for (int i = 1; i < nWidthParser - 1; i++)
		{
			float nY0 = mSample.m * (i - 1) + mSample.b;
			float nY1 = mSample.m * (i + 1) + mSample.b;
			if (nY0 < mImageParser->height || nY0 > 0)
			{
				Global::Line(mImageParser, (i - 1), nY0, (i + 1), nY1, RGB8(0, 255, 0));
			}
		}
	}
	return 1;
}


void DeletePoint(const int width, const int height, Sample &mSample, float* pParser_CSV)
{
	float fRandY0 = pParser_CSV[mSample.nX0];
	float fRandY1 = pParser_CSV[mSample.nX1];

	float nSurchDiff0;
	if (mSample.m < 0)
	{
		nSurchDiff0 = fSurchDiff;
	}
	else
	{
		nSurchDiff0 = -fSurchDiff;
	}


	for (int i = 1; i < width - 1; i++)
	{
		float fSurchPointMin = mSample.m * ((i + 1) - mSample.nX0 + nSurchDiff0) + fRandY0;
		float fSurchPointMax = mSample.m * ((i + 1) - mSample.nX0 - nSurchDiff0) + fRandY0;
		if (pParser_CSV[i + 1] > fSurchPointMin - nSurchDiff0 && pParser_CSV[i + 1] < fSurchPointMax + nSurchDiff0)
		{
			pParser_CSV[i] = 0;
		}

		float nY0 = mSample.m * (i - mSample.nX0) + fRandY0;
		if (nY0 > height || nY0 <= 0)
		{
			//break;
		}

	}
}

//Profile 재정렬
void ReAgainst(const int width, const int height, Sample &mSample, float* pParser_CSV)
{
	for (int i = 0; i < width; i++)
	{
		float fSurchPointMin = height - ((mSample.m * i) + mSample.b);
		//printf("fSurchPointMin %f \n", fSurchPointMin);
		pParser_CSV[i] += fSurchPointMin;
	}

}

#if 1
int main()
{
	ProfTimer mProfTimer;
	unsigned char *sTextData = TextData();


	float mArrVecParser_CSV[nWidthParser];
	float mArrVecParserCopy[nWidthParser];
	float mArrVecParserCopy2[nWidthParser];

	DataOpen("1909175498_image_profile.csv", mArrVecParser_CSV);
	//DataOpen("-1916029922_image_profile.csv", mArrVecParser_CSV);

	while (1)
	{

		memcpy(mArrVecParserCopy, mArrVecParser_CSV, sizeof(float)* nWidthParser);

		Global::Image mImageParser(nWidthParser, nheightParser, Global::TYPE::BYTE, 3);
		Global::Image mImageParser_Copy(nWidthParser, nheightParser, Global::TYPE::BYTE, 3);
		Global::Image mImageParser2(nWidthParser, nheightParser, Global::TYPE::BYTE, 3);
		Global::Image mImageParserReSize(img_wid, img_hig, Global::TYPE::BYTE, 3);

		for (int i = 0; i < nWidthParser; i++)
		{
			Global::Circle(&mImageParser, i, mArrVecParserCopy[i], 1, RGB8(0, 0, 255));
		}

		mImageParser_Copy = mImageParser;

#if 0
		int nXTest0 = 800;
		int nXTest1 = 900;
		int nTest;
		Sample sSample;
		sSample.nX0 = nXTest0;
		sSample.nX1 = nXTest1;
		Line(&mImageParser_Copy, mArrVecParserCopy, sSample, true);
		nXTest0 = 400;
		nXTest1 = 810;
		//Line(&mImageParser_Copy, nXTest0, nXTest1, mArrVecParserCopy, nTest, true);


		//DeletePoint(nWidthParser, nheightParser, 400, 700, mArrVecParserCopy);

#endif
#if 1



		int nLineCount(1);
		const int nSample = 1000;
		Sample sSample[nSample];
		while (nLineCount)
		{
			mProfTimer.Start();
			nLineCount--;

			int nArrNumderSize(0);
			int nSampleCount(0);
			memset(sSample, 0, sizeof(Sample)* nSample);

			int nMaxSample = 0;
			int nMaxSampleArrNum = 0;
			while (nSample - nArrNumderSize)
			{
				float fCutLine = nheightParser / 1.5;
				int nRandX0 = (std::rand() % nWidthParser);//중복은 나중에 고려
				int nRandX1 = (std::rand() % nWidthParser);//중복은 나중에 고려


				if (nRandX0 == nRandX1 || mArrVecParserCopy[nRandX0] < fCutLine || mArrVecParserCopy[nRandX1] < fCutLine)
				{
					continue;
				}

				sSample[nSampleCount].nX0 = nRandX0;
				sSample[nSampleCount].nX1 = nRandX1;
				if (Line(0, mArrVecParserCopy, sSample[nSampleCount]))
				{
					if (sSample[nSampleCount].nSn > nMaxSample && sSample[nSampleCount].nSn > 10)
					{
						nMaxSample = sSample[nSampleCount].nSn;
						nMaxSampleArrNum = nSampleCount;
						nSampleCount++;
						//printf(" nMaxSampleArrNum : %d \n", nMaxSampleArrNum);
					}
					//mImageParser_Copy = mImageParser;
					nArrNumderSize++;

				}



			}
			//mVecParserCopy


			if (nSampleCount > 0)
			{
				Line(0, mArrVecParserCopy, sSample[nMaxSampleArrNum], true);
				memcpy(mArrVecParserCopy2, mArrVecParser_CSV, sizeof(float)* nWidthParser);
				ReAgainst(nWidthParser, nheightParser, sSample[nMaxSampleArrNum], mArrVecParserCopy2);
			}
			mProfTimer.Stop();
			printf("mProfTimer : %f \n", mProfTimer.GetDurationInSecs());


			if (nSampleCount > 0)
			{
				Line(&mImageParser_Copy, mArrVecParserCopy, sSample[nMaxSampleArrNum], true);
				DeletePoint(nWidthParser, nheightParser, sSample[nMaxSampleArrNum], mArrVecParserCopy);


				for (int i = 0; i < nWidthParser; i++)
				{
					Global::Circle(&mImageParser2, i, mArrVecParserCopy2[i], 2, RGB8(0, 0, 255));
				}
			}
			//break;
		}





#endif
#if 0
		Global::Show("Parser ", &mImageParser_Copy);
		Global::Show("Test ", &mImageParser2);
#else	
		Global::Filter2D(&mImageParser2, &mImageParser2, Global::FILTER::GAUSSIAN, 1);
		Global::ReSize(&mImageParser2, &mImageParserReSize);
		Global::Show("Test ", &mImageParserReSize);

		Global::Filter2D(&mImageParser_Copy, &mImageParser_Copy, Global::FILTER::GAUSSIAN, 1);
		Global::ReSize(&mImageParser_Copy, &mImageParserReSize);
		Global::Show("Parser ", &mImageParserReSize);
#endif



		//Sleep(1);
	}

	delete sTextData;
	return 0;
}
#endif


