#include<opencv2/opencv.hpp>
#include<iostream>
#include "opencv2/core/core.hpp"   
#include "opencv2/imgproc/imgproc.hpp"  
#include <vector>
#include"config.h"

using namespace std;
using namespace cv;
int countOfEggs = 0;

struct ObjInformation
{
	Point curLocation;//特征点当前位置
	Point preLocation;//上一帧特征点位置
	Point nextLocation;//预测下一帧特征点位置
	bool isCurBeyondLine;//当前是否过线
	bool isPreBeyongLine;//前一帧是否过线
	bool isPreShipLoad;//前一帧是否装载
	bool isCurShipLoad;//当前是否装载
}obj[40] = {
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false },
		{ (0, 0), (0, 0), (0, 0), false, false, false, false }
};

ObjInformation obj_temp[40];//结构体数组副本
int ObjInformation_update_flag = 0;//对象装载标志
int r_Pixel = R_PIXEL;//搜索半径

void main(){
	while (1){
		Mat frameGRAY;
		Mat frameBINARY;
		Mat frameBINARYROI;
		Mat des_img;

		Mat imgUYVY(480, 720, CV_8UC2, vm_addr[vbuf.index]);
		cvtColor(imgUYVY, frameGRAY, COLOR_YUV2GRAY_UYVY);

		if (frameGRAY.empty()){
			break;
		}

		threshold(frameGRAY, frameBINARY, 180, 255, CV_THRESH_BINARY);
		frameBINARYROI = frameBINARY(Rect(XOFTOP_LEFT_CORNER, YOFTOP_LEFT_CORNER, WIDE_OF_RECTROI, HEIGHT_OF_RECTROI));
		for (int i = 0; i < ERODE_TIMES; i++){
			erode(frameBINARYROI, des_img, Mat(7, 7, CV_8U), Point(-1, -1), 3);
		}
		vector<vector<Point>> contours;//当前帧所有联通域的轮廓点集
		findContours(des_img, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		Moments M;
		double cX, cY;
		Point center_Point;
		vector<Point> centerPoints;//当前帧所有联通域的特征点（质心）点集
		for (int i = 0; i < contours.size(); i++){
			double eggArea = fabs(contourArea(contours.at(i)));
			if (eggArea > EGGS_AREA){
				M = moments(contours.at(i));
				cX = M.m10 / M.m00;
				cY = M.m01 / M.m00;
				center_Point.x = cX;
				center_Point.y = cY;
				centerPoints.push_back(center_Point);
			}
		}



		//------------------跟踪匹配器:主要处理目标跟踪匹配，目标状态更新-----------------------
		if (ObjInformation_update_flag == 0){
		}
		else{
			for (int i = 0; i < 40; i++){
				if (obj_temp[i].isCurShipLoad == false){
					continue;
				}
				double dis = 1000000;
				for (int j = 0; j < centerPoints.size(); j++){
					dis = sqrt(pow(centerPoints.at(j).x - obj_temp[i].nextLocation.x, 2) + pow(centerPoints.at(j).y - obj_temp[i].nextLocation.y, 2));
					if (dis < r_Pixel){
						obj_temp[i].preLocation = obj_temp[i].curLocation;
						obj_temp[i].curLocation = centerPoints.at(j);
						obj_temp[i].isPreShipLoad = obj_temp[i].isCurShipLoad;
						obj_temp[i].isCurShipLoad = true;
						if (centerPoints.at(j).y >DETECTION_LINE){
							obj_temp[i].isPreBeyongLine = obj_temp[i].isCurBeyondLine;
							obj_temp[i].isCurBeyondLine = true;
						}
						else{
							obj_temp[i].isPreBeyongLine = obj_temp[i].isCurBeyondLine;
							obj_temp[i].isCurBeyondLine = false;
						}
					}
					else if (obj_temp[i].curLocation.y>DELETING_LINE){
						Point resetLocation;
						resetLocation.x = 0;
						resetLocation.y = 0;
						obj_temp[i].curLocation = resetLocation;
						obj_temp[i].preLocation = resetLocation;
						obj_temp[i].nextLocation = resetLocation;
						obj_temp[i].isCurShipLoad = false;
						obj_temp[i].isPreShipLoad = false;
						obj_temp[i].isCurBeyondLine = false;
						obj_temp[i].isPreBeyongLine = false;
					}
				}
			}
		}


		//----------------------对象装载器：装载新加入的对象-------------------------
		for (int i = 0; i < centerPoints.size(); i++){
			double dis = 1000000;
			if (ObjInformation_update_flag == 0){
				int scan_flag = 0;
				for (int j = 0; j < 40; j++){
					if (obj[j].isCurShipLoad == true){
						dis = sqrt(pow(centerPoints.at(i).x - obj[j].curLocation.x, 2) + pow(centerPoints.at(i).y - obj[j].curLocation.y, 2));
					}
					if (dis < r_Pixel){
						break;
					}
					scan_flag++;
				}
				if (scan_flag == 40){
					for (int z = 0; z < 40; z++){
						if (obj[z].isCurShipLoad == false){
							obj[z].curLocation = centerPoints.at(i);
							obj[z].isCurShipLoad = true;
							int x = centerPoints.at(i).x;
							int y = centerPoints.at(i).y + MOVING_DISTANCE;
							Point predict;
							predict.x = x;
							predict.y = y;
							obj[z].nextLocation = predict;
							break;
						}
					}
				}
			}
			else{
				int scan_flag = 0;
				for (int j = 0; j < 40; j++){
					if (obj_temp[j].isCurShipLoad == true){
						dis = sqrt(pow(centerPoints.at(i).x - obj_temp[j].curLocation.x, 2) + pow(centerPoints.at(i).y - obj_temp[j].curLocation.y, 2));
					}
					if (dis < r_Pixel){
						obj_temp[j].curLocation.x = centerPoints.at(i).x;
						obj_temp[j].curLocation.y = centerPoints.at(i).y;
						Point predict;
						predict.x = centerPoints.at(i).x;
						predict.y = centerPoints.at(i).y + MOVING_DISTANCE;
						obj_temp[j].nextLocation = predict;
						break;
					}
					scan_flag++;
				}
				if (scan_flag == 40){
					for (int z = 0; z < 40; z++){
						if ((obj_temp[z].isCurShipLoad == false) && (centerPoints.at(i).y<DELETING_LINE)){
							obj_temp[z].curLocation = centerPoints.at(i);
							obj_temp[z].isCurShipLoad = true;
							int x = centerPoints.at(i).x;
							int y = centerPoints.at(i).y + MOVING_DISTANCE;
							Point predict;
							predict.x = x;
							predict.y = y;
							obj_temp[z].nextLocation = predict;
							break;
						}
					}
				}
			}
		}


		//----------------------计数器----------------------------
		int flag = 0;
		if (ObjInformation_update_flag == 0){
		}
		else{
			for (int i = 0; i < 40; i++){
				if ((obj_temp[i].isCurBeyondLine == true) && (obj_temp[i].isPreBeyongLine == false)){
					countOfEggs++;
					flag++;
				}
				else if (flag >= 40){
					break;
				}
			}
		}
		if (ObjInformation_update_flag == 0){//将所有点的状态信息
			for (int i = 0; i < 40; i++){
				obj_temp[i].curLocation = obj[i].curLocation;
				obj_temp[i].nextLocation = obj[i].nextLocation;
				obj_temp[i].preLocation = obj[i].preLocation;
				obj_temp[i].isCurBeyondLine = obj[i].isCurBeyondLine;
				obj_temp[i].isPreBeyongLine = obj[i].isPreBeyongLine;
				obj_temp[i].isCurShipLoad = obj[i].isCurShipLoad;
				obj_temp[i].isPreShipLoad = obj[i].isPreShipLoad;

			}
		}
		ObjInformation_update_flag++;
		if (ObjInformation_update_flag > 1048576){
			ObjInformation_update_flag = 1;
		}
	}
}