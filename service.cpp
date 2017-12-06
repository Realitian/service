/**************************************************************
 * Project          Augmented Reality Visit Assistance
 * (c) Copyright    2017
 * Company          Infinte Reality Solution LLC
 *                  All rights reserved
 **************************************************************/

#include "service.h"
#include <iostream>
#include <fstream>
#include "ZlibUtil.h"
#include <opencv2/opencv.hpp>

std::string inputPath = "/sandbox/tracking3d/aws_env/dl-crfrnn-caffe-g3/input.jpg";
std::string outputPath = "/sandbox/tracking3d/aws_env/dl-crfrnn-caffe-g3/output.png";
std::string command = "convert " + inputPath + " " + outputPath;

void exec_python();

using boost::asio::ip::tcp;

boost::asio::io_service io_service;
tcp::socket socket_(io_service);

void accep_handler( const boost::system::error_code &ec );
void do_process( );

void start()
{
    tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), 10000));
    acceptor_.async_accept(socket_, accep_handler);
    io_service.run();
}

void accep_handler(const boost::system::error_code &ec)
{
    if (ec)
        return;

    while(1)
        do_process();
}

void do_process()
{
    int compressedBytes = 640*480*3;
    char* compressedData = new char[compressedBytes];
    int readBufferSize = 0;
    size_t rb = socket_.read_some(boost::asio::buffer(compressedData, compressedBytes));
    readBufferSize += rb;
    std::cout << "log point 1 " << rb << std::endl;
//    while( rb )
//    {
//        rb = socket_.read_some(boost::asio::buffer(compressedData+rb, 1024));
//        readBufferSize += rb;
//        std::cout << "log point 1 : " << readBufferSize << std::endl;
//    }

    std::cout << "log point 2 " << std::endl;

    int imageBytes = 640*480*3;
    char* imageData = new char[imageBytes];
    int originSz = uncompressData(compressedData, readBufferSize, imageData, imageBytes);

    std::cout << "uncompressed recv size : " << originSz << std::endl;

    int type = ((int*)imageData)[0];
    int rows = ((int*)imageData)[1];
    int cols = ((int*)imageData)[2];
    char* data = (char*)((void*)imageData + sizeof(int)*3);

    cv::Mat inpuImage(rows, cols, type, data);

    std::cout << "input image : " << rows << ", " << cols << ", " << type << std::endl;
    cv::imwrite(inputPath, inpuImage);

    std::cout << "log point 3 " << std::endl;
    exec_python();
    std::cout << "log point 4 " << std::endl;

    cv::Mat outputImage = cv::imread(outputPath);
    type  = outputImage.type();
    rows = outputImage.rows;
    cols = outputImage.cols;

    int outputBytes = outputImage.total() * outputImage.elemSize();
    char* outputData = new char[outputBytes+sizeof(int)*3];
    memcpy( (void*)outputData, &type, sizeof(int) );
    memcpy( (void*)outputData + sizeof(int), &rows, sizeof(int) );
    memcpy( (void*)outputData + sizeof(int)*2, &cols, sizeof(int) );
    memcpy( (void*)outputData + sizeof(int)*3, outputImage.data, outputBytes );

    uint8_t * compressedBuffer = new uint8_t[outputBytes];
    int compressedLength = compressData(outputData, outputBytes+sizeof(int)*3, compressedBuffer, outputBytes);

    std::cout << "log point 5 " << std::endl;
   boost::asio::write(socket_, boost::asio::buffer(compressedBuffer, compressedLength));
   std::cout << "log point 6 " << std::endl;
}

void exec_python()
{
    system( command.c_str() );
}
