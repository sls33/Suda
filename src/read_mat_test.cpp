#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    // 读取SVHN数据集文件
    cv::FileStorage fs("../python/data/train_32x32.mat", cv::FileStorage::READ);

    if (!fs.isOpened()) {
        std::cerr << "Failed to open SVHN dataset file." << std::endl;
        return -1;
    }

    // 读取数字图像数据
    cv::Mat images;
    fs["X"] >> images;

    // 读取对应的数字标签
    cv::Mat labels;
    fs["y"] >> labels;

    // 输出图像和标签信息
    std::cout << "Number of images: " << images.rows << std::endl;
    std::cout << "Number of labels: " << labels.rows << std::endl;

    // 显示第一张图像
    cv::Mat firstImage = images.row(0);
    cv::Mat reshapedImage;
    cv::resize(firstImage, reshapedImage, cv::Size(32, 32));
    cv::imshow("First Image", reshapedImage);
    cv::waitKey(0);

    return 0;
}