#include <opencv2/opencv.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <chrono>

using namespace cv;


int main() {
    // Video dosyasını yükle
    cv::VideoCapture cap("v4l2src device=/dev/video0 ! image/jpeg, framerate=30/1,width=1280,height=720,format=(string)MJPG ! jpegdec ! videoconvert ! appsink");
    if (!cap.isOpened()) {
        std::cout << "Video dosyası yüklenemedi!" << std::endl;
        return -1;
    }
    
    // İlk kareyi oku
    cv::Mat frame;
    cap >> frame;

    int width = frame.cols;
    int height = frame.rows;
    int k = 4;
    cv::namedWindow("Nesne Takibi");

    // İzlenecek nesneyi seçin
    cv::Rect2d boundingBox = cv::selectROI("Nesne Takibi",frame, false);
    boundingBox.x = boundingBox.x/4;
    boundingBox.y = boundingBox.y/4;
    boundingBox.width = boundingBox.width/4;
    boundingBox.height = boundingBox.height/4;

    // CSRT takipçi nesnesini oluştur
    Ptr<Tracker> tracker = TrackerCSRT::create();
    cv::resize(frame, frame, cv::Size(width/k, height/k));

    // Nesneyi takip etmek için başlangıç noktasını ve boyutunu ayarla
    tracker->init(frame, boundingBox);

    // Videoyu oynat ve nesneyi takip et
    while (true) {
        // Başlangıç zamanını kaydet
        cap >> frame;
        auto start = std::chrono::high_resolution_clock::now();

        cv::resize(frame, frame, cv::Size(width/4, height/4));
        
        // Nesneyi güncelle
        
        bool success = tracker->update(frame, boundingBox);
        boundingBox.x = boundingBox.x*4;
        boundingBox.y = boundingBox.y*4;
        boundingBox.width = boundingBox.width*4;
        boundingBox.height = boundingBox.height*4;
        cv::resize(frame, frame, cv::Size(width, height));
        // Bitiş zamanını kaydet
        auto end = std::chrono::high_resolution_clock::now();
        
        // Süreyi hesapla
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        if (success) {
            // Nesneyi çerçeve üzerinde göster
            cv::rectangle(frame, boundingBox, cv::Scalar(255, 0, 0), 2, 1);
        }

        // Çerçeveyi göster
        cv::imshow("Nesne Takibi", frame);

        // Süreyi yazdır
        std::cout << "Takip süresi: " << duration.count() << " ms" << std::endl;

        // 'q' tuşuna basıldığında döngüden çık
        if (cv::waitKey(1) == 'q')
            break;
    }

    // Pencereyi kapat
    cv::destroyAllWindows();

    return 0;
}
