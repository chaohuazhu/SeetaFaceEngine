/*
*
* This file is part of the open-source SeetaFace engine, which includes three modules:
* SeetaFace Detection, SeetaFace Alignment, and SeetaFace Identification.
*
* This file is part of the SeetaFace Identification module, containing codes implementing the
* face identification method described in the following paper:
*
*
*   VIPLFaceNet: An Open Source Deep Face Recognition SDK,
*   Xin Liu, Meina Kan, Wanglong Wu, Shiguang Shan, Xilin Chen.
*   In Frontiers of Computer Science.
*
*
* Copyright (C) 2016, Visual Information Processing and Learning (VIPL) group,
* Institute of Computing Technology, Chinese Academy of Sciences, Beijing, China.
*
* The codes are mainly developed by Jie Zhang(a Ph.D supervised by Prof. Shiguang Shan)
*
* As an open-source face recognition engine: you can redistribute SeetaFace source codes
* and/or modify it under the terms of the BSD 2-Clause License.
*
* You should have received a copy of the BSD 2-Clause License along with the software.
* If not, see < https://opensource.org/licenses/BSD-2-Clause>.
*
* Contact Info: you can send an email to SeetaFace@vipl.ict.ac.cn for any problems.
*
* Note: the above information must be kept whenever or wherever the codes are used.
*
*/
#include <windows.h>
#include<iostream>
using namespace std;
#ifdef _WIN32
#pragma once
#include <opencv2/core/version.hpp>

#define CV_VERSION_ID CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) \
  CVAUX_STR(CV_SUBMINOR_VERSION)

#ifdef _DEBUG
#define cvLIB(name) "opencv_" name CV_VERSION_ID "d"
#else
#define cvLIB(name) "opencv_" name CV_VERSION_ID
#endif //_DEBUG

#pragma comment( lib, cvLIB("core") )
#pragma comment( lib, cvLIB("imgproc") )
#pragma comment( lib, cvLIB("highgui") )

#endif //_WIN32

#if defined(__unix__) || defined(__APPLE__)

#ifndef fopen_s

#define fopen_s(pFile,filename,mode) ((*(pFile))=fopen((filename),(mode)))==NULL

#endif //fopen_s

#endif //__unix

//#include <opencv/cv.h>
//#include <opencv/highgui.h>
//#include <opencv2/imgproc/imgproc.hpp> 
//#include <opencv2/imgcodecs/imgcodecs.hpp>
//
//using namespace cv;

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp> 
#include <opencv2/imgcodecs/imgcodecs.hpp>
using namespace cv;

#include "face_identification.h"
#include "recognizer.h"
#include "face_detection.h"
#include "face_alignment.h"

#include "math_functions.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

using namespace seeta;

#define TEST(major, minor) major##_##minor##_Tester()
#define EXPECT_NE(a, b) if ((a) == (b)) std::cout << "ERROR: "
#define EXPECT_EQ(a, b) if ((a) != (b)) std::cout << "ERROR: "

#ifdef _WIN32
std::string DATA_DIR = "../../data/";
std::string MODEL_DIR = "../../model/";
#else
std::string DATA_DIR = "./data/";
std::string MODEL_DIR = "./model/";
#endif

#include <fstream>
#include <io.h>
#include <string>
//std::string SrcPath = "D:/BaiduNetdiskDownload/CASIA-WebFace";
//std::string DstPath = "D:/BaiduNetdiskDownload/WebFace_Align";

//std::string SrcPath = "D:/BaiduNetdiskDownload/lfw";
//std::string DstPath = "D:/BaiduNetdiskDownload/lfw_Align";
std::string SrcPath = "D:/BaiduNetdiskDownload/lfw_Align";
std::string DstPath = "D:/BaiduNetdiskDownload/lfw_Align_half";

////std::string SrcPath = "D:/BaiduNetdiskDownload/lfw_Align_half";
////std::string DstPath = "D:/BaiduNetdiskDownload/lfw_Align_half_seq";

//std::string SrcPath = "D:/BaiduNetdiskDownload/WebFace_Align";
//std::string DstPath = "D:/BaiduNetdiskDownload/WebFace_Align_half";
////std::string SrcPath = "D:/BaiduNetdiskDownload/WebFace_Align_half";
////std::string DstPath = "D:/BaiduNetdiskDownload/WebFace_Align_half_seq";

int seq_label = 0;
int seq_file = 1;
#include <iostream>
#include <iomanip>


void createFilesList(string path, string lpath, int label, vector<string>& files) {
	intptr_t hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	stringstream ss;
	if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo)) != -1) {
		do {
			if ((fileinfo.attrib & _A_SUBDIR)) {
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
					//cout << "mkdir " << p.assign(DstPath).append("/").append(fileinfo.name).c_str() << endl;
					//CreateDirectory(p.assign(DstPath).append("/").append(fileinfo.name).c_str(), NULL);

					createFilesList(p.assign(path).append("/").append(fileinfo.name), fileinfo.name, seq_label++,files);
				}
			}
			else {
				stringstream ss;
				ss << label;
				files.push_back(DstPath+"/"+p.assign(lpath).append("/").append(fileinfo.name)+" " + ss.str() );
			}
		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}
}
void getAllFiles(string path, string lpath, vector<string>& files) {
  intptr_t hFile = 0;
  struct _finddata_t fileinfo;
  string p;
  if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo)) != -1) {
    do {
      if ((fileinfo.attrib & _A_SUBDIR)) {  
        if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
		  cout << "mkdir " << p.assign(DstPath).append("/").append(fileinfo.name).c_str() << endl;
		  CreateDirectory(p.assign(DstPath).append("/").append(fileinfo.name).c_str(), NULL);
          getAllFiles(p.assign(path).append("/").append(fileinfo.name), fileinfo.name, files);
        }
      }
      else {
		files.push_back(p.assign(lpath).append("/").append(fileinfo.name));
      }
    } while (_findnext(hFile, &fileinfo) == 0);

    _findclose(hFile);
  }
}
////void getAllFiles_seq(string path, string lpath, string npath,vector<string>& files) {
////	intptr_t hFile = 0;
////	struct _finddata_t fileinfo;
////	string p;
////	stringstream ss;
////
////	if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo)) != -1) {
////		do {
////			if ((fileinfo.attrib & _A_SUBDIR)) {
////				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
////					ss <<  setw(4) << setfill('0') << seq_label++;
////					string& newpath = ss.str();
////					ss.str("");
////					cout << "create" + DstPath + "/" + newpath  << endl;
////					CreateDirectory((DstPath + "/" + newpath).c_str(), NULL);
////					seq_file = 1;
////					getAllFiles_seq(p.assign(path).append("/").append(fileinfo.name), fileinfo.name,newpath, files);
////				}
////			}
////			else {
////				ss << setw(4) << setfill('0') << seq_file++;
////				string& newfile = ss.str();
////				ss.str("");
////				cout << "copyfrom" + path + "/" + fileinfo.name << endl;
////				cout << "copyto" + DstPath + "/" + npath + "/" + newfile + ".jpg" << endl;
////				CopyFile((path + "/" + fileinfo.name).c_str(), (DstPath + "/" + npath + "/" + newfile + ".jpg").c_str(), FALSE);
////				files.push_back(p.assign(lpath).append("/").append(fileinfo.name));
////			}
////		} while (_findnext(hFile, &fileinfo) == 0);
////
////		_findclose(hFile);
////	}
////}
int halfimage(string path) {
	//cvNamedWindow("Imgae Before Processing");
	//cvNamedWindow("Image After Processing"); 
	IplImage * in = cvLoadImage((SrcPath + "/" + path).c_str(), 1);  
	IplImage * out = cvCreateImage(cvSize(in->width / 2, in->height / 2), in->depth, in->nChannels);
	IplImage* img_show = cvCloneImage(in);
	cvPyrDown(in, out);
	cvSetImageROI(out, cvRect(16, 8, 96, 112));
	cvSaveImage((DstPath + "/" + path).c_str(), out);
	cvResetImageROI(out); 
	//cvShowImage("Imgae Before Processing", in);
	//cvShowImage("Imgae After Processing", out); 
	cvReleaseImage(&in);
	cvReleaseImage(&out);
	cvReleaseImage(&img_show); 
	//cvDestroyWindow("Imgae Before Processing");
	//cvDestroyWindow("Imgae After Processing");
	return 0;
}
void binary_image_show()
{
	//FILE *fpw = fopen("a.dat", "wb");
	//if (fpw == NULL)
	//{
	//	cout << "Open error!" << endl;
	//	fclose(fpw);
	//	return;
	//}

	//Mat image = imread("E:\\lena32.jpg");
	//if (!image.data || image.channels() != 3)
	//{
	//	cout << "Image read failed or image channels isn't equal to 3."
	//		<< endl;
	//	return;
	//}

	//// write image to binary format file
	//int rows = image.rows;
	//int cols = image.cols;

	//char* dp = (char*)image.data;
	//for (int i = 0; i<rows*cols; i++)
	//{
	//	fwrite(&dp[i * 3], sizeof(char), 1, fpw);
	//	fwrite(&dp[i * 3 + 1], sizeof(char), 1, fpw);
	//	fwrite(&dp[i * 3 + 2], sizeof(char), 1, fpw);
	//}
	//fclose(fpw);

	// read image from binary format file
	FILE *fpr = fopen("a.dat", "rb");
	if (fpr == NULL)
	{
		cout << "Open error!" << endl;
		fclose(fpr);
		return;
	}
	int rows = 256;
	int cols = 256;

	Mat image2(rows, cols, CV_8UC3, Scalar::all(0));
	char* pData = (char*)image2.data;
	//for (int i = 0; i<rows*cols; i++)
	//{
	//	fread(&pData[i * 3], sizeof(char), 1, fpr);
	//	fread(&pData[i * 3 + 1], sizeof(char), 1, fpr);
	//	fread(&pData[i * 3 + 2], sizeof(char), 1, fpr);
	//}
	char* const char_dst_head_r = new char[256*256];
	char* const char_dst_head_g = new char[256 * 256];
	char* const char_dst_head_b = new char[256 * 256];
	fread(char_dst_head_r, sizeof(char), 256*256, fpr);
	fread(char_dst_head_g, sizeof(char), 256 * 256, fpr);
	fread(char_dst_head_b, sizeof(char), 256 * 256, fpr);

	for (int i = 0; i<rows*cols; i++)
	{
		pData[i * 3] = char_dst_head_b[i];
		pData[i * 3 + 1] = char_dst_head_g[i];
		pData[i * 3 + 2] = char_dst_head_r[i];
	}
	fclose(fpr);

	imshow("1", image2);
	waitKey(0);
}
int main(int argc, char* argv[]) {
  vector<string> files;
  binary_image_show();
  return 0;
  //createFilesList(DstPath, "", 0, files);
  //cout << files.size() << endl;
  //ofstream ofile;
  //ofile.open("lfw_all.txt");
  //for (int i = 0; i < files.size(); i++)
  //{
	 // ofile << files[i].c_str() << endl;
  //}
  //ofile.close();
  //return 0;

  ////seq_label = 0;
  ////seq_file = 0;
  ////getAllFiles_seq(SrcPath, "","", files);
  ////return 0;

  //getAllFiles(SrcPath, "", files);
  //cout << files.size() << endl;
  //for (int i = 0; i < files.size(); i++)
  //{
  // string p;
  // cout << files[i].c_str() << endl;

  // halfimage(files[i]);
  //}
  //return 0;

  //getAllFiles(SrcPath, "", files);
  // Initialize face detection model
  seeta::FaceDetection detector("seeta_fd_frontal_v1.0.bin");
  detector.SetMinFaceSize(40);
  detector.SetScoreThresh(2.f);
  detector.SetImagePyramidScaleFactor(0.8f);
  detector.SetWindowStep(4, 4);
  // Initialize face alignment model 
  seeta::FaceAlignment point_detector("seeta_fa_v1.1.bin");
  // Initialize face Identification model 
  //FaceIdentification face_recognizer((MODEL_DIR + "seeta_fr_v1.0.bin").c_str());
  FaceIdentification face_recognizer("seeta_fr_v1.0.bin");
  //std::string test_dir = DATA_DIR + "test_face_recognizer/";

  ////load image
  //cv::Mat gallery_img_color = cv::imread(test_dir + "images/src/NF_200003_002.jpg", 1);
  cv::Mat gallery_img_color = cv::imread("094.jpg", 1);
  cv::Mat gallery_img_gray;
  cv::cvtColor(gallery_img_color, gallery_img_gray, CV_BGR2GRAY);

  //cv::Mat probe_img_color = cv::imread(test_dir + "images/compare_im/Aaron_Peirsol_0004.jpg", 1);
  cv::Mat probe_img_color = cv::imread("002.jpg", 1);
  cv::Mat probe_img_gray;
  cv::cvtColor(probe_img_color, probe_img_gray, CV_BGR2GRAY);

  ImageData gallery_img_data_color(gallery_img_color.cols, gallery_img_color.rows, gallery_img_color.channels());
  gallery_img_data_color.data = gallery_img_color.data;

  ImageData gallery_img_data_gray(gallery_img_gray.cols, gallery_img_gray.rows, gallery_img_gray.channels());
  gallery_img_data_gray.data = gallery_img_gray.data;

  ImageData probe_img_data_color(probe_img_color.cols, probe_img_color.rows, probe_img_color.channels());
  probe_img_data_color.data = probe_img_color.data;

  ImageData probe_img_data_gray(probe_img_gray.cols, probe_img_gray.rows, probe_img_gray.channels());
  probe_img_data_gray.data = probe_img_gray.data;

  cv::Mat gallery_img_crop_color(256, 256, CV_8UC(3));
  ImageData gallery_img_data_crop_color(256, 256, 3);
  gallery_img_data_crop_color.data = gallery_img_crop_color.data;
  cv::Mat probe_img_crop_color(256, 256, CV_8UC(3));
  ImageData probe_img_data_crop_color(256, 256, 3);
  probe_img_data_crop_color.data = probe_img_crop_color.data;

  // Detect faces
  std::vector<seeta::FaceInfo> gallery_faces = detector.Detect(gallery_img_data_gray);
  int32_t gallery_face_num = static_cast<int32_t>(gallery_faces.size());

  std::vector<seeta::FaceInfo> probe_faces = detector.Detect(probe_img_data_gray);
  int32_t probe_face_num = static_cast<int32_t>(probe_faces.size());

  if (gallery_face_num == 0 || probe_face_num==0)
  {
    std::cout << "Faces are not detected.";
    return 0;
  }
  cout << "detect face " << gallery_face_num << " " << probe_face_num << endl;
  //if (score) max
  int max_id = 0;
  for (int i = 0; i < gallery_face_num; i++) {
	  if (gallery_faces[i].score >= gallery_faces[max_id].score)
		  max_id = i;
	  cout << "face detect score: " << gallery_faces[i].score << endl;
  }
  for (int i = 0; i < probe_face_num; i++) {
	  cout << "face detect score: " << probe_faces[i].score << endl;
  }

  // Detect 5 facial landmarks
  seeta::FacialLandmark gallery_points[5];
  point_detector.PointDetectLandmarks(gallery_img_data_gray, gallery_faces[max_id], gallery_points);

  seeta::FacialLandmark probe_points[5];
  point_detector.PointDetectLandmarks(probe_img_data_gray, probe_faces[0], probe_points);

  for (int i = 0; i<5; i++)
  {
    cv::circle(gallery_img_color, cv::Point(gallery_points[i].x, gallery_points[i].y), 2,
      CV_RGB(0, 255, 0));
    cv::circle(probe_img_color, cv::Point(probe_points[i].x, probe_points[i].y), 2,
      CV_RGB(0, 255, 0));
  }

  face_recognizer.CropFace(gallery_img_data_color, gallery_points, gallery_img_data_crop_color);
  face_recognizer.CropFace(probe_img_data_color, probe_points, probe_img_data_crop_color);

  cv::imwrite("gallery_point_result.jpg", gallery_img_crop_color);
  cv::imwrite("probe_point_result.jpg", probe_img_crop_color);

  // Extract face identity feature
  float gallery_fea[2048];
  float probe_fea[2048];
  face_recognizer.ExtractFeatureWithCrop(gallery_img_data_color, gallery_points, gallery_fea);
  face_recognizer.ExtractFeatureWithCrop(probe_img_data_color, probe_points, probe_fea);

  // Caculate similarity of two faces
  float sim = face_recognizer.CalcSimilarity(gallery_fea, probe_fea);
  std::cout << sim <<endl;

  return 0;

  cout << files.size() << endl;
  for (int i = 0; i<files.size(); i++)
  {
	  string p;
	  cout << files[i].c_str() << endl;
	  //load image
	  cv::Mat gallery_img_color = cv::imread(SrcPath + "/" + files[i], 1);
	  cv::Mat gallery_img_gray;
	  cv::cvtColor(gallery_img_color, gallery_img_gray, CV_BGR2GRAY);

	  ImageData gallery_img_data_color(gallery_img_color.cols, gallery_img_color.rows, gallery_img_color.channels());
	  gallery_img_data_color.data = gallery_img_color.data;

	  ImageData gallery_img_data_gray(gallery_img_gray.cols, gallery_img_gray.rows, gallery_img_gray.channels());
	  gallery_img_data_gray.data = gallery_img_gray.data;

	  cv::Mat gallery_img_crop_color(256, 256, CV_8UC(3));
	  ImageData gallery_img_data_crop_color(256,256, 3);
	  gallery_img_data_crop_color.data = gallery_img_crop_color.data;
	  cout << "1111111111" << endl;
	  // Detect faces
	  std::vector<seeta::FaceInfo> gallery_faces = detector.Detect(gallery_img_data_gray);
	  int32_t gallery_face_num = static_cast<int32_t>(gallery_faces.size());
	  cout << "11111111112" << endl;

	  if (gallery_face_num == 0 )
	  {
		  std::cout << "Faces are not detected.";
		  //return 0;
		  continue;
	  }
	  cout << "11111111113" << endl;

	  // Detect 5 facial landmarks
	  seeta::FacialLandmark gallery_points[5];
	  point_detector.PointDetectLandmarks(gallery_img_data_gray, gallery_faces[0], gallery_points);
	  cout << "11111111114" << endl;

	  face_recognizer.CropFace(gallery_img_data_color, gallery_points, gallery_img_data_crop_color);
	  cout << "11111111115" << endl;

	  cout << "111111111156" + DstPath + "/" + files[i] << endl;

	   //show crop face
	  //cv::imshow("crop face", gallery_img_crop_color);
	  //cv::waitKey(0);
	  //cv::destroyWindow("crop face");

	  cv::imwrite(DstPath+"/"+files[i], gallery_img_crop_color);
	  cout << "11111111116" + DstPath + "/" + files[i] << endl;

  }
  return 0;
  //// Extract face identity feature
  //float gallery_fea[2048];
  //float probe_fea[2048];
  //face_recognizer.ExtractFeatureWithCrop(gallery_img_data_color, gallery_points, gallery_fea);
  //face_recognizer.ExtractFeatureWithCrop(probe_img_data_color, probe_points, probe_fea);

  //// Caculate similarity of two faces
  //float sim = face_recognizer.CalcSimilarity(gallery_fea, probe_fea);
  //std::cout << sim <<endl;

  //return 0;
}


