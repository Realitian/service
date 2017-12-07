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

int main(int argc, char *argv[])
{
    if ( argc > 2 )
    {
        inputPath = argv[1];
        outputPath = argv[2];
    }

    if ( argc > 3 )
    {
        command = argv[3];
    }

    try
    {
        start();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

void start()
{
    boost::asio::io_service io_service;
    tcp::endpoint endpoint(tcp::v4(), 10000);
    tcp::acceptor acceptor(io_service, endpoint);

    while(true){
        tcp::socket socket(io_service);
        acceptor.accept(socket);

        int compressedBytes = 0;
        socket.read_some(boost::asio::buffer(&compressedBytes, sizeof(int)));

        char* compressedData = new char[compressedBytes];

        int readBufferSize = 0;
        int buffer_size = 1024*1024;

        while( readBufferSize < compressedBytes )
        {
            boost::system::error_code error;

            int len = socket.read_some(boost::asio::buffer(compressedData+readBufferSize, buffer_size), error);

            if ( error == boost::asio::error::eof )
                break;
            else if ( error )
                throw boost::system::system_error(error);

            readBufferSize += len;
        }

        int imageBytes = 640*480*3;
        char* imageData = new char[imageBytes];
        uncompressData(compressedData, readBufferSize, imageData, imageBytes);

        int type = ((int*)imageData)[0];
        int rows = ((int*)imageData)[1];
        int cols = ((int*)imageData)[2];
        char* data = (char*)((void*)imageData + sizeof(int)*3);

        cv::Mat inpuImage(rows, cols, type, data);

        cv::imwrite(inputPath, inpuImage);

        exec_python();

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

        write(socket, boost::asio::buffer(&compressedLength, sizeof(int)), boost::asio::transfer_all());
        boost::asio::write(socket, boost::asio::buffer(compressedBuffer, compressedLength), boost::asio::transfer_all());
    }
}

void exec_python()
{
    system( command.c_str() );
}
