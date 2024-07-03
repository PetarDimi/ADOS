
#include "ImageProcessing.h"
#include "ColorSpaces.h"
#include "Augmentations.h"
#include <cstdlib>

#include <QDebug>
#include <QLabel>
#include <QImage>

void imageProcessingFun(
	const QString& prog_name,
	QImage& out_img,
    const QImage& in_img,
    const QVector<double>& params
) {
	int x_size = in_img.width();
	int y_size = in_img.height();

    if(prog_name == "HueOffset"){
        out_img = QImage(x_size, y_size, in_img.format());

        QImage rgbImg = in_img.convertToFormat(QImage::Format_RGB888);
        const uchar* rgbData = rgbImg.constBits();

        float hueOffset = params[0];
        float saturationScale = 1;

        //Calling HueOffset
        HueOffset(rgbData, x_size, y_size, hueOffset, saturationScale, out_img.bits());
		
    }else if(prog_name == "Cutout"){

        QImage rgbImg = in_img.convertToFormat(QImage::Format_RGB888);

        int x_size = rgbImg.width();
        int y_size = rgbImg.height();

        const uchar* rgbData = rgbImg.constBits();


        //float new_x_size = x_size;
        //float new_y_size = (y_size / 3) * 2;
        //float center_x = x_size + (x_size / 2);
        float center_y = y_size / 2;

        /*float new_x_size = (rand() % (700 - 200 + 1)) + 200;
        float fmin_x = new_x_size/2;
        int min_x = (int)fmin_x;
        float fmax_x = x_size * 3;
        int imax_x = (int)fmax_x;
        int max_x = imax_x - min_x;
        float new_y_size = 300;
        float center_x = (rand() % (max_x-min_x + 1)) + min_x;*/
        //float center_y = (rand() % ((y_size * 3 - 200)-200 + 1)) + 200;

        int new_x_size = random(200, 700);
        int min_x = 400;
        int max_x = 2000;
        float new_y_size = 300;
        int center_x = (random(min_x, max_x));


        out_img = QImage(new_x_size, new_y_size, in_img.format());

        cropAndStretchImage(rgbData, x_size,y_size, out_img.bits(), new_x_size, new_y_size, center_x, center_y);

	}
    else if(prog_name == "Occlusions"){

        out_img = QImage(x_size, y_size, in_img.format());

        int x_size = in_img.width();
        int y_size = in_img.height();

        QImage rgbImg = in_img.convertToFormat(QImage::Format_RGB888);
        const uchar* rgbData = rgbImg.constBits();

        int center_x = params[0];
        int center_y = params[1];

        int occlusion_width = params[2];
        int occlusion_height = params[3];

        //Calling Occlusions
        Occlusions(rgbData, out_img.bits(), x_size, y_size, center_x, center_y, occlusion_width, occlusion_height);

    }

    else if(prog_name == "DCTLowpassCompression"){
        int B = params[0];
        double strength = params[1];
        out_img = QImage(x_size, y_size, in_img.format());


        uchar* Y_buff = new uchar[x_size*in_img.height()];
        char* U_buff = new char[x_size*in_img.height()/4];
        char* V_buff = new char[x_size*in_img.height()/4];

        RGBtoYUV420(in_img.bits(), x_size, y_size, Y_buff, U_buff, V_buff);

        PerformDCT(Y_buff, x_size, y_size, B, strength);

        procesing_YUV420(Y_buff, U_buff, V_buff, x_size, y_size, 1, 1, 1);

        YUV420toRGB(Y_buff, U_buff, V_buff, x_size, y_size, out_img.bits());

        delete[] Y_buff;
        delete[] U_buff;
        delete[] V_buff;
    }

    else if(prog_name == "ColorBasedkMeans"){

        out_img = QImage(x_size, y_size, in_img.format());

        int x_size = in_img.width();
        int y_size = in_img.height();

        QImage rgbImg = in_img.convertToFormat(QImage::Format_RGB888);
        uchar* rgbData = rgbImg.bits();

        int clusters = params[0];

        ColorBasedKMeans(rgbData, x_size, y_size, clusters, out_img.bits());
    }

}
