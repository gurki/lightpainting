#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
// #include <opencv2/ocl/ocl.hpp>

#include <iostream>
#include <thread>

#include "timer.h"
#include "keys.h"

using namespace std;


// void initOpenCL();

void reduceMax(
    const std::vector<cv::Mat>::iterator begin,
    const std::vector<cv::Mat>::iterator end,
    cv::Mat& out
);

// void oclReduceMax(
//     const std::vector<cv::ocl::oclMat>::iterator begin,
//     const std::vector<cv::ocl::oclMat>::iterator end,
//     cv::ocl::oclMat& out
// );


int main(int argc, char* argv[])
{
    // initOpenCL();

    //  read frames

    const std::string filebase = "etc-tinyplanet";
    std::string filename = ROOT_PATH "data/" + filebase + ".mp4";

    // if ( argc >= 3 && std::string( argv[1] ) == "-i")
    //     filename = argv[2];
    if ( argc >= 2 )
        filename = argv[1];

    Timer timer;
    cv::VideoCapture video( filename );

    const size_t from = 0;
    const int to = -1;
    video.set( cv::CAP_PROP_POS_FRAMES, from );

    cv::Mat frame;
    // std::vector<cv::ocl::oclMat> frames;
    std::vector<cv::Mat> frames;

    const size_t frameCount = video.get( cv::CAP_PROP_FRAME_COUNT );
    const double fps = video.get( cv::CAP_PROP_FPS );
    const int width = video.get( cv::CAP_PROP_FRAME_WIDTH );
    const int height = video.get( cv::CAP_PROP_FRAME_HEIGHT );
    std::cout << frameCount << " frames found" << std::endl;
    std::cout << "loading frames ... " << std::endl;

    cv::namedWindow( "output" );
    cv::Mat composite;

    const int fourcc = cv::VideoWriter::fourcc( 'h', '2', '6', '4');
    const cv::Size size = { width, height };
    cv::VideoWriter writer( ROOT_PATH + filebase + ".mp4", fourcc, fps, size );

    while ( video.read( frame ) )
    {
        const size_t curr = video.get( cv::CAP_PROP_POS_FRAMES );

        if ( curr > to && to >= 0 )
            break;

        cv::Mat preview;
        cv::resize( frame, preview, cv::Size(), 0.5, 0.5, cv::INTER_CUBIC );

        frames.push_back( preview );
        // cv::ocl::oclMat oclPreview( preview );
        // frames.push_back( oclPreview );

        if ( composite.empty() ) {
            composite = frame.clone();
        } else {
            cv::max( frame, composite, composite );
        }


        if ( curr % 16 == 0 ) {
            writer.write( composite );
        }

        cv::imshow( "output", composite );
        cv::waitKey( 1 );

        const float t = std::round( 10000 * curr / (float)frameCount ) / 100.f;
        std::cout << t << "% (" << curr << ")" << std::endl;
    }

    cv::imwrite( ROOT_PATH + filebase + ".png", composite );
    timer.toc();

    if ( frames.empty() ) {
        std::cout << "no frames existent" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "read " << frames.size() << " frames" << std::endl;
    cv::waitKey();
    return EXIT_SUCCESS;

    //  reduce frames

    // const cv::Size size = frames[ 0 ].size();
    int left = 0, right = 1;
    int key = 0;
    int step = 1;

    Keys keys;

    // cv::ocl::oclMat oclComposite = cv::ocl::oclMat( size, frame.type() );
    // cv::Mat composite = cv::Mat( size, frame.type() );

    do
    {
        left = std::min( (int)frames.size(), std::max( 0, left ) );
        right = std::min( (int)frames.size(), std::max( 1, right ) );
        left = std::min( left, right - 1 );

        // composite.setTo( 0 );

        timer.tic();
        // oclReduceMax( frames.begin() + left, frames.begin() + right, oclComposite );
        // reduceMax( frames.begin(), frames.end(), composite );

        // std::cout << "window: ( " << left << ", " << right << " ), dt: " << timer.tac() << std::endl;

        //  show result

        // oclComposite.download( composite );
        cv::imshow( "output", composite );
        key = cv::waitKey( 1 );

        if ( ( key < '0' || key > '9' ) && key != 'r' && key != 'f' )
            keys.down( key );

        //  adjust window

        if ( keys.isPressed( 'd' ) )    //  right bwd
            right -= step;

        if ( keys.isPressed( 'e' ) )    //  right fwd
            right += step;

        if ( keys.isPressed( 'a' ) )    //  left bwd
            left -= step;

        if ( keys.isPressed( 'q' ) )    //  left fwd
            left += step;

        if ( keys.isPressed( 's' ) ) {  //  move fwd
            left -= step;
            right -= step;
        }

        if ( keys.isPressed( 'w' ) ) {  //  move bwd
            left += step;
            right += step;
        }

        switch ( key )
        {
            case 'r': left = right - 1; break;
            case 'f': right = left + 1; break;
            case '1': step = 1; break;
            case '2': step = 2; break;
            case '5': step = 5; break;
            case '0': step = 10; break;
            case 63232: left += step; right += step; break;
            case 63233: left -= step; right -= step; break;
            case 63234: step = std::max( 1, step - 1 ); std::cout << step << std::endl; break;
            case 63235: step++; std::cout << step << std::endl; break;
        }
    }
    while ( key != 27 );

    return 0;
}

// void oclReduceMax(
//     const std::vector<cv::ocl::oclMat>::iterator begin,
//     const std::vector<cv::ocl::oclMat>::iterator end,
//     cv::ocl::oclMat& out )
// {
//     out.setTo( 0 );

//     const size_t n = std::distance( begin, end );

//     if ( n == 0 ) {
//         return;
//     }
//     auto iter = begin;

//     while ( iter != end ) {
//         cv::ocl::max( *iter, out, out );
//         iter++;
//     }
// }


void reduceMax(
    const std::vector<cv::Mat>::iterator begin,
    const std::vector<cv::Mat>::iterator end,
    cv::Mat& out )
{
    const size_t n = std::distance( begin, end );

    if ( n == 0 ) {
        return;
    }

    const size_t len = 512;

    //  1024 - tictoc: 0.918506s
    //  512 - tictoc: 0.892084s
    //  256 - tictoc: 0.923865s
    //  128 - tictoc: 0.953859s
    //  64 - tictoc: 0.988026s
    //  32 - tictoc: 1.07413s
    //  16 - tictoc: 1.52617s
    //  8 - tictoc: 2.96731s

    if ( n < len )
    {
        auto iter = begin;

        while ( iter != begin + len && iter != end ) {
            cv::max( *iter, out, out );
            iter++;
        }

        return;
    }

    // if ( n == 1 )  {
    //     begin->copyTo( out );
    //     return;
    // }
    //
    // if ( n == 2 ) {
    //     cv::max( *begin, *(begin + 1), out );
    //     return;
    // }

    cv::Mat left = cv::Mat::zeros( begin->size(), begin->type() );
    cv::Mat right = cv::Mat::zeros( begin->size(), begin->type() );

    std::thread tl( reduceMax, begin, begin + n / 2 + 1, std::ref( left ) );
    std::thread tr( reduceMax, begin + n / 2 + 1, end, std::ref( right ) );

    tl.join();
    tr.join();

    cv::max( left, right, out );
}


// void initOpenCL()
// {
//     std::cout << "OpenCL v" << CV_VERSION_EPOCH << "." << CV_VERSION_MAJOR << "." << CV_VERSION_MINOR << std::endl;

//     cv::ocl::DevicesInfo devInfo;
//     int res = cv::ocl::getOpenCLDevices(devInfo);

//     if( res == 0 ) {
//         std::cerr << "There is no OPENCL Here !" << std::endl;
//         return;
//     }

//     for( size_t i = 0; i < devInfo.size(); i++ )
//         std::cout << "[" << i << "] " << devInfo[ i ]->deviceName << " is present" << std::endl;

//     const int dev = 1;
//     std::cout << "[" << dev << "] selected" << std::endl;
//     cv::ocl::setDevice( devInfo[ dev ] );        // select device to use
// }
